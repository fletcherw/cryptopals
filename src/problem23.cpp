#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <iomanip>

#include "bytevector.h"
#include "MT19937.h"

using std::array;
using std::cout;
using std::endl;

int main() {
  MT19937 m(0xDEADC0DE);  
  std::array<uint32_t, 624> state;
  for (int i = 0; i < 624; i++) {
    state[i] = untemper(m());
  }

  MT19937 copied(state);

  for (int i = 0; i < 10; i++) {
    cout << std::setw(10) << m() << " " << copied() << endl;
  }
}
