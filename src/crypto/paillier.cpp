#include <include/crypto/paillier.h>
#include <include/util.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>

Paillier::Paillier() : KeyGenerator()
{
    mpz_init(this->g);
    mpz_init(this->n);
    mpz_init(this->lambda);
    mpz_init(this->mu);
}

void Paillier::init(char *p, char *q, char* g)
{
    this->set_p(p);
    this->set_q(q);

    // Check if p and q has GCD of 1
    mpz_t pq;
    mpz_init(pq);
    mpz_mul(pq, this->p, this->q);

    mpz_t p_minus;
    mpz_t q_minus;
    mpz_t pq_minus;
    mpz_init(p_minus);
    mpz_init(q_minus);
    mpz_init(pq_minus);
    mpz_sub_ui(p_minus, this->p, 1);
    mpz_sub_ui(q_minus, this->q, 1);
    mpz_mul(pq_minus, p_minus, q_minus);

    mpz_t gcd;
    mpz_init(gcd);
    mpz_gcd(gcd, pq, pq_minus);
    if (mpz_cmp_ui(gcd, 1) != 0) {
        mpz_init(this->p);
        mpz_init(this->q);
        throw std::invalid_argument("GCD is not 1");
    }

    // Check if g < pq
    this->set_g(g);

    mpz_t n2;
    mpz_init(n2);
    mpz_mul(n2, pq, pq);
    if (mpz_cmp(this->g, n2) >= 0) {
        mpz_init(this->p);
        mpz_init(this->q);
        mpz_init(this->g);
        throw std::invalid_argument("g must be less than (pq)^2");
    }

    this->generate_keys();
}

void Paillier::generate_keys()
{
    // Set n
    mpz_mul(this->n, this->p, this->q);

    // Set lambda
    mpz_t p_minus;
    mpz_t q_minus;
    mpz_init(p_minus);
    mpz_init(q_minus);
    mpz_set(p_minus, this->p);
    mpz_set(q_minus, this->q);
    mpz_sub_ui(p_minus, p_minus, 1);
    mpz_sub_ui(q_minus, q_minus, 1);
    mpz_lcm(this->lambda, p_minus, q_minus);

    // Set mu
    mpz_t n2;   // n^2
    mpz_init(n2);
    mpz_mul(n2, this->n, this->n);

    mpz_t mu;
    mpz_init(mu);
    mpz_set(mu, this->g);
    mpz_powm(mu, this->g, this->lambda, n2);  // g^lambda mod n^2

    std::cout << mpz_get_ui(mu) << std::endl;

    mpz_sub_ui(mu, mu, 1);
    mpz_div(mu, mu, this->n);   // L(g) = (g-1)/n

    std::cout << mpz_get_ui(mu) << std::endl;

//    mpz_t one;
//    mpz_init(one);
//    mpz_set_ui(one, 1);
//    mpz_div(mu, one, mu);
    mpz_invert(mu, mu, this->n);

    mpz_set(this->mu, mu);
}

void Paillier::dump_keys_to_file(std::string private_key_file, std::string public_key_file)
{
    std::ofstream private_key(private_key_file, std::ios_base::out);
    std::ofstream public_key(public_key_file, std::ios_base::out);

    char temp_g[4096];
    char temp_n[4096];
    char temp_lambda[4096];
    char temp_mu[4096];

    mpz_get_str(temp_g, 10, this->g);
    mpz_get_str(temp_n, 10, this->n);
    mpz_get_str(temp_lambda, 10, this->lambda);
    mpz_get_str(temp_mu, 10, this->mu);

    private_key << "algorithm paillier" << std::endl;
    private_key << "type private" << std::endl;
    private_key << "lambda " << temp_lambda << std::endl;
    private_key << "mu " << temp_mu << std::endl;

    public_key << "algorithm paillier" << std::endl;
    public_key << "type public" << std::endl;
    public_key << "g " << temp_g << std::endl;
    public_key << "n " << temp_n << std::endl;

    private_key.close();
    public_key.close();
}

std::string Paillier::encrypt(std::string plain_text, std::string r_s)
{
    // Check if r is valid
    mpz_t r;
    mpz_init(r);
    mpz_set_str(r, r_s.c_str(), 10);
    if ((mpz_cmp_ui(r, 0) < 0) || (mpz_cmp(r, this->n) >= 0)) {
        throw std::invalid_argument("r must be 0 <= r < n");
    }

    mpz_t gcd;
    mpz_init(gcd);
    mpz_gcd(gcd, r, this->n);
    if (mpz_get_ui(gcd) != 1) {
        throw std::invalid_argument("GCD of r and n must be 1");
    }

    mpz_t n2;
    mpz_init(n2);
    mpz_mul(n2, this->n, this->n);

    std::vector<unsigned int> plain_block = Util::convert_and_pad(plain_text);

    std::ostringstream cipher_stream;
    for (std::vector<unsigned int>::iterator it = plain_block.begin(); it != plain_block.end(); it++) {
        char temp[4096];

        // g^m mod n^2
        mpz_t gm;
        mpz_init(gm);
        mpz_powm_ui(gm, this->g, *it, n2);

        mpz_get_str(temp, 10, gm);

        // r^n mod n^2
        mpz_t rn;
        mpz_init(rn);
        mpz_powm(rn, r, this->n, n2);

        mpz_get_str(temp, 10, rn);

        // g^m * r^n mod n^2
        mpz_t c;
        mpz_init(c);
        mpz_mul(c, gm, rn);

        mpz_get_str(temp, 10, c);

        char c_char[4096];
        mpz_get_str(c_char, 10, c);

        cipher_stream << c_char << " ";
    }

    return cipher_stream.str();
}

std::string Paillier::decrypt(std::string cipher_text)
{
    std::istringstream cipher_text_stream(cipher_text);
    std::ostringstream plain_text_stream;

    mpz_t n2;
    mpz_init(n2);
    mpz_mul(n2, this->n, this->n);

    std::string word;
    while (cipher_text_stream >> word) {
        mpz_t c;
        mpz_init(c);
        mpz_set_str(c, word.c_str(), 10);

        // L(c^lambda mod n^2)
        mpz_t l;
        mpz_init(l);
        mpz_powm(l, c, this->lambda, n2);
        mpz_sub_ui(l, l, 1);
        mpz_div(l, l, this->n);

        // L * mu
        mpz_mul(l, l, this->mu);

        // m = L * mu mod n
        mpz_t m;
        mpz_init(m);
        mpz_mod(m, l, this->n);

        unsigned int temp = mpz_get_ui(m);

        plain_text_stream << (char) (temp / 1000) << (char) (temp % 1000);
    }

    return plain_text_stream.str();
}

mpz_t* Paillier::get_g()
{
    return &this->g;
}
mpz_t* Paillier::get_n()
{
    return &this->n;
}
mpz_t* Paillier::get_lambda()
{
    return &this->lambda;
}
mpz_t* Paillier::get_mu()
{
    return &this->mu;
}
void Paillier::set_g(char *g)
{
    mpz_set_str(this->g, g, 10);
}
void Paillier::set_n(char *n)
{
    mpz_set_str(this->n, n, 10);
}
void Paillier::set_lambda(char *lambda)
{
    mpz_set_str(this->lambda, lambda, 10);
}
void Paillier::set_mu(char *mu)
{
    mpz_set_str(this->mu, mu, 10);
}
