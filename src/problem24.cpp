#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "bytevector.h"
#include "Crypto.h"
#include "MT19937.h"

using std::vector;
using std::cout;
using std::endl;

namespace {

Crypto cr;

}

bool is_time_token(bytevector bv) {
  uint32_t time = std::chrono::seconds(std::time(NULL)).count();
  for (uint32_t seed = time; seed > (time - (60 * 60 * 24)); seed--) {
    bytevector token = mt19937_token(256, seed);
    if (token == bv) {
      cout << "Validated Seed is: " << seed << endl;
      return true;
    }
  }
  return false;
}

int main() {
  bytevector prefix = random_string(10, 256);
  bytevector known(14, 'A');
  std::random_device rd;
  std::mt19937 mt(rd());

  cout << "Breaking Stream Cipher: " << endl;
  uint16_t seed = mt() % (1u << 16);
  cout << "Seed is " << seed << endl; 

  bytevector ciphertext = cr.encrypt_mt(prefix + known, seed);

  ciphertext.resize(ciphertext.size() - ciphertext.size() % 4);
  vector<bytevector> blocks = ciphertext.split_into_blocks(4);
  uint32_t state = (blocks.back() ^ bytevector(4, 'A')).to_uint32(); 
  unsigned steps = blocks.size(); 

  for (unsigned seed = 0; seed < (1u << 16); seed++) {
    MT19937 m(seed);
    uint32_t value;
    for (unsigned i = 0; i < steps; i++) value = m(); 
    if (value == state) {
      cout << "Predicted seed is " << seed << endl; 
      break;
    }
  }
  cout << endl;
  cout << "Checking password reset token:" << endl;
  uint32_t timeSeed = std::chrono::seconds(std::time(NULL)).count();
  cout << "Token seed is " << timeSeed << endl; 
  bytevector token = mt19937_token(256, timeSeed);
  bytevector fake_token = random_string(256, 256);

  cout << "Real token passes: " << is_time_token(token) << endl;
  cout << "Fake token passes: " << is_time_token(fake_token) << endl;

  return 0;
}
