#include <iostream>
#include <string>
#include <vector>

#include "bytevector.h"
#include "Crypto.h"
#include "cookie.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

namespace {
  bytevector key = random_bytevector();
  Crypto cr;
}

string profile_for(string email)
{
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

bytevector encrypt(string profile)
{
  bytevector plaintext(profile, bytevector::PLAIN);
  return cr.encrypt_ecb(plaintext, key, true);
}

cookie decrypt(bytevector profile)
{
  bytevector plaintext = cr.decrypt_ecb(profile, key, true);
  return parse_cookie(plaintext.to_string(bytevector::PLAIN), '&');
}

int main()
{
  bytevector base = encrypt(profile_for("admin@site.us"));
  bytevector role =
    encrypt(profile_for("qqqqqqqqqqadmin\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb\xb"));

  bytevector exploit;
  exploit.insert(exploit.end(), base.begin(), base.begin() + 32);
  exploit.insert(exploit.end(), role.begin() + 16, role.begin() + 32);

  cookie exploit_cookie = decrypt(exploit);
  cout << exploit_cookie << endl;
  return 0;
}
