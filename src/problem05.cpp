#include <iostream>
#include <string>

#include "bytevector.h"

int main() {
  std::string text =
    "Burning 'em, if you ain't quick and nimble\n"
                "I go crazy when I hear a cymbal";
  bytevector input = bytevector(text, bytevector::PLAIN);
  input.repeating_key_xor("ICE");
  std::cout << input.to_string(bytevector::HEX) << std::endl;
}
