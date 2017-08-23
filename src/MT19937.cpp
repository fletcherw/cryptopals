#include "MT19937.h"

uint32_t untemper(uint32_t val)
{
  uint32_t c = 0;
  c |= val & 0xffffc000;
  c |= (val ^ (c >> 18)) & 0x3fff;

  uint32_t b = 0;
  b |= c & 0x00007fff;
  b |= (c ^ ((b << 15) & 0xefc60000)) & 0x3fff8000;
  b |= (c ^ ((b << 15) & 0xefc60000)) & 0xc0000000;

  uint32_t a = 0;
  a |= b & 0x7f;
  a |= (b ^ ((a << 7) & 0x9d2c5680)) & 0x3f80;
  a |= (b ^ ((a << 7) & 0x9d2c5680)) & 0x1fc000;
  a |= (b ^ ((a << 7) & 0x9d2c5680)) & 0xfe00000;
  a |= (b ^ ((a << 7) & 0x9d2c5680)) & 0xf0000000;

  uint32_t y = 0;
  y |= a & 0xffe00000;  
  y |= (a ^ (y >> 11)) & 0x1ffc00;
  y |= (a ^ (y >> 11)) & 0x3ff;

  return y;
}

MT19937::MT19937(uint32_t seed)
{
  state_[0] = seed;
  index_ = 624;
  for (unsigned i = 1; i < 624; i++) {
    state_[i] = 1812433253 * (state_[i - 1] ^ (state_[i - 1] >> 30)) + i;
  }
}

MT19937::MT19937(std::array<uint32_t, 624> state)
{
  index_ = 624;
  for (unsigned i = 0; i < 624; i++) {
    state_[i] = state[i];
  }
}

uint32_t MT19937::operator ()()
{
  if (index_ >= 624) {
    for (unsigned i = 0; i < 624; i++) {
      uint32_t y = (state_[i] & 0x80000000) + 
                   (state_[(i + 1) % 624] & 0x7fffffff);
      state_[i] = state_[(i + 397) % 624] ^ (y >> 1);
      if (y % 2 == 1) {
        state_[i] ^= 0x9908b0df; 
      }
    }
    index_ = 0;
  }

  uint32_t y = state_[index_];
  y ^= y >> 11;
  y ^= (y << 7) & 0x9d2c5680;
  y ^= (y << 15) & 0xefc60000;
  y ^= y >> 18;

  index_++;
  return y;
}
