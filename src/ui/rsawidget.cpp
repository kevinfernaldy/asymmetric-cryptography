#include <include/ui/rsawidget.h>
#include <fstream>
#include <iostream>
#include "ui_rsawidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

RSAWidget::RSAWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RSAWidget)
{
    ui->setupUi(this);

    // Setup key generate widget
    ui->key_generate_select->addItem(QString("Generate"));
    ui->key_generate_select->addItem(QString("Import"));
    ui->key_generate_source->addItem(QString("Keyboard"));
    ui->key_generate_source->addItem(QString("File"));

    ui->from_text->setVisible(false);
    ui->key_generate_source->setVisible(false);
    ui->key_browse_widget->setVisible(true);
    ui->private_browse->setVisible(false);
    ui->public_browse->setVisible(false);
    ui->private_save_button->setVisible(true);
    ui->public_save_button->setVisible(true);
    ui->key_browse_widget->setVisible(false);
    ui->save_to_file_container->setVisible(true);

    ui->n_textbox->setReadOnly(true);
    ui->e_textbox->setReadOnly(true);
    ui->d_textbox->setReadOnly(true);

    // Setup encrypt decrypt widget
    ui->encrypt_decrypt_select->addItem(tr("encrypt"));
    ui->encrypt_decrypt_select->addItem(tr("decrypt"));
    ui->encrypt_decrypt_source->addItem(tr("keyboard"));
    ui->encrypt_decrypt_source->addItem(tr("file"));

    ui->encrypt_decrypt_file_container->setVisible(false);
    ui->decrypt_button->setVisible(false);

    rsa = nullptr;
}

RSAWidget::~RSAWidget()
{
    delete ui;
    if (rsa != nullptr) {
        delete rsa;
    }
}

void RSAWidget::read_content_private(std::string filename, std::string& n, std::string& d)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "rsa") {
            throw std::ifstream::failure("Algorithm is not RSA");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "private") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read n
        infile >> dummy >> token;
        std::string local_n = token;
        n = local_n;

        // Read d
        infile >> dummy >> token;
        std::string local_d = token;
        d = local_d;
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void RSAWidget::read_content_public(std::string filename, std::string& n, std::string& e)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "rsa") {
            throw std::ifstream::failure("Algorithm is not RSA");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "public") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read n
        infile >> dummy >> token;
        std::string local_n = token;
        n = local_n;

        // Read e
        infile >> dummy >> token;
        std::string local_e = token;
        e = local_e;
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void RSAWidget::on_key_generate_select_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->p_q_container->setVisible(true);
            ui->from_text->setVisible(false);
            ui->key_generate_source->setVisible(false);
            ui->n_textbox->setReadOnly(true);
            ui->e_textbox->setReadOnly(true);
            ui->d_textbox->setReadOnly(true);
            ui->key_browse_widget->setVisible(true);
            ui->private_browse->setVisible(false);
            ui->public_browse->setVisible(false);
            ui->private_save_button->setVisible(true);
            ui->public_save_button->setVisible(true);
            ui->save_to_file_container->setVisible(true);
            break;
        }
        case 1: {
            ui->p_q_container->setVisible(false);
            ui->from_text->setVisible(true);
            ui->key_generate_source->setVisible(true);
            ui->n_textbox->setReadOnly(false);
            ui->e_textbox->setReadOnly(false);
            ui->d_textbox->setReadOnly(false);

            ui->private_browse->setVisible(true);
            ui->public_browse->setVisible(true);
            ui->private_save_button->setVisible(false);
            ui->public_save_button->setVisible(false);
            if (ui->key_generate_source->currentIndex() == 0) {
                ui->key_browse_widget->setVisible(false);
            } else {
                ui->key_browse_widget->setVisible(true);
            }
            ui->save_to_file_container->setVisible(false);
            break;
        }
    }

    ui->n_textbox->setPlainText(QString(""));
    ui->e_textbox->setPlainText(QString(""));
    ui->d_textbox->setPlainText(QString(""));
    ui->private_key_textbox->setText(tr(""));
    ui->public_key_textbox->setText(tr(""));
}


void RSAWidget::on_key_generate_source_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->key_browse_widget->setVisible(false);
            ui->save_to_file_container->setVisible(true);
            break;
        }
        case 1: {
            ui->key_browse_widget->setVisible(true);
            ui->save_to_file_container->setVisible(false);
            break;
        }
    }
}


void RSAWidget::on_private_browse_clicked()
{
    std::string n;
    std::string d;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);

        this->read_content_private(filename.toStdString(), n, d);
        ui->n_textbox->setPlainText(tr(n.c_str()));
        ui->d_textbox->setPlainText(tr(d.c_str()));

        if (rsa == nullptr) {
            rsa = new RSA();
        }
        rsa->set_n((char *) n.c_str());
        rsa->set_d((char *) d.c_str());
    }

    delete dialog;
}


