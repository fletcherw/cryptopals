#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>
#include <unordered_map>

#include "bytevector.h"

using std::string;
using std::unordered_map;
using std::array;
using std::vector;
using std::cout;
using std::endl;

typedef unordered_map<string, string> cookie;

array<byte, 16> key = random_key();

cookie parse_cookie(string cookie_str) {
  vector<string> kv_pairs;
  size_t last_idx = 0;
  size_t idx = cookie_str.find('&');
  while (idx != string::npos) {
    kv_pairs.push_back(cookie_str.substr(last_idx, idx - last_idx));
    last_idx = idx + 1;
    idx = cookie_str.find('&', last_idx);
  }
  kv_pairs.push_back(cookie_str.substr(last_idx, idx));

  cookie c;
  for (const string &s : kv_pairs) {
    idx = s.find('=');
    c[s.substr(0, idx)] = s.substr(idx+1);
  }
  return c;
}

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
  return parse_cookie(bytevector_to_string(plaintext));
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
