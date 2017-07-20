#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>

#include "bytevector.h"

using std::string;
using std::array;
using std::vector;
using std::cout;
using std::endl;

bytevector random_encrypt(bytevector data) {
  array<byte, 16> key = random_key();

  bytevector pt;
  int prepad = random() % 6 + 5;
  for (int i = 0; i < prepad; i++) pt.push_back(rand() % 95 + 32);
  for (byte b : data) pt.push_back(b);
  int postpad = random() % 6 + 5;
  for (int i = 0; i < postpad; i++) pt.push_back(rand() % 95 + 32);

  bytevector ct;
  if (random() % 2) {
    ct = encrypt_ecb(pt, key.data(), true); 
    cout << "Real:  ECB" << endl;
  } else {
    array<byte, 16> iv = random_key();
    ct = encrypt_cbc(pt, key.data(), iv.data()); 
    cout << "Real:  CBC" << endl;
  }

  return ct; 
}

int main() {
  srandom(time(NULL));
  bytevector input = string_to_bytevector(
      "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq");
  bytevector res = random_encrypt(input);
  vector<bytevector> blocks = split_into_blocks(res, 16);
  if (blocks[1] == blocks[2]) {
    cout << "Guess: ECB" << endl;
  } else { 
    cout << "Guess: CBC" << endl;
  }
  return 0;
}
