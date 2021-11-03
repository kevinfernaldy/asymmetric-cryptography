#include <include/crypto/ecc.h>
#include <iostream>
#include <sstream>
#include <fstream>

ECC::ECC()
{
    mpz_init(this->base_x);
    mpz_init(this->base_y);
    mpz_init(this->base_k);

    mpz_init(this->p);
    mpz_init(this->a_ecc);
    mpz_init(this->b_ecc);

    mpz_init(this->private_key);
    mpz_init(this->public_key_x);
    mpz_init(this->public_key_y);
}

void ECC::initECC(char *a, char *b, char *p)
{
    this->set_a_ecc(a);
    this->set_b_ecc(b);
    this->set_p(p);

    mpz_set_ui(this->base_x, 0);
    mpz_t root_remainder;
    mpz_init(root_remainder);
    do {
        mpz_add_ui(this->base_x, this->base_x, 1);

        mpz_pow_ui(this->base_y, this->base_x, 3);  // x^3
        mpz_t ax;
        mpz_init(ax);
        mpz_mul(ax, this->base_x, this->a_ecc); // a*x

        mpz_add(this->base_y, this->base_y, ax);
        mpz_add(this->base_y, this->base_y, this->b_ecc);   // x^3 + ax + b
        mpz_mod(this->base_y, this->base_y, this->p);   // x^3 + ax + b mod p

        mpz_sqrtrem(this->base_y, root_remainder, this->base_y);
    } while (mpz_cmp_ui(root_remainder, 0));
}

void ECC::initClient(char *private_key, char *base_k)
{
    mpz_set_str(this->private_key, private_key, 10);
    mpz_mul(this->public_key_x, this->private_key, this->base_x);
    mpz_mul(this->public_key_y, this->private_key, this->base_y);

    std::cout << mpz_get_ui(this->private_key) << std::endl;
    std::cout << mpz_get_ui(base_y) << std::endl;
    std::cout << mpz_get_ui(public_key_y) << std::endl;

    mpz_set_str(this->base_k, base_k, 10);
}

void ECC::dump_keys_to_file(std::string private_key_file, std::string public_key_file) {
    std::ofstream private_key(private_key_file, std::ios_base::out);
    std::ofstream public_key(public_key_file, std::ios_base::out);

    private_key << "algorithm eceg" << std::endl;
    private_key << "type private" << std::endl;
    public_key << "algorithm eceg" << std::endl;
    public_key << "type public" << std::endl;

    char temp[4096];

    mpz_get_str(temp, 10, this->a_ecc);
    private_key << "ecc_a " << temp << std::endl;
    public_key << "ecc_a " << temp << std::endl;
    mpz_get_str(temp, 10, this->b_ecc);
    private_key << "ecc_b " << temp << std::endl;
    public_key << "ecc_b " << temp << std::endl;
    mpz_get_str(temp, 10, this->p);
    private_key << "mod_p " << temp << std::endl;
    public_key << "mod_p " << temp << std::endl;
    mpz_get_str(temp, 10, this->base_x);
    private_key << "base_x " << temp << std::endl;
    public_key << "base_x " << temp << std::endl;
    mpz_get_str(temp, 10, this->base_y);
    private_key << "base_y " << temp << std::endl;
    public_key << "base_y " << temp << std::endl;
    mpz_get_str(temp, 10, this->base_k);
    private_key << "const_k " << temp << std::endl;
    public_key << "const_k " << temp << std::endl;
    mpz_get_str(temp, 10, this->private_key);
    private_key << "key " << temp << std::endl;
    mpz_get_str(temp, 10, this->public_key_y);
    public_key << "key " << temp << std::endl;

    private_key.close();
    public_key.close();
}

std::string ECC::encrypt(std::string plain_text)
{
    std::stringstream pairs = this->encodePlainText(plain_text);

    mpz_t kb_x;
    mpz_t kb_y;
    mpz_t pc_x;
    mpz_t pc_y;
    mpz_init(kb_x);
    mpz_init(kb_y);
    mpz_init(pc_x);
    mpz_init(pc_y);

    std::ostringstream stream;
    std::string s_x;
    std::string s_y;
    while (pairs >> s_x >> s_y) {
        mpz_t m_x;
        mpz_t m_y;
        mpz_init(m_x);
        mpz_init(m_y);
        mpz_set_str(m_x, s_x.c_str(), 10);
        mpz_set_str(m_y, s_y.c_str(), 10);

        mpz_mul(kb_x, this->base_k, this->base_x);
        mpz_mul(kb_y, this->base_k, this->base_y);

        mpz_set(pc_x, this->public_key_x);
        mpz_set(pc_y, this->public_key_y);
        mpz_mul(pc_x, pc_x, this->base_k);
        mpz_mul(pc_y, pc_y, this->base_k);
        mpz_add(pc_x, pc_x, m_x);
        mpz_add(pc_y, pc_y, m_y);

        char temp[4096];
        mpz_get_str(temp, 10, kb_x);
        stream << temp << " ";
        mpz_get_str(temp, 10, kb_y);
        stream << temp << " ";
        mpz_get_str(temp, 10, pc_x);
        stream << temp << " ";
        mpz_get_str(temp, 10, pc_y);
        stream << temp << std::endl;
    }

    return stream.str();
}

