#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "bytevector.h"

using std::vector;
using std::cout;
using std::endl;

bool is_time_token(bytevector bv) {
  uint32_t time = std::chrono::seconds(std::time(NULL)).count();
  for (uint32_t seed = time; seed > (time - (60 * 60 * 24)); seed--) {
    bytevector token = mt19937_token(256, seed);
    if (token == bv) {
      cout << "Seed is: " << seed << endl;
      return true;
    }
  }
  return false;
}

int main() {
  bytevector prefix = random_string();
  bytevector known(14, 'A');
  std::random_device rd;
  std::mt19937 mt(rd());

  cout << "Breaking Stream Cipher: " << endl;
  uint16_t seed = mt() % (1u << 16);
  cout << "Seed is " << seed << endl; 

  bytevector ct = encrypt_mt_stream(prefix + known, seed);

  ct.resize(ct.size() - ct.size() % 4);
  vector<bytevector> blocks = split_into_blocks(ct, 4);
  uint32_t state = bytevector_to_int(blocks.back() ^ bytevector(4, 'A')); 
  unsigned steps = blocks.size(); 

  for (unsigned seed = 0; seed < (1u << 16); seed++) {
    MT19937 m(seed);
    uint32_t value;
    for (unsigned i = 0; i < steps; i++) value = m.next(); 
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
  bytevector fake_token = random_string(256);

  cout << "Real token passes: " << is_time_token(token) << endl;
  cout << "Fake token passes: " << is_time_token(fake_token) << endl;

  return 0;
}
