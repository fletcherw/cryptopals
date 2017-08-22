#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>

#include "bytevector.h"

using std::string;
using std::array;
using std::vector;
using std::cout;
using std::endl;

array<byte, 16> key = random_key();
bytevector random_prefix = random_string();

bytevector encrypt(bytevector plaintext) {
  bytevector sec = base64_file_to_bytevector("problem12.data");
  bytevector ciphertext =
    encrypt_ecb(random_prefix + plaintext + sec, key.data(), true);
  return ciphertext;
}

int main() {
  crypto_init();

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
  bytevector align_padding;
  unsigned prefix_bytes = 0;
  bool found = false;
  while (!found) {
    bytevector res = encrypt(align_padding + input);
    vector<bytevector> blocks = split_into_blocks(res, block_size);
    for (unsigned i = 0; i < blocks.size() - 1; i++) {
      if (blocks[i] == blocks[i+1]) {
        cout << "Prefix size: " << i * block_size - align_padding.size() << endl;
        prefix_bytes = i * block_size;
        found = true;
      }
    }
    if (found) break;
    align_padding += 'q';
  }

  cout << "Start len: " << start_len << endl << endl;
  bytevector decrypted;
  bytevector res;
  for (unsigned i = prefix_bytes; i < start_len; i++) {
    int block_offset = i % block_size;
    int block_index = i / block_size;

    bytevector padding(block_size - block_offset - 1, 'q');
    res = encrypt(align_padding + padding);
    vector<bytevector> enc_blocks = split_into_blocks(res, block_size);

    byte b;
    for (b = 10; b <= 126; b++) {
      bytevector test_res = encrypt(align_padding + padding + decrypted + b);
      vector<bytevector> test_blocks = split_into_blocks(test_res, block_size);
      if (test_blocks[block_index] == enc_blocks[block_index]) {
        decrypted.push_back(b);
        break;
      }
    }
    if (b == 126) break;
  }

  cout << "Decrypted: " << bytevector_to_string(decrypted) << endl;
  crypto_cleanup();
  return 0;
}
