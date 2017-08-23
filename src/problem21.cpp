#include <iostream>
#include <random>
#include <iomanip>

#include "bytevector.h"
#include "MT19937.h"

int main() {
  std::mt19937 gen(1);
  MT19937 m(1);

  for (int i = 0; i < 10; i++) {
    std::cout << std::setw(10) << gen() << " " << m() << std::endl;
  }

  return 0;
}
