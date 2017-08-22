#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

#include "bytevector.h"

using std::string;
using std::array;
using std::vector;
using std::cout;
using std::endl;

array<byte, 16> key = random_key();

int main() {
  unsigned min_length = static_cast<unsigned>(-1);
  vector<bytevector> ciphertexts;
  std::ifstream infile("problem20.data");
  std::string line;
  while (std::getline(infile, line)) {
    ciphertexts.push_back(base64_to_bytevector(line));
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
  bytevector bvKey = string_to_bytevector(key);
  for (const bytevector &ct : ciphertexts) {
    cout << bytevector_to_string(ct ^ bvKey) << endl;
  }

  return 0;
}
