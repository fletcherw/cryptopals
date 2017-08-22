#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "bytevector.h"

using std::string;
using std::unordered_map;
using std::array;
using std::vector;
using std::cout;
using std::endl;

array<byte, 16> key = random_key();

string profile_for(string email) {
  string sanitized_email;
  for (char c : email) {
    if (c == '&' || c == '=') continue;
    sanitized_email.push_back(c);
  }

  string profile;
  profile += "email=";
  profile += sanitized_email;
  profile += "&uid=";
  profile += "10";
  profile += "&role=user";
  return profile;
}

bytevector encrypt(string profile) {
  bytevector plaintext = string_to_bytevector(profile);
  return encrypt_ecb(plaintext, key.data(), true);
}

cookie decrypt(bytevector profile) {
  bytevector plaintext = decrypt_ecb(profile, key.data(), true);
  return parse_cookie(bytevector_to_string(plaintext), '&');
}

int main() {
  crypto_init();
  bytevector base = encrypt(profile_for("admin@site.us"));
  bytevector role =
    encrypt(profile_for("qqqqqqqqqqadmin\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb"));

  bytevector exploit;
  exploit.insert(exploit.end(), base.begin(), base.begin() + 32);
  exploit.insert(exploit.end(), role.begin() + 16, role.begin() + 32);

  cookie exploit_cookie = decrypt(exploit);

  for (const auto &pair : exploit_cookie) {
    cout << pair.first << " : " << pair.second << endl;
  }

  crypto_cleanup();
  return 0;
}
