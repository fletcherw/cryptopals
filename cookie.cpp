#include "cookie.h"

#include <vector>

using std::vector;
using std::string;

cookie parse_cookie(string cookie_str, char separator) {
  vector<string> kv_pairs;
  size_t last_idx = 0;
  size_t idx = cookie_str.find(separator);
  while (idx != string::npos) {
    kv_pairs.push_back(cookie_str.substr(last_idx, idx - last_idx));
    last_idx = idx + 1;
    idx = cookie_str.find(separator, last_idx);
  }
  kv_pairs.push_back(cookie_str.substr(last_idx, idx));

  cookie c;
  for (const string &s : kv_pairs) {
    idx = s.find('=');
    c[s.substr(0, idx)] = s.substr(idx+1);
  }
  return c;
}

