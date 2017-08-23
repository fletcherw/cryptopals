#include <iostream>

#include "bytevector.h"

using std::cout;
using std::endl;

int main() {
  bytevector text("YELLOW SUBMARINE", bytevector::PLAIN);
  text.pad_to_length(20);

  cout << "length is: " << text.size(); 
  cout << ", padding is: " ;
  cout << (text.check_padding() ? "valid" : "invalid");
  cout << endl;
  return 0;
}
