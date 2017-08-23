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

bytevector key = random_key();

int main() {
  unsigned min_length = static_cast<unsigned>(-1);
  vector<bytevector> ciphertexts;
  std::ifstream infile("data/problem20.data");
  std::string line;
  while (std::getline(infile, line)) {
    ciphertexts.push_back(bytevector(line, bytevector::BASE64));
    if (ciphertexts.back().size() < min_length)
      min_length = ciphertexts.back().size();
  }

  bytevector mega_ct;
  for (bytevector &ct : ciphertexts) {
    ct.resize(min_length);
    mega_ct += ct;
  }

  string key = solve_repeating_key_xor(mega_ct, min_length);
  cout << "Key: " << key << endl;
  bytevector bvKey(key, bytevector::PLAIN);
  for (const bytevector &ct : ciphertexts) {
    cout << (ct ^ bvKey) << endl;
  }

  return 0;
}
