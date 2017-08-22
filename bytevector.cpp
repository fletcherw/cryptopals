#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>
#include <random>
#include <stdexcept>
#include <cstdio>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <strings.h>

#include "bytevector.h"

using namespace std;

namespace {
  std::random_device rd;
  std::mt19937 gen(rd());
}

char base64_char_decode(char c) {
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
    throw std::invalid_argument("character is not base64");
  }
}

bytevector base64_file_to_bytevector(string file) {
  std::ifstream input(file);
  bytevector output;
  char a, b, c, d;
  while (input >> a >> b >> c >> d) {
    char i1 = base64_char_decode(a);
    char i2 = base64_char_decode(b);
    char i3 = base64_char_decode(c);
    char i4 = base64_char_decode(d);

    char b1 = (i1 << 2) | ((i2 >> 4) & 0x03);
    char b2 = (i2 << 4) | ((i3 >> 2) & 0x0F);
    char b3 = (i3 << 6) | (i4);

    output.push_back(b1);
    output.push_back(b2);
    output.push_back(b3);
  }
  return output;
}

bytevector base64_to_bytevector(string input) {
  if (input.size() % 4 != 0) {
    throw invalid_argument(
        "input to base64_to_bytevector must have length that is multiple of 4");
  }
  bytevector output;
  for (unsigned i = 0; i < input.size(); i += 4) {
    char i1 = base64_char_decode(input[i]);
    char i2 = base64_char_decode(input[i+1]);
    char i3 = base64_char_decode(input[i+2]);
    char i4 = base64_char_decode(input[i+3]);

    char b1 = (i1 << 2) | ((i2 >> 4) & 0x03);
    char b2 = (i2 << 4) | ((i3 >> 2) & 0x0F);
    char b3 = (i3 << 6) | (i4);

    output.push_back(b1);
    output.push_back(b2);
    output.push_back(b3);
  }
  return output;
}

bytevector int_to_bytevector(uint64_t input)
{
  bytevector bv;
  for (unsigned i = 0; i < 8; i++) {
    byte b = static_cast<byte>((input >> (8 * i)) & 0xFF);
    bv.push_back(b);
  }
  return bv;
}

byte hex_digit_to_bits(byte c) {
  if ('0' <= c && c <= '9') {
    return (c - '0');
  } else if ('a' <= c && c <= 'f') {
    return c - 'a' + 10;
  } else if ('A' <= c && c <= 'F') {
    return c - 'A' + 10;
  } else {
    throw invalid_argument("byte passed was not valid hex");
  }
}

bytevector hex_to_bytevector(string s) {
  bytevector bytes;
  auto it = s.begin();
  while (it != s.end()) {
    byte d1 = *it;
    ++it;
    byte d2 = (it == s.end()) ? '\0' : *it;
    ++it;

    byte b1 = hex_digit_to_bits(d1);
    byte b2 = hex_digit_to_bits(d2);
    bytes.push_back(b1 << 4 | b2);
  }
  return bytes;
}

bytevector string_to_bytevector(string s) {
  bytevector bytes;
  for (byte c : s) { bytes.push_back(c); }
  return bytes;
}

string bytevector_to_string(bytevector bytes) {
  ostringstream out;
  for (byte c : bytes) {
    out << c;
  }
  return out.str();
}

