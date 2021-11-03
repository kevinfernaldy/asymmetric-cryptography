#ifndef ELGAMAL_H
#define ELGAMAL_H

#include <include/crypto/keygenerator.h>
#include <string>

class ElGamal : public KeyGenerator
{
public:
    ElGamal();

    void init(char* p, char* g, char* x);
    void generate_keys();
    void dump_keys_to_file(std::string private_key_file, std::string public_key_file);

    std::string encrypt(std::string plain_text);
    std::string decrypt(std::string cipher_text);

    mpz_t* get_g();
    mpz_t* get_x();
    mpz_t* get_y();
    void set_g(char* g);
    void set_x(char* x);
    void set_y(char* y);

private:
    mpz_t g;
    mpz_t y;    // Public key
    mpz_t x;    // Private key
};


#endif // ELGAMAL_H
