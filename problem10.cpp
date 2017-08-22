#include <iostream>
#include <string>
#include <vector>

#include "bytevector.h"

using std::string;
using std::cout;
using std::endl;

byte *key = (byte *)"YELLOW SUBMARINE";
byte *iv = (byte *)"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

int main() {
  crypto_init();
  bytevector ciphertext = base64_to_bytevector("problem10.data");
  bytevector plaintext = decrypt_cbc(ciphertext, key, iv);
  cout << bytevector_to_string(plaintext) << endl;
  crypto_cleanup();
  return 0;
}
