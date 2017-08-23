#include <fstream>
#include <iostream>
#include <string>

#include "bytevector.h"

int main() {
  std::ifstream input_file("data/problem04.data");
  bytevector best_output;
  double best_error = -1;
  std::string input_line;
  while (input_file >> input_line) {
    bytevector input(input_line, bytevector::HEX);
    for (char c = 20; c < 126; c++) {
      bytevector mask(input.size(), c);
      bytevector decrypted = input ^ mask;
      auto frequencies = letter_frequencies(decrypted);
      double error = squared_error(frequencies);
      if (error < best_error || best_error == -1) {
        best_output = decrypted;
        best_error = error;
      }
    }
  }

  std::cout << best_output << std::endl;
  return 0;
}