static const char charset[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string bytevector_to_base64(bytevector bytes) {
  string output;
  auto it = bytes.begin();
  while (it != bytes.end()) {
    char b1 = *(it++);
    char b2 = (it == bytes.end()) ? '\0' : *(it++);
    char b3 = (it == bytes.end()) ? '\0' : *(it++);

    int i1 = (b1 & 0xFC) >> 2;
    int i2 = (b1 & 0x03) << 4 | ((b2 & 0xF0) >> 4);
    int i3 = (b2 & 0x0F) << 2 | ((b3 & 0xC0) >> 6);
    int i4 =  b3 & 0x3F;

    output += charset[i1];
    output += charset[i2];
    output += charset[i3];
    output += charset[i4];
  }
  return output;
}

ostream& operator<<(ostream& os, bytevector b) {
  char equiv[3];
  equiv[2] = '\0';
  auto it = b.begin();
  while (it != b.end()) {
    sprintf(equiv, "%02x", *it);
    os << equiv;
    if (it + 1 != b.end()) os << " ";
    it++;
  }
  return os;
}

bytevector operator^(bytevector a, bytevector b) {
  if (a.size() != b.size()) {
    throw invalid_argument("XORed buffers must have equal length");
  }

  auto it_a = a.begin();
  auto it_b = b.begin();
  bytevector output;
  while (it_a != a.end() && it_b != b.end()) {
    output.push_back(*it_a ^ *it_b);
    it_a++;
    it_b++;
  }
  if (!(it_a == a.end() && it_b == b.end())) {
    throw logic_error("XOR should fully read both buffers");
  }
  return output;
}

bytevector operator+=(bytevector &a, bytevector b) {
  a.insert(a.end(), b.begin(), b.end());
  return a;
}

bytevector operator+(bytevector a, bytevector b) {
  return a += b;
}

bytevector operator+=(bytevector &a, char b) {
  a.push_back(b);
  return a;
}

bytevector operator+(bytevector a, char b) {
  return a += b;
}

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

bytevector repeating_key_xor(bytevector text, string key) {
  bytevector mask;
  unsigned count = 0;
  while (count < text.size()) {
    for (byte c : key) {
      if (count == text.size()) break;
      mask.push_back(c);
      count++;
    }
  }
  return text ^ mask;
}

string solve_repeating_key_xor(bytevector ciphertext, unsigned key_size) {
  vector<bytevector> blocks = split_into_blocks(ciphertext, key_size);
  if (blocks.back().size() < key_size) {
    blocks.resize(blocks.size() - 1);
  }

  vector<bytevector> by_index = transpose(blocks);
  std::string key;
  for (bytevector bv : by_index) {
    char best_key = 'a';
    double best_error = -1;
    for (unsigned c = 0; c < 255; c++) {
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

vector<bytevector> split_into_blocks(bytevector input, unsigned blocksize) {
  vector<bytevector> output;

  for (unsigned index = 0; index < input.size(); index += blocksize) {
    auto it = input.begin() + index;
    bytevector bv;
    unsigned count = 0;
    while (count < blocksize && it != input.end()) {
      bv.push_back(*it);
      count++;
      it++;
    }
    output.push_back(bv);
  }
  return output;
}

vector<bytevector> transpose(vector<bytevector> input) {
  vector<bytevector> output(input[0].size());
  for (bytevector bv : input) {
    for (unsigned i = 1; i < bv.size(); i++) {
      output[i].push_back(bv[i]);
    }
  }
  return output;
}

bytevector pad_bytevector(bytevector bv, unsigned int length) {
  if (length < bv.size()) {
    throw std::invalid_argument(
        "PKCS7 padding length must be longer than bytevector length");
  }
  if ((length - bv.size()) > 256) {
    throw std::invalid_argument("PKCS7 cannot pad more than 256 bytes");
  }

  bytevector output = bv;
  output.resize(length, length - bv.size());
  return output;
}

bytevector pad_to_block(bytevector bv, unsigned int blocksize) {
  int size = bv.size();
  return pad_bytevector(bv, size + (blocksize - (size % blocksize)));
}

bool check_padding(bytevector bv) {
  unsigned len = bv.size();
  for (unsigned i = 1; i <= 256 && i <= len; i++) {
    if (i > 1 && bv[len - i] != bv[len - i + 1]) {
      return false;
    }
    if (bv[len - i] == i) {
      return true;
    }
  }
  return false;
}

bytevector strip_padding(bytevector bv) {
  unsigned len = bv.size();
  for (unsigned i = 1; i <= 256 && i <= len; i++) {
    if (i > 1 && bv[len - i] != bv[len - i + 1]) {
      throw std::invalid_argument("Bad padding");
    }
    if (bv[len - i] == i) {
      bytevector res = bv;
      res.resize(len - i);
      return res;
    }
  }
  throw std::invalid_argument("Bad padding");
}

void crypto_init(void) {
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);
}

void crypto_cleanup(void) {
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
  ERR_free_strings();
}

void handleErrors(void) {
  ERR_print_errors_fp(stderr);
}

array<byte, 16> random_key(void) {
  std::uniform_int_distribution<> dis(32, 126);

  array<byte, 16> key;
  for (int i = 0; i < 16; i++) {
    key[i] = dis(gen);
  }
  return key;
}

bytevector random_string(void) {
  std::uniform_int_distribution<> random_len(0, 256);
  int len = random_len(gen);

  std::uniform_int_distribution<> dis(32, 126);
  bytevector s;
  for (int i = 0; i < len; i++) {
    s.push_back(dis(gen));
  }
  return s;
}

bytevector encrypt_ecb(bytevector data, byte *key, bool pad) {
  byte ciphertext[data.size() + 16];
  byte plaintext[data.size()];
  int i = 0;
  for (byte b : data) {
    plaintext[i++] = b;
  }

  int len;
  int ciphertext_len;
  EVP_CIPHER_CTX *ctx;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
    handleErrors();
  if (!pad) EVP_CIPHER_CTX_set_padding(ctx, 0);

  /* Provide the message to be encrypted, and obtain the ciphertext output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, data.size()))
    handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  bytevector output;
  for (int i = 0; i < ciphertext_len; i++) {
    output.push_back(ciphertext[i]);
  }
  return output;
}

bytevector decrypt_ecb(bytevector data, byte *key, bool pad) {
  byte ciphertext[data.size()];
  byte plaintext[data.size()];
  int i = 0;
  for (byte b : data) {
    ciphertext[i++] = b;
  }

  int len;
  int plaintext_len;
  EVP_CIPHER_CTX *ctx;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL))
    handleErrors();
  if (!pad) EVP_CIPHER_CTX_set_padding(ctx, 0);

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, data.size()))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  bytevector output;
  for (int i = 0; i < plaintext_len; i++) {
    output.push_back(plaintext[i]);
  }

  return output;
}

bytevector encrypt_cbc(bytevector plaintext, byte *key, byte *iv) {
  if (!check_padding(plaintext)) {
    throw std::invalid_argument("input to encrypt_cbc must be padded to 16 bytes");
  }
  vector<bytevector> blocks = split_into_blocks(plaintext, 16);
  bytevector ciphertext;
  bytevector prev_block(iv, iv + 16);

  for (const bytevector &block : blocks) {
    bytevector cipher_block = encrypt_ecb(block ^ prev_block, key, false);
    ciphertext += cipher_block;
    prev_block = cipher_block;
  }
  return ciphertext;
}

bytevector decrypt_cbc(bytevector ciphertext, byte *key, byte *iv) {
  vector<bytevector> blocks = split_into_blocks(ciphertext, 16);
  bytevector plaintext;
  bytevector prev_block(iv, iv + 16);

  for (const bytevector &block : blocks) {
    bytevector plain_block = decrypt_ecb(block, key, false);
    plaintext += (plain_block ^ prev_block);
    prev_block = block;
  }
  return plaintext;
}

bytevector do_ctr(bytevector input, byte *key, uint64_t nonceInt) {
  bytevector nonce = int_to_bytevector(nonceInt);
  uint64_t block_counter = 0;
  vector<bytevector> blocks = split_into_blocks(input, 16);

  bytevector output;
  for (const bytevector &block : blocks) {
    bytevector key_block = nonce + int_to_bytevector(block_counter);
    bytevector key_stream =
      encrypt_ecb(key_block, key, false);
    key_stream.resize(block.size());
    output += key_stream ^ block;
    block_counter++;
  }
  return output;
}

bytevector encrypt_ctr(bytevector plaintext, byte *key, uint64_t nonce)
{
  return do_ctr(plaintext, key, nonce);
}

bytevector decrypt_ctr(bytevector ciphertext, byte *key, uint64_t nonce)
{
  return do_ctr(ciphertext, key, nonce);
}

cookie parse_cookie(string cookie_str, char separator) {
  vector<string> kv_pairs;
  size_t last_idx = 0;
  size_t idx = cookie_str.find(separator);
  while (idx != string::npos) {
    kv_pairs.push_back(cookie_str.substr(last_idx, idx - last_idx));
    last_idx = idx + 1;
    idx = cookie_str.find(separator, last_idx);
  }
  kv_pairs.push_back(cookie_str.substr(last_idx, idx));

  cookie c;
  for (const string &s : kv_pairs) {
    idx = s.find('=');
    c[s.substr(0, idx)] = s.substr(idx+1);
  }
  return c;
}

