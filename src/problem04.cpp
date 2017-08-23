#include <iostream>
#include <array>
#include <string>
#include <fstream>

#include "bytevector.h"

using std::array;
using std::string;

int main() {
  std::ifstream input("problem4.data");
  bytevector best_output;
  double best_error = -1;
  string input_line;
  while (input >> input_line) {
    bytevector input_vector = hex_to_bytevector(input_line);
    for (char c = 20; c < 126; c++) {
      bytevector mask(input_vector.size(), c);
      bytevector decrypted = input_vector ^ mask;
      auto frequencies = letter_frequencies(decrypted);
      double error = squared_error(frequencies);
      if (error < best_error || best_error == -1) {
        best_output = decrypted;
        best_error = error;
      }
    }
  }

  std::cout << bytevector_to_string(best_output);
  return 0;
}
