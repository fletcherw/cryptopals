#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <iomanip>

#include "bytevector.h"

using std::string;
using std::array;
using std::vector;
using std::cout;
using std::endl;

bytevector ciphertext =
  base64_to_bytevector(
      "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==");

byte* key = (byte *)"YELLOW SUBMARINE";

int main() {
  crypto_init();

  bytevector bv = decrypt_ctr(ciphertext, key, 0);

  cout << bytevector_to_string(bv) << endl;
  crypto_cleanup();
  return 0;
}
