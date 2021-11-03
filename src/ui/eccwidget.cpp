#include <include/ui/eccwidget.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_eccwidget.h"

ECCWidget::ECCWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ECCWidget)
{
    ui->setupUi(this);

    // Setup key generate widget
    ui->key_generate_select->addItem(QString("Generate"));
    ui->key_generate_select->addItem(QString("Import"));
    ui->key_generate_source->addItem(QString("Keyboard"));
    ui->key_generate_source->addItem(QString("File"));

    ui->key_generate_source->setVisible(false);
    ui->from_text->setVisible(false);
    ui->private_browse->setVisible(false);
    ui->public_browse->setVisible(false);

    ui->public_key_textbox->setReadOnly(true);
    ui->key_browse_widget->setVisible(true);
    ui->x_textbox->setReadOnly(true);
    ui->y_textbox->setReadOnly(true);

    // Setup encrypt decrypt widget
    ui->encrypt_decrypt_select->addItem(tr("encrypt"));
    ui->encrypt_decrypt_select->addItem(tr("decrypt"));
    ui->encrypt_decrypt_source->addItem(tr("keyboard"));
    ui->encrypt_decrypt_source->addItem(tr("file"));

    ui->encrypt_decrypt_file_container->setVisible(false);
    ui->decrypt_button->setVisible(false);

    ecc = nullptr;
}

ECCWidget::~ECCWidget()
{
    delete ui;
    if (ecc != nullptr) {
        delete ecc;
    }
}

void ECCWidget::read_content_private(std::string filename)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "eceg") {
            throw std::ifstream::failure("Algorithm is not ECC ElGamal");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "private") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read ecc_a
        infile >> dummy >> token;
        ui->a_textbox->setText(tr(token.c_str()));
        ecc->set_a_ecc((char *) token.c_str());

        // Read ecc_b
        infile >> dummy >> token;
        ui->b_textbox->setText(tr(token.c_str()));
        ecc->set_b_ecc((char *) token.c_str());

        // Read mod_p
        infile >> dummy >> token;
        ui->p_textbox_2->setText(tr(token.c_str()));
        ecc->set_p((char *) token.c_str());

        // Read base X
        infile >> dummy >> token;
        ui->x_textbox->setText(tr(token.c_str()));
        ecc->set_base_x((char *) token.c_str());

        // Read base Y
        infile >> dummy >> token;
        ui->y_textbox->setText(tr(token.c_str()));
        ecc->set_base_y((char *) token.c_str());

        // Read k
        infile >> dummy >> token;
        ui->k_textbox->setText(tr(token.c_str()));
        ecc->set_base_k((char *) token.c_str());

        // Read key
        infile >> dummy >> token;
        ui->private_textbox->setPlainText(tr(token.c_str()));
        ecc->set_private_key((char *) token.c_str());

    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void ECCWidget::read_content_public(std::string filename)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "eceg") {
            throw std::ifstream::failure("Algorithm is not ECC ElGamal");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "public") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read ecc_a
        infile >> dummy >> token;
        ui->a_textbox->setText(tr(token.c_str()));
        ecc->set_a_ecc((char *) token.c_str());

        // Read ecc_b
        infile >> dummy >> token;
        ui->b_textbox->setText(tr(token.c_str()));
        ecc->set_b_ecc((char *) token.c_str());

        // Read mod_p
        infile >> dummy >> token;
        ui->p_textbox_2->setText(tr(token.c_str()));
        ecc->set_p((char *) token.c_str());

        // Read base X
        infile >> dummy >> token;
        ui->x_textbox->setText(tr(token.c_str()));
        ecc->set_base_x((char *) token.c_str());

        // Read base Y
        infile >> dummy >> token;
        ui->y_textbox->setText(tr(token.c_str()));
        ecc->set_base_y((char *) token.c_str());

        // Read k
        infile >> dummy >> token;
        ui->k_textbox->setText(tr(token.c_str()));
        ecc->set_base_k((char *) token.c_str());

        // Read key
        infile >> dummy >> token;
        ui->public_textbox->setPlainText(tr(token.c_str()));
        ecc->set_private_key((char *) token.c_str());
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void ECCWidget::on_key_generate_select_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->private_browse->setVisible(false);
            ui->public_browse->setVisible(false);
            ui->private_save_button->setVisible(true);
            ui->public_save_button->setVisible(true);
            ui->save_to_file_container->setVisible(true);

            ui->key_generate_source->setVisible(false);
            ui->from_text->setVisible(false);
            break;
        }
        case 1: {
            ui->private_browse->setVisible(true);
            ui->public_browse->setVisible(true);
            ui->private_save_button->setVisible(false);
            ui->public_save_button->setVisible(false);
            ui->save_to_file_container->setVisible(false);

            ui->key_generate_source->setVisible(true);
            ui->from_text->setVisible(true);

            if (ui->key_generate_source->currentIndex() == 0) {
                ui->key_browse_widget->setVisible(false);
                ui->a_textbox->setReadOnly(false);
                ui->b_textbox->setReadOnly(false);
                ui->p_textbox_2->setReadOnly(false);
                ui->x_textbox->setReadOnly(false);
                ui->y_textbox->setReadOnly(false);
                ui->private_textbox->setReadOnly(false);
                ui->public_key_textbox->setReadOnly(false);
                ui->public_key_textbox->setReadOnly(false);
                ui->x_textbox->setReadOnly(false);
                ui->y_textbox->setReadOnly(false);
            } else {
                ui->key_browse_widget->setVisible(true);
                ui->a_textbox->setReadOnly(true);
                ui->b_textbox->setReadOnly(true);
                ui->p_textbox_2->setReadOnly(true);
                ui->x_textbox->setReadOnly(true);
                ui->y_textbox->setReadOnly(true);
                ui->private_textbox->setReadOnly(true);
                ui->public_key_textbox->setReadOnly(true);
                ui->x_textbox->setReadOnly(true);
                ui->y_textbox->setReadOnly(true);
            }
            break;
        }
    }
}


