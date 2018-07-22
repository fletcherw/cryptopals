#include <iostream>
#include <fstream>

#include "bytevector.h"
#include "Crypto.h"

int main() {
  bytevector key("YELLOW SUBMARINE", bytevector::PLAIN);
  Crypto cr;
  std::ifstream in_file("data/problem07.data");
  bytevector ciphertext(in_file);
  bytevector plaintext = cr.decrypt_ecb(ciphertext, key, true);
  std::cout << "Plaintext: " << std::endl
            << plaintext.to_string(bytevector::ASCII) << std::endl;
  return 0;
}
