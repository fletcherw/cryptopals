#include <iostream>
#include <fstream>
#include <string>

#include "bytevector.h"

using std::string;
using std::cout;
using std::endl;

int main() {
  std::ifstream input_file("data/problem06.data");
  bytevector data(input_file);

  unsigned min_dist = static_cast<unsigned>(-1);
  unsigned min_size;

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

    if (dist < min_dist) {
      min_dist = dist;
      min_size = key_size;
    }
  }
  cout << "Key Size: " << min_size << endl;

  bytevector key = solve_repeating_key_xor(data, min_size);
  cout << "Key: " << key.to_string(bytevector::ASCII) << endl;

  data.repeating_key_xor(key);
  cout << "Plaintext: " << endl << data.to_string(bytevector::ASCII) << endl;
  return 0;
}