void ECCWidget::on_key_generate_source_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->key_browse_widget->setVisible(false);
            ui->a_textbox->setReadOnly(false);
            ui->b_textbox->setReadOnly(false);
            ui->p_textbox_2->setReadOnly(false);
            ui->x_textbox->setReadOnly(false);
            ui->y_textbox->setReadOnly(false);
            ui->private_textbox->setReadOnly(false);
            ui->public_key_textbox->setReadOnly(false);
            break;
        }
        case 1: {
            ui->key_browse_widget->setVisible(true);
            ui->a_textbox->setReadOnly(true);
            ui->b_textbox->setReadOnly(true);
            ui->p_textbox_2->setReadOnly(true);
            ui->x_textbox->setReadOnly(true);
            ui->y_textbox->setReadOnly(true);
            ui->private_textbox->setReadOnly(true);
            ui->public_key_textbox->setReadOnly(true);
            break;
        }
    }
}


void ECCWidget::on_generate_button_clicked()
{
    if (ecc != nullptr) {
        delete ecc;
    }
    ecc = new ECC();

    ecc->initECC(
        (char*) ui->a_textbox->text().toLatin1().toStdString().c_str(),
        (char*) ui->b_textbox->text().toLatin1().toStdString().c_str(),
        (char*) ui->p_textbox_2->text().toLatin1().toStdString().c_str()
    );
    ecc->initClient(
        (char*) ui->private_textbox->toPlainText().toLatin1().toStdString().c_str(),
        (char*) ui->k_textbox->text().toLatin1().toStdString().c_str()
    );

    char temp[4096];
    memset(temp, 0x0, 4096 * sizeof(char));

    ui->x_textbox->setText(tr(std::to_string(mpz_get_ui(*ecc->get_base_x())).c_str()));
    ui->y_textbox->setText(tr(std::to_string(mpz_get_ui(*ecc->get_base_y())).c_str()));

    std::ostringstream public_key_stream;
    mpz_get_str(temp, 10, *(ecc->get_public_key_x()));
    public_key_stream << temp << " ";
    mpz_get_str(temp, 10, *(ecc->get_public_key_y()));
    public_key_stream << temp;

    ui->public_textbox->setPlainText(tr(public_key_stream.str().c_str()));
}

void ECCWidget::on_private_browse_clicked()
{
    std::string n;
    std::string d;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);

        if (ecc == nullptr) {
            ecc = new ECC();
        }

        this->read_content_private(filename.toStdString());
    }

    delete dialog;
}


void ECCWidget::on_public_browse_clicked()
{
    std::string n;
    std::string e;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);

        this->read_content_public(filename.toStdString());
    }

    delete dialog;
}


void ECCWidget::on_private_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);
    }

    delete dialog;
}

void ECCWidget::on_public_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);
    }

    delete dialog;
}

void ECCWidget::on_save_key_clicked()
{
    if (ecc != nullptr) {
        ecc->dump_keys_to_file(ui->private_key_textbox->text().toLatin1().toStdString() + "/private.pem", ui->private_key_textbox->text().toLatin1().toStdString() + "/public.pem");

        QMessageBox message_box;
        message_box.information(this, tr("Info"), tr("Keys are saved as private.pem and public.pem"));
    }
}

void ECCWidget::on_encrypt_decrypt_select_currentIndexChanged(int index)
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


void ECCWidget::on_encrypt_decrypt_source_currentIndexChanged(int index)
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


void ECCWidget::on_browse_source_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->file_source->setText(filename);
    }

    delete dialog;
}

void ECCWidget::on_encrypt_button_clicked()
{
    if (ecc == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string plain_text = ui->plaint_text->toPlainText().toLatin1().toStdString();
        std::string cipher_text = ecc->encrypt(plain_text);

        ui->cipher_text->setPlainText(tr(cipher_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string cipher_text = ecc->encrypt(stream.str());

        std::ofstream file_stream_output(filename + ".enc");
        file_stream_output << cipher_text;
        file_stream_output.close();
    }
}


void ECCWidget::on_decrypt_button_clicked()
{
    if (ecc == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string cipher_text = ui->cipher_text->toPlainText().toLatin1().toStdString();
        std::string plain_text = ecc->decrypt(cipher_text);

        ui->plaint_text->setPlainText(tr(plain_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename, std::ios_base::in | std::ios_base::binary);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string plain_text= ecc->decrypt(stream.str());

        std::ofstream file_stream_output(filename + ".dec", std::ios_base::out | std::ios_base::binary);
        file_stream_output << plain_text;
        file_stream_output.close();
    }
}

