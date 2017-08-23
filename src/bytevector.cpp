#include "bytevector.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <random>
#include <stdexcept>
#include <cstdio>

#include <strings.h>

#include "MT19937.h"

using std::invalid_argument;
using std::string;
using std::vector;

using std::array;

namespace {
  const char base64_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char decode_base64_char(char c)
{
  if ('A' <= c && c <= 'Z') {
    return (c - 'A');
  } else if ('a' <= c && c <= 'z') {
    return (c - 'a' + 26);
  } else if ('0' <= c && c <= '9') {
    return (c - '0' + 52);
  } else if (c == '+') {
    return 62;
  } else if (c == '/') {
    return 63;
  } else if (c == '=') {
    return 0;
  } else {
    throw invalid_argument("decode_base64_char: character is not base64");
  }
}

byte hex_digit_to_bits(byte c)
{
  if ('0' <= c && c <= '9') {
    return (c - '0');
  } else if ('a' <= c && c <= 'f') {
    return c - 'a' + 10;
  } else if ('A' <= c && c <= 'F') {
    return c - 'A' + 10;
  } else {
    throw invalid_argument("hex_digit_to_bits: byte passed was not valid hex");
  }
}

void decode_base64_to_vector(char a, char b, char c, char d, vector<byte> &v)
{
  char i1 = decode_base64_char(a);
  char i2 = decode_base64_char(b);
  char i3 = decode_base64_char(c);
  char i4 = decode_base64_char(d);

  char b1 = (i1 << 2) | ((i2 >> 4) & 0x03);
  char b2 = (i2 << 4) | ((i3 >> 2) & 0x0F);
  char b3 = (i3 << 6) | (i4);

  v.push_back(b1);
  v.push_back(b2);
  v.push_back(b3);
}

} // namespace

bytevector::bytevector()
{
}

bytevector::bytevector(size_t count, byte value)
{
  resize(count, value);
}

bytevector::bytevector(const string &s, bv_mode mode)
{
  if (mode == bytevector::HEX) {
    string::const_iterator it = s.begin();
    while (it != s.end()) {
      byte d1 = *it;
      ++it;
      byte d2 = (it == s.end()) ? '\0' : *it;
      ++it;

      byte b1 = hex_digit_to_bits(d1);
      byte b2 = hex_digit_to_bits(d2);
      data_.push_back(b1 << 4 | b2);
    }
  } 
  else if (mode == bytevector::BASE64) {
    if (s.size() % 4 != 0) {
      throw invalid_argument(
          "bytevector: base64 input must have length that is multiple of 4");
    }

    for (size_t i = 0; i < s.size(); i += 4) {
      decode_base64_to_vector(s[i], s[i+1], s[i+2], s[i+3], data_);
    }
  }
  else if (mode == bytevector::PLAIN) {
    for (byte c : s) data_.push_back(c);
  }
  else {
    throw invalid_argument("bytevector: invalid mode");
  }
}

bytevector::bytevector(std::ifstream& is, bv_mode mode)
{
  if (mode != bytevector::BASE64) {
    throw invalid_argument(
        "bytevector: istream constructor only supports base64 mode");
  }

  char a, b, c, d;
  while (is >> a >> b >> c >> d) {
    decode_base64_to_vector(a, b, c, d, data_);
  }
}

bytevector bytevector::operator ^=(const bytevector &other)
{
  if (size() != other.size()) {
    throw invalid_argument("XORed bytevectors must have equal length");
  }

  for (size_t i = 0; i < size(); i++) {
    data_[i] ^= other.data_[i];
  }
  return *this;
}

bytevector bytevector::operator +=(const bytevector &other)
{
  data_.insert(data_.end(), other.data_.begin(), other.data_.end());
  return *this;
}

bytevector bytevector::operator +=(char c)
{
  data_.push_back(c);
  return *this;
}

std::ostream& operator<<(std::ostream& os, bytevector b) {
  os << b.to_string(bytevector::ASCII); 
  return os;
}


string bytevector::to_string(bv_mode mode) const
{
  string out;

  if (mode == bytevector::HEX) {
    char equiv[3];
    equiv[2] = '\0';

    for (auto it = data_.begin(); it != data_.end(); it++) {
      snprintf(equiv, 3, "%02x", *it);
      out += equiv;
      if (it + 1 != data_.end()) out += " ";
    }
  } 
  else if (mode == bytevector::BASE64) {
    for (auto it = data_.begin(); it != data_.end(); it++) {
      size_t padding = 0;

      char b1 = *(it++);
      char b2 = (it == data_.end()) ? (padding++, '\0') : *(it++);
      char b3 = (it == data_.end()) ? (padding++, '\0') : *(it++);

      int i1 = (b1 & 0xFC) >> 2;
      int i2 = (b1 & 0x03) << 4 | ((b2 & 0xF0) >> 4);
      int i3 = (b2 & 0x0F) << 2 | ((b3 & 0xC0) >> 6);
      int i4 =  b3 & 0x3F;

      out += base64_charset[i1];
      out += base64_charset[i2];
      out += padding > 1 ? '=' : base64_charset[i3];
      out += padding > 0 ? '=' : base64_charset[i4];
    }
  }
  else if (mode == bytevector::ASCII) {
    for (byte c : data_) {
      if ((32 <= c && c <= 126) ||
           c == '\n' || c == '\t' || 
           c == '\r' || c == '\v' ||
           c == '\f')
      {
        out += c; 
      }
      else {
        out += '_';
      }
    }
  }
  else if (mode == bytevector::PLAIN) {
    for (byte c : data_) {
      out += c;
    }
  }

  return out;
}

