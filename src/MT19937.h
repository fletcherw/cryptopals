#pragma once

#include <array>

uint32_t untemper(uint32_t val);

class MT19937 {
public:
  MT19937(uint32_t seed);
  MT19937(std::array<uint32_t, 624> state);
  uint32_t operator ()(); 

private:
  unsigned index_;
  std::array<uint32_t, 624> state_;
};
