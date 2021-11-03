#include <include/crypto/rsa.h>
#include <include/util.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>

RSA::RSA() : KeyGenerator()
{
    mpz_init(this->n);
    mpz_init(this->e);
    mpz_init(this->d);
}

void RSA::generate_keys() {
    // Calculate n
    mpz_t n;
    mpz_init(n);
    mpz_mul(n, this->p, this->q);

    // Calculate lambda n
    mpz_t lambda_n;
    mpz_t p_1;
    mpz_t q_1;
    mpz_init(lambda_n);
    mpz_init(p_1);
    mpz_init(q_1);

    mpz_sub_ui(p_1, this->p, 1);
    mpz_sub_ui(q_1, this->q, 1);

    mpz_lcm(lambda_n, p_1, q_1);

    // Calculate e such that e is coprime to lambda_n
    mpz_t e;
    mpz_init(e);
    mpz_set_ui(e, 1);

    mpz_t gcd;
    mpz_init(gcd);

    do {
        mpz_add_ui(e, e, 1);
        mpz_gcd(gcd, lambda_n, e);

        char temp[4096];
        mpz_get_str(temp, 10, e);
    } while ((mpz_cmp_ui(gcd, 1) != 0) && (mpz_cmp(e, lambda_n) < 0));

    if (mpz_cmp_ui(gcd, 1) != 0) {
        throw std::invalid_argument("Cannot find coprime");
    }

    // Calculate d
    mpz_t d;
    mpz_init(d);
    mpz_invert(d, e, lambda_n);

    // Set keys
    mpz_set(this->d, d);
    mpz_set(this->e, e);
    mpz_set(this->n, n);
}

void RSA::init(char* p, char* q) {
    this->set_p(p);
    this->set_q(q);

    this->generate_keys();
}

mpz_t* RSA::get_n() {
    return &this->n;
}

mpz_t* RSA::get_e() {
    return &this->e;
}

mpz_t* RSA::get_d() {
    return &this->d;
}

void RSA::set_n(char* n) {
    mpz_set_str(this->n, n, 10);
}

void RSA::set_e(char* e) {
    mpz_set_str(this->e, e, 10);
}

void RSA::set_d(char* d) {
    mpz_set_str(this->d, d, 10);
}

void RSA::dump_keys_to_file(std::string private_key_file, std::string public_key_file) {
    std::ofstream private_key(private_key_file, std::ios_base::out);
    std::ofstream public_key(public_key_file, std::ios_base::out);

    char temp_n[4096];
    char temp_e[4096];
    char temp_d[4096];

    mpz_get_str(temp_n, 10, this->n);
    mpz_get_str(temp_e, 10, this->e);
    mpz_get_str(temp_d, 10, this->d);

    private_key << "algorithm rsa" << std::endl;
    private_key << "type private" << std::endl;
    private_key << "n " << temp_n << std::endl;
    private_key << "d " << temp_d << std::endl;

    public_key << "algorithm rsa" << std::endl;
    public_key << "type public" << std::endl;
    public_key << "n " << temp_n << std::endl;
    public_key << "e " << temp_e << std::endl;

    private_key.close();
    public_key.close();
}

std::string RSA::encrypt(std::string plain_text)
{
    std::vector<unsigned int> plain_block = Util::convert_and_pad(plain_text);

    std::ostringstream cipher_stream;
    for (std::vector<unsigned int>::iterator it = plain_block.begin(); it != plain_block.end(); it++) {
        mpz_t temp_plain_block;
        mpz_init(temp_plain_block);
        mpz_set_ui(temp_plain_block, (unsigned int) *it);

        mpz_t temp_cipher_block;
        mpz_init(temp_cipher_block);

        mpz_powm(temp_cipher_block, temp_plain_block, this->e, this->n);

        char temp[4096];
        mpz_get_str(temp, 10, temp_cipher_block);

        std::string temp_str(temp);
        int length = this->get_max_length() - temp_str.length();
        for (int i = 0; i < length; i++) {
            temp_str = "0" + temp_str;
        }
        cipher_stream << temp_str << ' ';
    }


    return cipher_stream.str();
}

std::string RSA::decrypt(std::string cipher_text)
{
    std::istringstream cipher_text_stream(cipher_text);
    std::ostringstream plain_text_stream;

    std::string word;
    while (cipher_text_stream >> word) {
        mpz_t c;
        mpz_init(c);
        mpz_set_str(c, word.c_str(), 10);

        mpz_t m;
        mpz_init(m);
        mpz_powm(m, c, this->d, this->n);

        unsigned int temp = (unsigned int) mpz_get_ui(m);


        plain_text_stream << (char) (temp / 1000) << (char) (temp % 1000);
    }

    return plain_text_stream.str();
}
