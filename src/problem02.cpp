#include <iostream>

#include "bytevector.h"

int main() {
  bytevector a("1c0111001f010100061a024b53535009181c", bytevector::HEX);
  bytevector b("686974207468652062756c6c277320657965", bytevector::HEX);
  bytevector c = a ^ b;
  std::cout << c << std::endl;
  return 0;
}
