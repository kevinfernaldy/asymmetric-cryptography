#include <include/crypto/elgamal.h>
#include <random>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <include/util.h>

ElGamal::ElGamal() : KeyGenerator()
{
    mpz_init(this->p);
    mpz_init(this->g);
    mpz_init(this->x);
    mpz_init(this->y);
}

void ElGamal::init(char* p, char* g, char* x)
{
    this->set_p(p);
    this->set_g(g);
    this->set_x(x);

    if (mpz_cmp(this->g, this->p) >= 0) {
        mpz_init(this->p);
        mpz_init(this->g);
        mpz_init(this->x);
        throw std::invalid_argument("g must be less than p");
    }
    mpz_t one;
    mpz_t p_min_2;
    mpz_init(one);
    mpz_init(p_min_2);
    mpz_set_ui(one, 1);
    mpz_sub_ui(p_min_2, this->p, 2);

    if ((mpz_cmp(one, this->x) > 0) || (mpz_cmp(this->x, p_min_2) > 0)) {
        mpz_init(this->p);
        mpz_init(this->g);
        mpz_init(this->x);
        throw std::invalid_argument("x must be 1 <= x <= p-2");
    }

    this->generate_keys();
}

void ElGamal::generate_keys()
{
    mpz_t y;
    mpz_init(y);
    mpz_powm(y, this->g, this->x, this->p);

    mpz_set(this->y, y);
}

void ElGamal::dump_keys_to_file(std::string private_key_file, std::string public_key_file)
{
    std::ofstream private_key(private_key_file, std::ios_base::out);
    std::ofstream public_key(public_key_file, std::ios_base::out);

    char temp_x[4096];
    char temp_y[4096];
    char temp_g[4096];
    char temp_p[4096];

    mpz_get_str(temp_x, 10, this->x);
    mpz_get_str(temp_y, 10, this->y);
    mpz_get_str(temp_g, 10, this->g);
    mpz_get_str(temp_p, 10, this->p);

    private_key << "algorithm elgamal" << std::endl;
    private_key << "type private" << std::endl;
    private_key << "x " << temp_x << std::endl;
    private_key << "p " << temp_p << std::endl;

    public_key << "algorithm elgamal" << std::endl;
    public_key << "type public" << std::endl;
    public_key << "y " << temp_y << std::endl;
    public_key << "g " << temp_g << std::endl;
    public_key << "p " << temp_p << std::endl;

    private_key.close();
    public_key.close();
}

std::string ElGamal::encrypt(std::string plain_text)
{
    std::vector<unsigned int> plain_block = Util::convert_and_pad(plain_text);

    std::ostringstream cipher_stream;
    for (std::vector<unsigned int>::iterator it = plain_block.begin(); it != plain_block.end(); it++) {
        mpz_t m;
        mpz_init(m);
        mpz_set_ui(m, (unsigned int) *it);

        mpz_t a;
        mpz_t b;
        mpz_t b_1;
        mpz_t b_2;
        mpz_init(a);
        mpz_init(b);
        mpz_init(b_1);
        mpz_init(b_2);

        mpz_powm(a, this->g, this->x, this->p);
        mpz_powm(b_1, this->y, this->x, this->p);
        mpz_powm_ui(b_2, m, 1, this->p);
        mpz_mul(b, b_1, b_2);

        char a_char[4096];
        char b_char[4096];
        mpz_get_str(a_char, 10, a);
        mpz_get_str(b_char, 10, b);

        cipher_stream << a_char << " " << b_char << std::endl;
    }


    return cipher_stream.str();
}

std::string ElGamal::decrypt(std::string cipher_text)
{
    std::istringstream stream(cipher_text);
    std::ostringstream plain_text_stream;

    std::string a;
    std::string b;
    while (stream >> a >> b) {
        mpz_t a_t;
        mpz_t b_t;
        mpz_init(a_t);
        mpz_init(b_t);
        mpz_set_str(a_t, a.c_str(), 10);
        mpz_set_str(b_t, b.c_str(), 10);

        // Calculate 1/a^x
        mpz_t power;
        mpz_init(power);
        mpz_set(power, this->p);
        mpz_sub_ui(power, power, 1);
        mpz_sub(power, power, this->x);

        mpz_t a_inv;
        mpz_init(a_inv);
        mpz_powm(a_inv, a_t, power, this->p);

        // Calculate m
        mpz_t m;
        mpz_init(m);
        mpz_mul(m, b_t, a_inv);
        mpz_mod(m, m, this->p);

        unsigned int temp = mpz_get_ui(m);

        plain_text_stream << (char) (temp / 1000) << (char) (temp % 1000);
    }

    return plain_text_stream.str();
}

mpz_t* ElGamal::get_g()
{
    return &this->g;
}

mpz_t* ElGamal::get_x()
{
    return &this->x;
}

mpz_t* ElGamal::get_y()
{
    return &this->y;
}

void ElGamal::set_g(char* g)
{
    mpz_set_str(this->g, g, 10);
}

void ElGamal::set_x(char* x)
{
    mpz_set_str(this->x, x, 10);
}

void ElGamal::set_y(char* y)
{
    mpz_set_str(this->y, y, 10);
}
