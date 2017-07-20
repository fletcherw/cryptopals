#include <iostream>
#include <string>

#include "bytevector.h"

int main() {
  std::string text =
    "Burning 'em, if you ain't quick and nimble\n"
                "I go crazy when I hear a cymbal";
  bytevector input = string_to_bytevector(text);
  bytevector output = repeating_key_xor(input, "ICE");
  std::cout << output << std::endl;
}
