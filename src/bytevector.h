#pragma once

#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

typedef unsigned char byte;

class bytevector {
public:
  typedef std::vector<byte>::iterator iterator;
  typedef std::vector<byte>::const_iterator const_iterator;

  enum bv_mode {
    HEX,
    BASE64,
    ASCII,
    PLAIN
  };

  bytevector();
  bytevector(size_t count, byte value);
  bytevector(const std::string &s, bv_mode mode);
  bytevector(std::ifstream& is, bv_mode mode=BASE64);

  template <class T>
  explicit bytevector(T input)
  {
    static_assert(std::is_integral<T>::value, 
                  "bytevector(T input) requires integral type");
    for (unsigned i = 0; i < sizeof(T); i++) {
      byte b = static_cast<byte>((input >> (8 * i)) & 0xFF);
      data_.push_back(b);
    }
  }

  template <class InputIt>
  bytevector(InputIt first, InputIt last) 
    : data_(first, last)
  {
  }

  std::string to_string(bv_mode mode) const;
  uint32_t to_uint32() const;

  void resize(size_t size, byte value = 0); 
  size_t size() const;
  void push_back(byte value);
  byte *data();

  template <class InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last)
  {
    return data_.insert(pos, first, last); 
  }

  template <class InputIt>
  void insert(iterator pos, InputIt first, InputIt last)
  {
    data_.insert(pos, first, last); 
  }

  iterator begin();
  iterator end();

  const_iterator cbegin() const;
  const_iterator cend() const;

  bytevector operator ^=(const bytevector &other);
  bytevector operator +=(const bytevector &other);
  bytevector operator +=(char c);

  byte& operator [](size_t idx) {
    return data_[idx];
  }

  byte operator [](size_t idx) const {
    return data_[idx];
  }

  friend bool operator ==(const bytevector &lhs, const bytevector &rhs)
  {
    return lhs.data_ == rhs.data_;
  }

  friend bytevector operator ^(bytevector lhs, const bytevector &rhs) {
    return lhs ^= rhs;
  }

  friend bytevector operator +(bytevector lhs, const bytevector &rhs) {
    return lhs += rhs;
  }

  friend bytevector operator +(bytevector lhs, char rhs) {
    return lhs += rhs;
  }

  void pad_to_length(size_t new_length);
  void pad_to_block(size_t blocksize);
  bool check_padding() const;
  void strip_padding();

  void repeating_key_xor(bytevector key);
  std::vector<bytevector> split_into_blocks(size_t blocksize) const;

private:
  std::vector<byte> data_;
};

std::ostream& operator<<(std::ostream& os, bytevector b);

// =====================

std::vector<bytevector> transpose(std::vector<bytevector> input);

std::array<double, 27> letter_frequencies(bytevector b);
bytevector solve_repeating_key_xor(bytevector ciphertext, size_t key_size);
int hamming_distance(bytevector a, bytevector b);
double squared_error(std::array<double, 27> freq);
