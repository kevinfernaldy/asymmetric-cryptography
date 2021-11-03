#ifndef RSA_H
#define RSA_H

#include <include/crypto/keygenerator.h>
#include <string>

class RSA : public KeyGenerator
{
public:
    RSA();

    void init(char* p, char* q);
    void generate_keys();
    void dump_keys_to_file(std::string private_key_file, std::string public_key_file);

    std::string encrypt(std::string plain_text);
    std::string decrypt(std::string cipher_text);

    mpz_t* get_n();
    mpz_t* get_e();
    mpz_t* get_d();
    void set_n(char* n);
    void set_e(char* e);
    void set_d(char* d);

private:
    mpz_t n;
    mpz_t e;    // Public key
    mpz_t d;    // Private key
};


#endif // RSA_H
