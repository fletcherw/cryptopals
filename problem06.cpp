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
    double dist = 0;
    bytevector a(it, it + key_size);
    it += key_size;

    bytevector b(it, it + key_size);
    it += key_size;

    bytevector c(it, it + key_size);
    it += key_size;

    bytevector d(it, it + key_size);

    dist += (hamming_distance(a, b) / (double) key_size);
    dist += (hamming_distance(a, c) / (double) key_size);
    dist += (hamming_distance(a, d) / (double) key_size);
    dist += (hamming_distance(b, c) / (double) key_size);
    dist += (hamming_distance(b, d) / (double) key_size);
    dist += (hamming_distance(c, d) / (double) key_size);

    keys.push_back(std::make_pair(dist, key_size));
  }
  std::sort(keys.begin(), keys.end());
  int key_size = keys[0].second;
  cout << "key_size: " << key_size << endl;

  string key = solve_repeating_key_xor(data, key_size);
  cout << "key: " << key << endl;

  bytevector plain = repeating_key_xor(data, key);
  cout << bytevector_to_string(plain) << endl;
  return 0;
}
