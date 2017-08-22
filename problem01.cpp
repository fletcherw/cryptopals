#include <iostream>
#include <vector>
#include <string>

#include "bytevector.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

int main() {
  string raw = "49276d206b696c6c696e6720796f757220627261696e206c"
               "696b65206120706f69736f6e6f7573206d757368726f6f6d";

  bytevector input = hex_to_bytevector(raw);
  cout << bytevector_to_base64(input) << endl;
  return 0;
}
