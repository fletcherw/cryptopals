#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <iomanip>

#include "bytevector.h"

using std::string;
using std::array;
using std::vector;
using std::cout;
using std::endl;

namespace {
  std::random_device rd;
  std::mt19937 gen(rd());

  array<byte, 16> key = random_key();
  array<byte, 16> iv = random_key();
}

vector<string> strings =
  {"MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc=",
   "MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic=",
   "MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw==",
   "MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg==",
   "MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl",
   "MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA==",
   "MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw==",
   "MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8=",
   "MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g=",
   "MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93"};

bytevector getCipherText() {
  std::uniform_int_distribution<> dis(0, strings.size() - 1);
  string input = strings[dis(gen)];
  bytevector plaintext = pad_to_block(base64_to_bytevector(input), 16);
  bytevector ciphertext = encrypt_cbc(plaintext, key.data(), iv.data());
  return ciphertext;
}

bool decrypt(bytevector ciphertext) {
  bytevector plaintext = decrypt_cbc(ciphertext, key.data(), iv.data());
  return check_padding(plaintext);
}

int main() {
  crypto_init();

  cout << std::string(key.data(), key.data() + 16) << endl;
  cout << std::string(iv.data(), iv.data() + 16) << endl;

  bytevector ciphertext = getCipherText();
  cout << ciphertext << endl;
  vector<bytevector> blocks = split_into_blocks(ciphertext, 16);
  bytevector prev = bytevector(iv.data(), iv.data() + 16);
  bytevector plaintext;
  for (const bytevector& block : blocks) {
    bytevector plain_block = bytevector(16, '_');
    bytevector corrupter = bytevector(16, '\0');
    for (int i = 15; i >= 0; i--) {
      for (int j = 15; j > i; j--) {
        corrupter[j] ^= (16 - i - 1);
        corrupter[j] ^= (16 - i);
      }

      for (unsigned guess = 0; guess <= 255; guess++) {
        bytevector test_corrupter = corrupter;
        test_corrupter[i] ^= guess ^ (16 - i);
        if (decrypt(test_corrupter + block)) {
          if (i > 0) {
            test_corrupter[i-1] ^= (i+1);
            bool success = decrypt(test_corrupter + block);
            test_corrupter[i-1] ^= (i+1);
            if (!success) {
              cout << "FAIL" << endl;
              continue;
            }
          }
          plain_block[i] = guess ^ prev[i];
          std::cout << std::setw(2) << i << std::setw(0)
                    << " " << bytevector_to_string(plain_block) << " " << guess << endl;
          corrupter[i] ^= guess ^ (16 - i);
          break;
        }
      }
    }
    cout << endl;
    plaintext += plain_block;
    prev = block;
  }

  cout << bytevector_to_string(strip_padding(plaintext)) << endl;
  crypto_cleanup();
  return 0;
}
