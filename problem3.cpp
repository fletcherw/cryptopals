#include <iostream>
#include <array>
#include <string>

#include "bytevector.h"

using std::array;
using std::string;

int main() {
  bytevector input = hex_to_bytevector(
     "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736");

  bytevector best_output;
  double best_error = -1;
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
  std::cout << bytevector_to_string(best_output) << std::endl;
  return 0;
}
