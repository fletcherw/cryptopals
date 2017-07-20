#include <iostream>
#include <string>
#include <vector>

#include "bytevector.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

byte *key = (byte *)"YELLOW SUBMARINE";

int main() {
  crypto_init();
  bytevector ciphertext = base64_to_bytevector("problem7.data"); 
  bytevector plaintext = decrypt_ecb(ciphertext, key, true);
  cout << bytevector_to_string(plaintext) << endl;
  crypto_cleanup();
  return 0;
}
