#pragma once

#include "bytevector.h"

class Crypto {
public:
  Crypto();
  ~Crypto();

  void handle_errors();
  bytevector encrypt_ecb(bytevector plaintext, bytevector key, bool pad);
  bytevector decrypt_ecb(bytevector ciphertext, bytevector key, bool pad);

  bytevector encrypt_cbc(bytevector plaintext, bytevector key, bytevector iv);
  bytevector decrypt_cbc(bytevector ciphertext, bytevector key, bytevector iv);

  bytevector encrypt_ctr(bytevector plaintext, bytevector key, uint64_t nonce);
  bytevector decrypt_ctr(bytevector ciphertext, bytevector key, uint64_t nonce);

  bytevector encrypt_mt(bytevector plaintext, uint16_t key);
  bytevector decrypt_mt(bytevector ciphertext, uint16_t key);
private:
  bytevector do_ctr(bytevector input, bytevector key, uint64_t nonce);
  bytevector do_mt(bytevector input, uint16_t seed);

  unsigned instances_;
};

bytevector random_bytevector(size_t length=16);
bytevector random_string(size_t min, size_t max);
bytevector mt19937_token(size_t length, uint32_t seed);
