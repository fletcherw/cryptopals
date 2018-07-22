#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bytevector.h"
#include "Crypto.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

bytevector key;

int main() {
  key = random_bytevector();
  Crypto cr;

  unsigned min_length = static_cast<unsigned>(-1);
  vector<bytevector> ciphertexts;
  std::ifstream infile("data/problem20.data");
  std::string line;
  while (std::getline(infile, line)) {
    bytevector plaintext(line, bytevector::BASE64);
    cout << plaintext.to_string(bytevector::ASCII) << endl;
    ciphertexts.push_back(cr.encrypt_ctr(plaintext, key, 0));
    if (ciphertexts.back().size() < min_length)
      min_length = ciphertexts.back().size();
  }

  bytevector mega_ct;
  for (bytevector &ct : ciphertexts) {
    ct.resize(min_length);
    mega_ct += ct;
  }

  bytevector xorKey = solve_repeating_key_xor(mega_ct, min_length);
  cout << "Key: " << xorKey << endl << endl;

  for (const bytevector &ct : ciphertexts) {
    cout << (ct ^ xorKey).to_string(bytevector::ASCII) << endl;
  }

  return 0;
}
