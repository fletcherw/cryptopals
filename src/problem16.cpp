#include <iostream>
#include <string>
#include <vector>

#include "bytevector.h"
#include "cookie.h"
#include "Crypto.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace {
bytevector key = random_key();
bytevector iv = random_key();
Crypto cr;
}

bytevector encrypt(bytevector userdata) {
  bytevector prefix("comment1=cooking%20MCs;userdata=", bytevector::PLAIN);
  bytevector suffix(
      ";comment2=%20like%20a%20pound%20of%20bacon", bytevector::PLAIN);

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
  bytevector plaintext = prefix + sanitized + suffix;
  plaintext.pad_to_block(16);
  bytevector ciphertext = cr.encrypt_cbc(plaintext, key, iv);
  return ciphertext;
}

int main() {

  bytevector input("QQQQQQQQQQQQQQQQ:admin<true", bytevector::PLAIN);
  bytevector ciphertext = encrypt(input);

  ciphertext[32] ^= 1;
  ciphertext[38] ^= 1;

  bytevector plaintext = cr.decrypt_cbc(ciphertext, key, iv);
  plaintext.strip_padding();
  cookie c = parse_cookie(plaintext.to_string(bytevector::PLAIN), ';');
  for (const auto &member : c) {
    if (member.first == "admin") {
      cout << "Admin: " << member.second << endl;
    }
  }

  return 0;
}
