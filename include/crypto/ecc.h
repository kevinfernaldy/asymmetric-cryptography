#ifndef ECC_H
#define ECC_H

#include <gmp.h>
#include <vector>
#include <utility>

class ECC
{
public:
    ECC();

    void initECC(char* a, char* b, char* p);
    void initClient(char* private_key, char* base_k);

    void dump_keys_to_file(std::string private_key_file, std::string public_key_file);

    mpz_t* get_base_x();
    mpz_t* get_base_y();
    mpz_t* get_base_k();
    mpz_t* get_p();
    mpz_t* get_a_ecc();
    mpz_t* get_b_ecc();
    mpz_t* get_private_key();
    mpz_t* get_public_key_x();
    mpz_t* get_public_key_y();
    void set_base_x(char* base_x);
    void set_base_y(char* base_y);
    void set_base_k(char* base_k);
    void set_p(char* p);
    void set_a_ecc(char* a_ecc);
    void set_b_ecc(char* b_ecc);
    void set_private_key(char* private_key);
    void set_public_key_x(char* public_key_x);
    void set_public_key_y(char* public_key_y);

    std::string encrypt(std::string plain_text);
    std::string decrypt(std::string cipher_text);
private:
    std::stringstream encodePlainText(std::string plainText);
    unsigned int decodePlainText(mpz_t* cipherWord);

    // Base
    mpz_t base_x;
    mpz_t base_y;
    mpz_t base_k;

    // ECC
    mpz_t p;
    mpz_t a_ecc;
    mpz_t b_ecc;

    // Key
    mpz_t private_key;    // private
    mpz_t public_key_x;  // public
    mpz_t public_key_y;  // public
};

#endif // ECC_H
