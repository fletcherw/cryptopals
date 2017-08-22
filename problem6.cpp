#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdexcept>

#include "bytevector.h"

using std::vector;
using std::pair;
using std::string;
using std::cout;
using std::endl;

int main() {
  bytevector data = base64_file_to_bytevector("problem6.data");

  vector<pair<double, int>> keys;
  for (int key_size = 2; key_size < 40; key_size++) {
    auto it = data.begin();

    int rounds = 6;
    double dist = 0;
    for (int i = 0; i < rounds; i++) {
      bytevector a(it, it + key_size);
      it += key_size;
      bytevector b(it, it + key_size);
      it += key_size;

      dist += (hamming_distance(a, b) / (double) key_size);
    }

    keys.push_back(std::make_pair(dist, key_size));
  }
  std::sort(keys.begin(), keys.end());

  vector<bytevector> blocks = split_into_blocks(data, keys[0].second);
  vector<bytevector> by_index = transpose(blocks);
  std::string key;
  for (bytevector bv : by_index) {
    char best_key;
    double best_error = -1;
    for (char c = 32; c < 126; c++) {
      bytevector mask(bv.size(), c);
      bytevector decrypted = bv ^ mask;
      auto frequencies = letter_frequencies(decrypted);
      double error = squared_error(frequencies);
      if (error < best_error || best_error == -1) {
        best_key = c;
        best_error = error;
      }
    }
    key += best_key;
  }

  bytevector plain = repeating_key_xor(data, key);
  cout << bytevector_to_string(plain);
  return 0;
}
