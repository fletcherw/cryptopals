#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <iomanip>

#include "bytevector.h"
#include "Crypto.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace {

std::random_device rd;
std::mt19937 gen(rd());

bytevector key = random_key();
bytevector iv = random_key();
Crypto cr;

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

bytevector get_cipher_text() {
  std::uniform_int_distribution<> dis(0, strings.size() - 1);
  string input = strings[dis(gen)];
  bytevector plaintext(input, bytevector::BASE64);
  plaintext.pad_to_block(16);
  bytevector ciphertext = cr.encrypt_cbc(plaintext, key, iv);
  return ciphertext;
}

bool decrypt(bytevector ciphertext) {
  bytevector plaintext = cr.decrypt_cbc(ciphertext, key, iv);
  return plaintext.check_padding();
}

int main() {
  cout << "Key: " << key << endl;
  cout << "IV: "  << iv  << endl;

  bytevector ciphertext = get_cipher_text();
  cout << ciphertext << endl;
  vector<bytevector> blocks = ciphertext.split_into_blocks(16);
  bytevector prev = iv;
  bytevector plaintext;
  for (const bytevector& block : blocks) {
    bytevector plain_block(16, '_');
    bytevector corrupter(16, '\0');
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
              continue;
            }
          }
          plain_block[i] = guess ^ prev[i];
          std::cout << std::setw(2) << i << std::setw(0)
                    << " " << plain_block << " " << guess << endl;
          corrupter[i] ^= guess ^ (16 - i);
          break;
        }
      }
    }
    cout << endl;
    plaintext += plain_block;
    prev = block;
  }

  plaintext.strip_padding();
  cout << plaintext << endl;
  return 0;
}
