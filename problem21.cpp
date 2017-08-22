#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>

#include "bytevector.h"

using std::string;
using std::cout;
using std::endl;

int main() {
  std::mt19937 gen(1);
  MT19937 m(1);

  for (int i = 0; i < 10; i++) {
    cout << std::setw(10) << gen() << " " << m.next() << endl;
  }

  return 0;
}
