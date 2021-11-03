#ifndef KEYGENERATOR_H
#define KEYGENERATOR_H

#include <gmp.h>
#include <string>

class KeyGenerator
{
public:
    KeyGenerator();

    virtual void generate_keys() = 0;
    virtual void dump_keys_to_file(std::string private_key_file, std::string public_key_file) = 0;

    void set_p(char* p);
    void set_q(char* q);

    int get_max_length();

protected:
    mpz_t p;
    mpz_t q;
};

#endif // KEYGENERATOR_H
