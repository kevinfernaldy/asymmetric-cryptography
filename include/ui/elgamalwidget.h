#ifndef ELGAMALWIDGET_H
#define ELGAMALWIDGET_H

#include <QWidget>
#include <include/crypto/elgamal.h>

namespace Ui {
class ElGamalWidget;
}

class ElGamalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ElGamalWidget(QWidget *parent = nullptr);
    ~ElGamalWidget();

private slots:
    void on_key_generate_select_currentIndexChanged(int index);
    void on_key_generate_source_currentIndexChanged(int index);
    void on_private_browse_clicked();
    void on_public_browse_clicked();
    void on_generate_button_clicked();
    void on_save_key_clicked();
    void on_private_save_button_clicked();
    void on_public_save_button_clicked();
    void on_encrypt_decrypt_select_currentIndexChanged(int index);
    void on_encrypt_decrypt_source_currentIndexChanged(int index);
    void on_browse_source_clicked();

    void on_encrypt_button_clicked();

    void on_decrypt_button_clicked();

private:
    void read_content_public(std::string filename, std::string& p, std::string& g, std::string& y);
    void read_content_private(std::string filename, std::string& p, std::string& x);

    Ui::ElGamalWidget *ui;
    ElGamal *elgamal;
};

#endif // ELGAMALWIDGET_H