uint32_t bytevector::to_uint32() const
{
  if (size() != 4) {
    throw invalid_argument(
        "to_uint32: length of bytevector must be 4");
  }

  uint32_t output = 0;
  for (size_t i = 0; i < 4; i++) {
    output |= (data_[i] << (8 * i));
  }
  return output;
}

void bytevector::resize(size_t size, byte value)
{
  data_.resize(size, value);
}

size_t bytevector::size() const
{
  return data_.size();
}

void bytevector::push_back(byte value)
{
  data_.push_back(value);
}

byte *bytevector::data()
{
  return data_.data();
}

bytevector::iterator bytevector::begin()
{
  return data_.begin();
}

bytevector::iterator bytevector::end()
{
  return data_.end();
}

bytevector::const_iterator bytevector::cbegin() const
{
  return data_.cbegin();
}

bytevector::const_iterator bytevector::cend() const
{
  return data_.cend();
}

void bytevector::pad_to_length(size_t new_size)
{
  if (new_size < size()) {
    throw invalid_argument(
        "pad_to_length: padding length must be longer than bytevector length");
  }

  if ((new_size - size()) > 256) {
    throw invalid_argument(
        "pad_to_length: PKCS7 cannot pad more than 256 bytes");
  }

  data_.resize(new_size, new_size - size());
}

void bytevector::pad_to_block(size_t blocksize)
{
  pad_to_length(size() + (blocksize - (size() % blocksize)));
}

bool bytevector::check_padding() const
{
  for (size_t i = 1; i <= 256 && i <= size(); i++) {
    if (i > 1 && data_[size() - i] != data_[size() - i + 1]) {
      return false;
    }
    if (data_[size() - i] == i) {
      return true;
    }
  }
  return false;
}

void bytevector::strip_padding()
{
  for (size_t i = 1; i <= 256 && i <= size(); i++) {
    if (i > 1 && data_[size() - i] != data_[size() - i + 1]) {
      throw invalid_argument("strip_padding: bad padding");
    }

    if (data_[size() - i] == i) {
      data_.resize(size() - i);
      return;
    }
  }
  throw invalid_argument("strip_padding: bad padding");
}

void bytevector::repeating_key_xor(string key)
{
  for (size_t i = 0; i < size(); i++) {
    data_[i] ^= key[i % key.size()];
  }
}

vector<bytevector> bytevector::split_into_blocks(size_t blocksize) const
{
  vector<bytevector> output;

  for (size_t index = 0; index < size(); index += blocksize) {
    auto it = cbegin() + index;
    bytevector bv;
    size_t count = 0;
    while (count < blocksize && it != cend()) {
      bv.push_back(*it);
      count++;
      it++;
    }
    output.push_back(bv);
  }
  return output;
}

// ===================================

array<double, 27> letter_frequencies(bytevector b) {
  double total = 0;
  array<double, 27> counts;
  for (int i = 0; i < 27; i++) counts[i] = 0;
  for (byte c : b) {
    total++;
    if ('A' <= c && c <= 'Z') {
      counts[c - 'A']++;
    } else if ('a' <= c && c <= 'z') {
      counts[c - 'a']++;
    } else {
      counts[26]++;
    }
  }

  for (auto it = counts.begin(); it != counts.end(); it++) {
    (*it) /= total;
  }
  return counts;
}

string solve_repeating_key_xor(bytevector ciphertext, size_t key_size) {
  vector<bytevector> blocks = ciphertext.split_into_blocks(key_size);
  if (blocks.back().size() < key_size) {
    blocks.resize(blocks.size() - 1);
  }

  vector<bytevector> by_index = transpose(blocks);
  std::string key;
  for (bytevector bv : by_index) {
    char best_key = 'a';
    double best_error = -1;
    for (size_t c = 0; c < 255; c++) {
      bytevector mask(bv.size(), c);
      bytevector decrypted = bv ^ mask;
      auto frequencies = letter_frequencies(decrypted);
      double error = squared_error(frequencies);
      if (error < best_error || best_error == -1) {
        best_key = c;
        best_error = error;
      }
    }
    key += best_key;
  }
  return key;
}

int hamming_distance(bytevector a, bytevector b) {
  bytevector diff = a ^ b;
  int count = 0;
  for (byte c : diff) {
    for (int i = 0; i < 8; i++) {
      if (c & (1 << i)) { count++; }
    }
  }
  return count;
}

const array<double, 27> standard = {0.08167, 0.01492, 0.02782, 0.04253, 0.12702,
                                    0.02228, 0.02015, 0.06094, 0.06966, 0.00153,
                                    0.00772, 0.04025, 0.02406, 0.06749, 0.07507,
                                    0.01929, 0.00095, 0.05987, 0.06327, 0.09056,
                                    0.02758, 0.00978, 0.02360, 0.00150, 0.01974,
                                    0.00074, 0.0 };

double squared_error(array<double, 27> freq) {
  double error = 0;
  for (size_t i = 0; i < 27; i++) {
    error += (freq[i] - standard[i]) * (freq[i] - standard[i]);
  }
  return error;
}

vector<bytevector> transpose(vector<bytevector> input) {
  vector<bytevector> output(input[0].size());
  for (bytevector bv : input) {
    for (size_t i = 1; i < bv.size(); i++) {
      output[i].push_back(bv[i]);
    }
  }
  return output;
}