void RSAWidget::on_public_browse_clicked()
{
    std::string n;
    std::string e;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);

        this->read_content_public(filename.toStdString(), n, e);
        ui->n_textbox->setPlainText(tr(n.c_str()));
        ui->e_textbox->setPlainText(tr(e.c_str()));

        if (rsa == nullptr) {
            rsa = new RSA();
        }
        rsa->set_n((char *) n.c_str());
        rsa->set_e((char *) e.c_str());
    }

    delete dialog;
}


void RSAWidget::on_private_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);
    }

    delete dialog;
}

void RSAWidget::on_public_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);
    }

    delete dialog;
}

void RSAWidget::on_generate_button_clicked()
{
    if (rsa != nullptr) {
        delete rsa;
    }
    rsa = new RSA();

    rsa->set_p((char*) ui->p_textbox->text().toLatin1().toStdString().c_str());
    rsa->set_q((char*) ui->q_textbox->text().toLatin1().toStdString().c_str());

    rsa->generate_keys();

    char temp[4096];
    mpz_get_str(temp, 10, (*rsa->get_n()));
    ui->n_textbox->setPlainText(tr(temp));

    mpz_get_str(temp, 10, (*rsa->get_e()));
    ui->e_textbox->setPlainText(tr(temp));

    mpz_get_str(temp, 10, (*rsa->get_d()));
    ui->d_textbox->setPlainText(tr(temp));
}


void RSAWidget::on_save_key_clicked()
{
    if (rsa != nullptr) {
        rsa->dump_keys_to_file(ui->private_key_textbox->text().toLatin1().toStdString() + "/private.pem", ui->private_key_textbox->text().toLatin1().toStdString() + "/public.pem");

        QMessageBox message_box;
        message_box.information(this, tr("Info"), tr("Keys are saved as private.pem and public.pem"));
    }
}

void RSAWidget::on_encrypt_decrypt_select_currentIndexChanged(int index)
{
    if (index == 0) {
        if (ui->encrypt_decrypt_source->currentIndex() == 0) {
            ui->plaint_text->setReadOnly(false);
        } else {
            ui->plaint_text->setReadOnly(true);
        }
        ui->cipher_text->setReadOnly(true);

        ui->decrypt_button->setVisible(false);
        ui->encrypt_button->setVisible(true);
    } else {
        if (ui->encrypt_decrypt_source->currentIndex() == 0) {
            ui->cipher_text->setReadOnly(false);
        } else {
            ui->cipher_text->setReadOnly(true);
        }
        ui->plaint_text->setReadOnly(true);

        ui->decrypt_button->setVisible(true);
        ui->encrypt_button->setVisible(false);
    }
}


void RSAWidget::on_encrypt_decrypt_source_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->encrypt_decrypt_file_container->setVisible(false);
        ui->encrypt_decrypt_text_container->setVisible(true);
    } else {
        ui->encrypt_decrypt_file_container->setVisible(true);
        ui->encrypt_decrypt_text_container->setVisible(false);
    }

    if (ui->encrypt_decrypt_select->currentIndex() == 0) {
        if (index == 0) {
            ui->plaint_text->setReadOnly(false);
        } else {
            ui->plaint_text->setReadOnly(true);
        }
        ui->cipher_text->setReadOnly(true);
    } else {
        if (index == 0) {
            ui->cipher_text->setReadOnly(false);
        } else {
            ui->cipher_text->setReadOnly(true);
        }
        ui->plaint_text->setReadOnly(true);
    }
}


void RSAWidget::on_browse_source_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->file_source->setText(filename);
    }

    delete dialog;
}

void RSAWidget::on_encrypt_button_clicked()
{
    if (rsa == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string plain_text = ui->plaint_text->toPlainText().toLatin1().toStdString();
        std::string cipher_text = rsa->encrypt(plain_text);

        ui->cipher_text->setPlainText(tr(cipher_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string cipher_text = rsa->encrypt(stream.str());

        std::ofstream file_stream_output(filename + ".enc");
        file_stream_output << cipher_text;
        file_stream_output.close();
    }
}


void RSAWidget::on_decrypt_button_clicked()
{
    if (rsa == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string cipher_text = ui->cipher_text->toPlainText().toLatin1().toStdString();
        std::string plain_text = rsa->decrypt(cipher_text);

        ui->plaint_text->setPlainText(tr(plain_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename, std::ios_base::in | std::ios_base::binary);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string plain_text= rsa->decrypt(stream.str());

        std::ofstream file_stream_output(filename + ".dec", std::ios_base::out | std::ios_base::binary);
        file_stream_output << plain_text;
        file_stream_output.close();
    }
}

