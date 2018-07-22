#include <iostream>
#include <string>

#include "bytevector.h"

int main() {
  std::string text =
    "Burning 'em, if you ain't quick and nimble\n"
                "I go crazy when I hear a cymbal";
  bytevector input = bytevector(text, bytevector::PLAIN);
  bytevector key("ICE", bytevector::PLAIN);
  input.repeating_key_xor(key);
  std::cout << "Result:   " << input.to_string(bytevector::HEX) << std::endl;
  std::cout << std::endl;
  std::cout << "Expected: "
               "0b 36 37 27 2a 2b 2e 63 62 2c 2e 69 69 2a 23 69 3a 2a 3c 63 24 "
               "20 2d 62 3d 63 34 3c 2a 26 22 63 24 27 27 65 27 2a 28 2b 2f 20 "
               "43 0a 65 2e 2c 65 2a 31 24 33 3a 65 3e 2b 20 27 63 0c 69 2b 20 "
               "28 31 65 28 63 26 30 2e 27 28 2f" << std::endl;
}
