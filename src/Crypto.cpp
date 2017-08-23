#include "Crypto.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <random>
#include <stdexcept>
#include <vector>
#include <random>

#include "bytevector.h"
#include "MT19937.h"

using std::invalid_argument;
using std::vector;

namespace {
  std::random_device rd;
  std::mt19937 gen(rd());
}

Crypto::Crypto()
{
  if (instances_++ == 0) {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
  }
}

Crypto::~Crypto()
{
  if (--instances_ == 0) {
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
  }
}

void Crypto::handle_errors()
{
  ERR_print_errors_fp(stderr);
}

bytevector Crypto::encrypt_ecb(bytevector data, bytevector key, bool pad)
{
  if (key.size() != 16) {
    throw invalid_argument("encrypt_ecb: key must have length 16");
  }

  byte ciphertext[data.size() + 16];
  byte plaintext[data.size()];
  int i = 0;
  for (byte b : data) {
    plaintext[i++] = b;
  }

  int len;
  int ciphertext_len;
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

  if (ctx == nullptr) handle_errors();

  if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL) != 1)
    handle_errors();
  if (!pad) EVP_CIPHER_CTX_set_padding(ctx, 0);

  if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, data.size()) != 1)
    handle_errors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1)
    handle_errors();
  ciphertext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  bytevector output;
  for (int i = 0; i < ciphertext_len; i++) {
    output += ciphertext[i];
  }
  return output;
}

bytevector Crypto::decrypt_ecb(bytevector data, bytevector key, bool pad)
{
  if (key.size() != 16) {
    throw invalid_argument("decrypt_ecb: key must have length 16");
  }

  byte ciphertext[data.size()];
  byte plaintext[data.size()];
  int i = 0;
  for (byte b : data) {
    ciphertext[i++] = b;
  }

  int len;
  int plaintext_len;
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

  if (ctx == nullptr) {
    handle_errors();
    return bytevector();
  }

  if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key.data(), NULL) != 1)
    handle_errors();
  if (!pad) EVP_CIPHER_CTX_set_padding(ctx, 0);

  if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, data.size()) != 1)
    handle_errors();

  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1)
    handle_errors();
  plaintext_len += len;

  EVP_CIPHER_CTX_free(ctx);

  bytevector output;
  for (int i = 0; i < plaintext_len; i++) {
    output += plaintext[i];
  }

  return output;
}

bytevector Crypto::encrypt_cbc(
    bytevector plaintext, bytevector key, bytevector iv)
{
  if (!plaintext.check_padding()) {
    throw invalid_argument(
        "encrypt_cbc: input to encrypt_cbc must be padded to 16 bytes");
  }

  if (key.size() != 16) {
    throw invalid_argument("encrypt_cbc: key must have length 16");
  }

  vector<bytevector> blocks = plaintext.split_into_blocks(16);
  bytevector ciphertext;
  bytevector prev_block = iv;

  for (const bytevector &block : blocks) {
    bytevector cipher_block = encrypt_ecb(block ^ prev_block, key, false);
    ciphertext += cipher_block;
    prev_block = cipher_block;
  }
  return ciphertext;
}

bytevector Crypto::decrypt_cbc(
    bytevector ciphertext, bytevector key, bytevector iv)
{
  if (key.size() != 16) {
    throw invalid_argument("decrypt_cbc: key must have length 16");
  }

  vector<bytevector> blocks = ciphertext.split_into_blocks(16);
  bytevector plaintext;
  bytevector prev_block = iv;

  for (const bytevector &block : blocks) {
    bytevector plain_block = decrypt_ecb(block, key, false);
    plaintext += (plain_block ^ prev_block);
    prev_block = block;
  }
  return plaintext;
}

bytevector Crypto::do_ctr(bytevector input, bytevector key, uint64_t nonceInt)
{
  bytevector nonce(nonceInt);
  uint64_t block_counter = 0;
  vector<bytevector> blocks = input.split_into_blocks(16);

  bytevector output;
  for (const bytevector &block : blocks) {
    bytevector key_block = nonce + bytevector(block_counter);
    bytevector key_stream = encrypt_ecb(key_block, key, false);
    key_stream.resize(block.size());
    output += key_stream ^ block;
    block_counter++;
  }
  return output;
}

bytevector Crypto::encrypt_ctr(
    bytevector plaintext, bytevector key, uint64_t nonce)
{
  if (key.size() != 16) {
    throw invalid_argument("encrypt_ctr: key must have length 16");
  }

  return do_ctr(plaintext, key, nonce);
}

bytevector Crypto::decrypt_ctr(
    bytevector ciphertext, bytevector key, uint64_t nonce)
{
  if (key.size() != 16) {
    throw invalid_argument("decrypt_ctr: key must have length 16");
  }

  return do_ctr(ciphertext, key, nonce);
}

bytevector Crypto::do_mt(bytevector input, uint16_t key)
{
  MT19937 m(static_cast<uint32_t>(key));
  bytevector output;
  vector<bytevector> blocks = input.split_into_blocks(4);
  for (const bytevector &block : blocks) {
    bytevector ks = bytevector(m());
    ks.resize(block.size());
    output += block ^ ks; 
  }
  return output;
}

bytevector Crypto::encrypt_mt(bytevector plaintext, uint16_t key)
{
  return do_mt(plaintext, key);
}

bytevector Crypto::decrypt_mt(bytevector ciphertext, uint16_t key)
{
  return do_mt(ciphertext, key);
}

bytevector random_key(size_t length)
{
  std::uniform_int_distribution<> dis(0, 255);

  bytevector key;
  for (size_t i = 0; i < length; i++) {
    key += dis(gen);
  }
  return key;
}

bytevector random_string(size_t min, size_t max)
{
  if (min > max) {
    throw invalid_argument("random_string: min must be less than max");
  }

  std::uniform_int_distribution<> random_len(min, max);
  size_t length = random_len(gen);

  std::uniform_int_distribution<> dis(32, 126);
  bytevector s;
  for (size_t i = 0; i < length; i++) {
    s += dis(gen);
  }
  return s;
}

bytevector mt19937_token(size_t length, uint32_t seed)
{
  bytevector output;
  MT19937 m(seed);
  for (size_t i = 0; i < length; i+=4) {
    output += bytevector(m()); 
  }
  output.resize(length);
  return output;
}

