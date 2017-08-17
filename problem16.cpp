#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "bytevector.h"

using std::string;
using std::array;
using std::vector;
using std::cout;
using std::endl;

array<byte, 16> key = random_key();
array<byte, 16> iv = random_key();

bytevector encrypt(bytevector userdata) {
  bytevector prefix = 
    string_to_bytevector("comment1=cooking%20MCs;userdata=");
  bytevector suffix = 
    string_to_bytevector(";comment2=%20like%20a%20pound%20of%20bacon");

  bytevector sanitized;
  for (char c : userdata) {
    if (c == ';' || c == '=') {
      sanitized.push_back('"');
      sanitized.push_back(c);
      sanitized.push_back('"');
    } else {
      sanitized.push_back(c);
    }
  }
  bytevector ciphertext =
    encrypt_cbc(prefix + sanitized + suffix, key.data(), iv.data());
  return ciphertext;
}

int main() {
  crypto_init();

  string input = "QQQQQQQQQQQQQQQQ:admin<true"; 
  bytevector ciphertext = encrypt(string_to_bytevector(input));
  
  ciphertext[32] ^= 1;
  ciphertext[38] ^= 1;

  bytevector plaintext = 
    decrypt_cbc(ciphertext, key.data(), iv.data());
  cookie c = parse_cookie(bytevector_to_string(plaintext), ';');
  for (const auto &member : c) {
    if (member.first == "admin") {
      cout << "Admin: " << member.second << endl;
    }
  }

  crypto_cleanup();
  return 0;
}
