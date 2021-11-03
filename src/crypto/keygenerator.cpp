#include <include/crypto/keygenerator.h>
#include <gmp.h>
#include <cstdlib>
#include <iostream>
#include <cstring>

KeyGenerator::KeyGenerator()
{
    mpz_init(this->p);
    mpz_init(this->q);
}

void KeyGenerator::set_p(char *p)
{
    mpz_set_str(this->p, p, 10);
}

void KeyGenerator::set_q(char *q)
{
    mpz_set_str(this->q, q, 10);
}

int KeyGenerator::get_max_length() {
    char temp[4096];
    memset(temp, 0x0, 4096 * sizeof(char));
    int length = 0;

    mpz_get_str(temp, 10, p);
    length += strlen(temp);
    mpz_get_str(temp, 10, q);
    length += strlen(temp);

    return length;
}
