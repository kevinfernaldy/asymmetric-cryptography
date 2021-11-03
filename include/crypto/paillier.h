#ifndef PAILLIER_H
#define PAILLIER_H

#include <include/crypto/keygenerator.h>
#include <string>

class Paillier : public KeyGenerator
{
public:
    Paillier();

    void init(char* p, char* q, char* g);
    void generate_keys();
    void dump_keys_to_file(std::string private_key_file, std::string public_key_file);

    std::string encrypt(std::string plain_text, std::string r_s);
    std::string decrypt(std::string cipher_text);

    mpz_t* get_g();
    mpz_t* get_n();
    mpz_t* get_lambda();
    mpz_t* get_mu();
    void set_g(char* g);
    void set_n(char* n);
    void set_lambda(char* lambda);
    void set_mu(char* mu);

private:
    // Public key
    mpz_t g;
    mpz_t n;
    // Private key
    mpz_t lambda;
    mpz_t mu;
};

#endif // PAILLIER_H
