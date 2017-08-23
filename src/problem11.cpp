#include <iostream>
#include <string>
#include <vector>

#include "bytevector.h"
#include "Crypto.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

static Crypto cr;

bytevector random_encrypt(bytevector data) {
  bytevector key = random_key(16);

  bytevector plaintext;
  plaintext += random_string(5, 10);
  plaintext += data;
  plaintext += random_string(5, 10);
  plaintext.pad_to_block(16);

  bytevector ciphertext;
  if (random() % 2) {
    ciphertext = cr.encrypt_ecb(plaintext, key, false);
    cout << "Real:  ECB" << endl;
  } else {
    bytevector iv = random_key(16);
    ciphertext = cr.encrypt_cbc(plaintext, key, iv);
    cout << "Real:  CBC" << endl;
  }

  return ciphertext;
}

int main() {
  srand(time(NULL));
  bytevector input(43, 'q'); 
  bytevector res = random_encrypt(input);
  vector<bytevector> blocks = res.split_into_blocks(16);
  if (blocks[1] == blocks[2]) {
    cout << "Guess: ECB" << endl;
  } else {
    cout << "Guess: CBC" << endl;
  }
  return 0;
}
