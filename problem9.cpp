#include <iostream>

#include "bytevector.h"

using std::cout;

int main() {
  bytevector text = string_to_bytevector("YELLOW SUBMARINE"); 
  bytevector padded = pad_bytevector(text, 20);  
  for (byte c : padded) {
    if (32 <= c && c <= 126) cout << c; 
    else cout << "\\x" << (int) c;
  }
  cout << std::endl;
  return 0;
}
