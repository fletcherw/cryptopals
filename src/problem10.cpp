#include <fstream>
#include <iostream>

#include "bytevector.h"
#include "Crypto.h"

bytevector key = bytevector("YELLOW SUBMARINE", bytevector::PLAIN);
bytevector iv = bytevector(16, '\0');

int main() {
  Crypto cr;
  std::ifstream in_file("data/problem10.data");
  bytevector ciphertext(in_file);
  bytevector plaintext = cr.decrypt_cbc(ciphertext, key, iv);
  std::cout << "Plaintext: " << std::endl
            << plaintext.to_string(bytevector::ASCII) << std::endl;
  return 0;
}
