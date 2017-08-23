#include <iostream>
#include <string>

#include "bytevector.h"

int main() {
  std::string raw = "49276d206b696c6c696e6720796f757220627261696e206c"
               "696b65206120706f69736f6e6f7573206d757368726f6f6d";

  bytevector input(raw, bytevector::HEX);
  std::cout << input.to_string(bytevector::BASE64) << std::endl;
  return 0;
}
