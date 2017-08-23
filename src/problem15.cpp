#include <iostream>
#include <vector>
#include <stdexcept>

#include "bytevector.h"

using std::vector;
using std::cout;
using std::endl;

int main() {
  bytevector a = string_to_bytevector("ICE ICE BABY\x04\x04\x04\x04");
  cout << a << endl;
  cout << strip_padding(a) << endl;

  bytevector b = string_to_bytevector("ICE ICE BABY\x05\x05\x05\x05");
  cout << b << endl;
  try {
    cout << strip_padding(b) << endl;
  } catch (std::invalid_argument &) {
    cout << "Padding is invalid" << endl;
  }
}
