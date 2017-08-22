#include <array>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

typedef unsigned char byte;
typedef std::vector<byte> bytevector;
typedef std::unordered_map<std::string, std::string> cookie;

std::ostream& operator<<(std::ostream& os, bytevector b);

bytevector operator^(bytevector a, bytevector b);
bytevector operator+=(bytevector &a, bytevector b);
bytevector operator+(bytevector a, bytevector b);
bytevector operator+=(bytevector &a, char b);
bytevector operator+(bytevector a, char b);

bytevector hex_to_bytevector(std::string s);
bytevector string_to_bytevector(std::string s);
bytevector base64_file_to_bytevector(std::string file);
bytevector base64_to_bytevector(std::string input);
bytevector int_to_bytevector(uint64_t i);

std::string bytevector_to_string(bytevector bytes);
std::string bytevector_to_base64(bytevector bytes);

std::array<double, 27> letter_frequencies(bytevector b);
bytevector repeating_key_xor(bytevector text, std::string key);
std::string solve_repeating_key_xor(bytevector ciphertext, unsigned key_size);
int hamming_distance(bytevector a, bytevector b);
double squared_error(std::array<double, 27> freq);
std::vector<bytevector> split_into_blocks(bytevector input, unsigned blocksize);
std::vector<bytevector> transpose(std::vector<bytevector> input);

bytevector pad_bytevector(bytevector bv, unsigned int length);
bytevector pad_to_block(bytevector bv, unsigned int blocksize);
bool check_padding(bytevector bv);
bytevector strip_padding(bytevector bv);

void crypto_init(void);
void crypto_cleanup(void);
void handleErrors(void);
std::array<byte, 16> random_key(void);
bytevector random_string(void);

bytevector encrypt_ecb(bytevector plaintext, byte *key, bool pad);
bytevector decrypt_ecb(bytevector ciphertext, byte *key, bool pad);

bytevector encrypt_cbc(bytevector plaintext, byte *key, byte *iv);
bytevector decrypt_cbc(bytevector ciphertext, byte *key, byte *iv);

bytevector encrypt_ctr(bytevector plaintext, byte *key, uint64_t nonce);
bytevector decrypt_ctr(bytevector ciphertext, byte *key, uint64_t nonce);

cookie parse_cookie(std::string cookie_str, char separator);

uint32_t untemper(uint32_t val);

class MT19937 {
public:
  MT19937(uint32_t seed);
  MT19937(std::array<uint32_t, 624> state);
  uint32_t next(); 

private:
  void twist();

  unsigned index;
  std::array<uint32_t, 624> mt;
};

