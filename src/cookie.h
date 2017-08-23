#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> cookie;

cookie parse_cookie(std::string cookie_str, char separator);
