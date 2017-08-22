#include <iostream>

#include "bytevector.h"

using std::cout;
using std::endl;

int main() {
  bytevector a = hex_to_bytevector("1c0111001f010100061a024b53535009181c");
  bytevector b = hex_to_bytevector("686974207468652062756c6c277320657965");
  bytevector c = a ^ b;
  cout << c << endl;
  return 0;
}
