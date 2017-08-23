#include <iostream>

#include "bytevector.h"
#include "Crypto.h"

bytevector ciphertext(
      "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==",
      bytevector::BASE64);

bytevector key("YELLOW SUBMARINE", bytevector::PLAIN);
Crypto cr;

int main() {
  bytevector plaintext = cr.decrypt_ctr(ciphertext, key, 0);

  std::cout << plaintext << std::endl;
  return 0;
}
