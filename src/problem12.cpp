#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "bytevector.h"
#include "Crypto.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace {
  bytevector key = random_key();
  Crypto cr;
}

bytevector encrypt(bytevector plaintext) {
  std::ifstream in_file("data/problem12.data");
  bytevector sec(in_file);
  bytevector ciphertext = cr.encrypt_ecb(plaintext + sec, key, true);
  return ciphertext;
}

int main() {
  int block_size;
  unsigned start_len = encrypt(bytevector()).size();
  for (int i = 1; i < 128; i++) {
    bytevector input(i, 'A');
    bytevector res = encrypt(input);
    if (start_len < res.size()) {
      block_size = res.size() - start_len;
      break;
    }
  }
  cout << "Block size: " << block_size << endl;

  bytevector input(block_size * 2, 'A');
  bytevector res = encrypt(input);
  vector<bytevector> blocks = res.split_into_blocks(block_size);
  cout << "Block mode: ";
  if (blocks[0] == blocks[1]) {
    cout << "ECB" << endl;
  } else {
    cout << "CBC" << endl;
  }

  cout << "Start len: " << start_len << endl << endl;
  bytevector decrypted;
  for (unsigned i = 0; i < start_len; i++) {
    int block_offset = i % block_size;
    int block_index = i / block_size;

    bytevector padding(16 - block_offset - 1, 'q');
    res = encrypt(padding);
    vector<bytevector> enc_blocks = res.split_into_blocks(block_size);

    byte b;
    for (b = 10; b <= 126; b++) {
      bytevector test_res = encrypt(padding + decrypted + b);
      vector<bytevector> test_blocks = test_res.split_into_blocks(block_size);
      if (test_blocks[block_index] == enc_blocks[block_index]) {
        decrypted.push_back(b);
        break;
      }
    }
    if (b == 126) break;
  }

  cout << "Decrypted: " << decrypted << endl;
  return 0;
}
