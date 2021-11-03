#ifndef PAILLIERWIDGET_H
#define PAILLIERWIDGET_H

#include <QWidget>
#include <include/crypto/paillier.h>

namespace Ui {
class PaillierWidget;
}

class PaillierWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PaillierWidget(QWidget *parent = nullptr);
    ~PaillierWidget();

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
    void read_content_public(std::string filename, std::string& lambda, std::string& mu);
    void read_content_private(std::string filename, std::string& g, std::string& n);

    Ui::PaillierWidget *ui;
    Paillier *paillier;
};

#endif // PAILLIERWIDGET_H