std::string ECC::decrypt(std::string cipher_text)
{
    std::istringstream stream(cipher_text);
    std::ostringstream plain_text;

    std::string kb_x;
    std::string kb_y;
    std::string pm_x;
    std::string pm_y;

    while (stream >> kb_x >> kb_y >> pm_x >> pm_y) {
        mpz_t kb_x_t;
        mpz_t kb_y_t;
        mpz_t pm_x_t;
        mpz_t pm_y_t;
        mpz_init(kb_x_t);
        mpz_init(kb_y_t);
        mpz_init(pm_x_t);
        mpz_init(pm_y_t);
        mpz_set_str(kb_x_t, kb_x.c_str(), 10);
        mpz_set_str(kb_y_t, kb_y.c_str(), 10);
        mpz_set_str(pm_x_t, pm_x.c_str(), 10);
        mpz_set_str(pm_y_t, pm_y.c_str(), 10);

        // b * kB
        mpz_t bkb_x;
        mpz_t bkb_y;
        mpz_init(bkb_x);
        mpz_init(bkb_y);
        mpz_mul(bkb_x, kb_x_t, this->private_key);
        mpz_mul(bkb_y, kb_y_t, this->private_key);

        // Pc - b * kB
        mpz_sub(pm_x_t, pm_x_t, bkb_x);
        mpz_sub(pm_y_t, pm_y_t, bkb_y);

        // Decode m
        plain_text << (char) this->decodePlainText(&pm_x_t);
    }

    return plain_text.str();
}

std::stringstream ECC::encodePlainText(std::string plainText)
{
    std::stringstream stream;
    for (char& c : plainText) {
        mpz_t x;
        mpz_init(x);
        mpz_set_ui(x, c);

        // Search for y
        mpz_t y;
        mpz_init(y);
        mpz_mul(x, x, this->base_k);
        mpz_t root_remainder;
        mpz_init(root_remainder);
        do {
            mpz_add_ui(x, x, 1);

            mpz_t ax;   // a*x
            mpz_init(ax);
            mpz_mul(ax, this->a_ecc, x);
            mpz_pow_ui(y, x, 3);    // x^3
            mpz_add(y, y, ax);      // x^3 + ax
            mpz_add(y, y, this->b_ecc); // x^3 + ax + b
            mpz_mod(y, y, this->p);

            mpz_sqrtrem(y, root_remainder, y);
        } while(mpz_cmp_ui(root_remainder, 0));

        char temp[4096];
        mpz_get_str(temp, 10, x);
        stream << temp << " ";
        mpz_get_str(temp, 10, y);
        stream << temp << std::endl;
    }

    return stream;
}

unsigned int ECC::decodePlainText(mpz_t* cipher_word)
{
    mpz_t m;
    mpz_init(m);
    mpz_set(m, *cipher_word);
    mpz_sub_ui(m, m, 1);
    mpz_div(m, m, this->base_k);

    return mpz_get_ui(m);
}

mpz_t* ECC::get_base_x() {
    return &this->base_x;
}
mpz_t* ECC::get_base_y() {
    return &this->base_y;
}
mpz_t* ECC::get_base_k() {
    return &this->base_k;
}
mpz_t* ECC::get_p() {
    return &this->p;
}
mpz_t* ECC::get_a_ecc() {
    return &this->a_ecc;
}
mpz_t* ECC::get_b_ecc() {
    return &this->b_ecc;
}
mpz_t* ECC::get_private_key() {
    return &this->private_key;
}
mpz_t* ECC::get_public_key_x() {
    return &this->public_key_x;
}
mpz_t* ECC::get_public_key_y() {
    return &this->public_key_y;
}
void ECC::set_base_x(char* base_x) {
    mpz_set_str(this->base_x, base_x, 10);
}
void ECC::set_base_y(char* base_y) {
    mpz_set_str(this->base_y, base_y, 10);
}
void ECC::set_base_k(char* base_k) {
    mpz_set_str(this->base_k, base_k, 10);
}
void ECC::set_p(char* p) {
    mpz_set_str(this->p, p, 10);
}
void ECC::set_a_ecc(char* a_ecc) {
    mpz_set_str(this->a_ecc, a_ecc, 10);
}
void ECC::set_b_ecc(char* b_ecc) {
    mpz_set_str(this->b_ecc, b_ecc, 10);
}
void ECC::set_private_key(char* private_key) {
    mpz_set_str(this->private_key, private_key, 10);
}
void ECC::set_public_key_x(char* public_key_x) {
    mpz_set_str(this->public_key_x, public_key_x, 10);
}
void ECC::set_public_key_y(char* public_key_y) {
    mpz_set_str(this->public_key_y, public_key_y, 10);
}
