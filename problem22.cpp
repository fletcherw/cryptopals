#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

#include "bytevector.h"

using std::string;
using std::cout;
using std::endl;

int main() {
  std::random_device rd;
  std::mt19937 durationMT(rd());
  int64_t time = std::chrono::seconds(std::time(NULL)).count();
  cout << "Current epoch time: " << time << endl;
  std::this_thread::sleep_for(std::chrono::seconds(10 + durationMT() % 990));

  time = std::chrono::seconds(std::time(NULL)).count();
  MT19937 mt(static_cast<uint32_t>(time & 0xFFFFFFFFl));
  cout << "Seeded MT19937 with: " << time << endl;

  std::this_thread::sleep_for(std::chrono::seconds(10 + durationMT() % 990));
  uint32_t value = mt.next();
  cout << "First value: " << value << endl;

  time = std::chrono::seconds(std::time(NULL)).count();
  for (int64_t i = time; i > 0; i--) {
    MT19937 m(static_cast<uint32_t>(i & 0xFFFFFFFFl));
    if (m.next() == value) {
      cout << "Guess at MT seed is: " << i << endl;
      break;
    }
  }
  return 0;
}
