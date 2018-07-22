#include <iostream>
#include <stdexcept>

#include "bytevector.h"

using std::cout;
using std::endl;

int main() {
  bytevector a("ICE ICE BABY\x04\x04\x04\x04", bytevector::PLAIN);
  cout << "Original: " << a << endl;
  a.strip_padding();
  cout << "Stripped: " << a << endl;
  cout << endl;

  bytevector b("ICE ICE BABY\x05\x05\x05\x05", bytevector::PLAIN);
  cout << "Original: " << b << endl;
  try {
    b.strip_padding();
    cout << "Stripped: " << b << endl;
  } catch (std::invalid_argument &) {
    cout << "Padding is invalid" << endl;
  }
}
