#include <include/ui/elgamalwidget.h>
#include <fstream>
#include <iostream>
#include "ui_elgamalwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

ElGamalWidget::ElGamalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ElGamalWidget)
{
    ui->setupUi(this);

    // Setup key generate widget
    ui->key_generate_select->addItem(QString("Generate"));
    ui->key_generate_select->addItem(QString("Import"));
    ui->key_generate_source->addItem(QString("Keyboard"));
    ui->key_generate_source->addItem(QString("File"));

    ui->from_text->setVisible(false);
    ui->key_generate_source->setVisible(false);
    ui->private_browse->setVisible(false);
    ui->public_browse->setVisible(false);
    ui->private_save_button->setVisible(true);
    ui->public_save_button->setVisible(true);
    ui->key_browse_widget->setVisible(true);
    ui->save_to_file_container->setVisible(true);

    ui->y_textbox->setReadOnly(true);
    ui->g_textbox->setPlaceholderText(tr("g < p"));
    ui->x_textbox->setPlaceholderText(tr("1 <= x <= p-2"));

    // Setup encrypt decrypt widget
    ui->encrypt_decrypt_select->addItem(tr("encrypt"));
    ui->encrypt_decrypt_select->addItem(tr("decrypt"));
    ui->encrypt_decrypt_source->addItem(tr("keyboard"));
    ui->encrypt_decrypt_source->addItem(tr("file"));

    ui->encrypt_decrypt_file_container->setVisible(false);
    ui->decrypt_button->setVisible(false);

    elgamal = nullptr;
}

ElGamalWidget::~ElGamalWidget()
{
    delete ui;
    if (elgamal != nullptr) {
        delete elgamal;
    }
}

void ElGamalWidget::read_content_private(std::string filename, std::string& p, std::string& x)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "elgamal") {
            throw std::ifstream::failure("Algorithm is not ElGamal");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "private") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read x
        infile >> dummy >> token;
        std::string local_x = token;
        x = local_x;

        // Read p
        infile >> dummy >> token;
        std::string local_p = token;
        p = local_p;
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void ElGamalWidget::read_content_public(std::string filename, std::string& p, std::string& g, std::string& y)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "elgamal") {
            std::cout << "OK" << std::endl;
            throw std::ifstream::failure("Algorithm is not ElGamal");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "public") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read y
        infile >> dummy >> token;
        std::string local_y = token;
        y = local_y;

        // Read g
        infile >> dummy >> token;
        std::string local_g = token;
        g = local_g;

        // Read g
        infile >> dummy >> token;
        std::string local_p = token;
        p = local_p;
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void ElGamalWidget::on_key_generate_select_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->generate_button->setVisible(true);
            ui->from_text->setVisible(false);
            ui->key_generate_source->setVisible(false);
            ui->y_textbox->setReadOnly(true);
            ui->key_browse_widget->setVisible(true);
            ui->private_browse->setVisible(false);
            ui->public_browse->setVisible(false);
            ui->private_save_button->setVisible(true);
            ui->public_save_button->setVisible(true);
            ui->save_to_file_container->setVisible(true);
            break;
        }
        case 1: {
            ui->generate_button->setVisible(false);
            ui->from_text->setVisible(true);
            ui->key_generate_source->setVisible(true);
            ui->y_textbox->setReadOnly(false);

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

    ui->g_textbox->setPlainText(QString(""));
    ui->x_textbox->setPlainText(QString(""));
    ui->y_textbox->setPlainText(QString(""));
    ui->p_textbox->setText(QString(""));
    ui->private_key_textbox->setText(tr(""));
    ui->public_key_textbox->setText(tr(""));
}


void ElGamalWidget::on_key_generate_source_currentIndexChanged(int index)
{
    switch(index) {
        case 0: {
            ui->key_browse_widget->setVisible(false);
            break;
        }
        case 1: {
            ui->key_browse_widget->setVisible(true);
            break;
        }
    }
}


void ElGamalWidget::on_private_browse_clicked()
{
    std::string p;
    std::string x;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);

        this->read_content_private(filename.toStdString(), p, x);
        ui->p_textbox->setText(tr(p.c_str()));
        ui->x_textbox->setPlainText(tr(x.c_str()));

        if (elgamal == nullptr) {
            elgamal = new ElGamal();
        }
        elgamal->set_p((char *) p.c_str());
        elgamal->set_x((char *) x.c_str());
    }

    delete dialog;
}


void ElGamalWidget::on_public_browse_clicked()
{
    std::string p;
    std::string g;
    std::string y;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);

        this->read_content_public(filename.toStdString(), p, g, y);
        ui->p_textbox->setText(tr(p.c_str()));
        ui->g_textbox->setPlainText(tr(g.c_str()));
        ui->y_textbox->setPlainText(tr(y.c_str()));

        if (elgamal == nullptr) {
            elgamal = new ElGamal();
        }
        elgamal->set_p((char *) p.c_str());
        elgamal->set_g((char *) g.c_str());
        elgamal->set_y((char *) y.c_str());
    }

    delete dialog;
}


void ElGamalWidget::on_private_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);
    }

    delete dialog;
}

void ElGamalWidget::on_public_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);
    }

    delete dialog;
}

void ElGamalWidget::on_generate_button_clicked()
{
    if (elgamal != nullptr) {
        delete elgamal;
    }
    elgamal = new ElGamal();

    elgamal->set_p((char*) ui->p_textbox->text().toLatin1().toStdString().c_str());
    elgamal->set_g((char*) ui->g_textbox->toPlainText().toLatin1().toStdString().c_str());
    elgamal->set_x((char*) ui->x_textbox->toPlainText().toLatin1().toStdString().c_str());

    elgamal->generate_keys();

    char temp[4096];
    mpz_get_str(temp, 10, (*elgamal->get_y()));
    ui->y_textbox->setPlainText(tr(temp));
}


void ElGamalWidget::on_save_key_clicked()
{
    if (elgamal != nullptr) {
        elgamal->dump_keys_to_file(ui->private_key_textbox->text().toLatin1().toStdString() + "/private.pem", ui->private_key_textbox->text().toLatin1().toStdString() + "/public.pem");

        QMessageBox message_box;
        message_box.information(this, tr("Info"), tr("Keys are saved as private.pem and public.pem"));
    }
}

void ElGamalWidget::on_encrypt_decrypt_select_currentIndexChanged(int index)
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


void ElGamalWidget::on_encrypt_decrypt_source_currentIndexChanged(int index)
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


void ElGamalWidget::on_browse_source_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->file_source->setText(filename);
    }

    delete dialog;
}

void ElGamalWidget::on_encrypt_button_clicked()
{
    if (elgamal == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string plain_text = ui->plaint_text->toPlainText().toLatin1().toStdString();
        std::string cipher_text = elgamal->encrypt(plain_text);

        ui->cipher_text->setPlainText(tr(cipher_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string cipher_text = elgamal->encrypt(stream.str());

        std::ofstream file_stream_output(filename + ".enc");
        file_stream_output << cipher_text;
        file_stream_output.close();
    }
}


void ElGamalWidget::on_decrypt_button_clicked()
{
    if (elgamal == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string cipher_text = ui->cipher_text->toPlainText().toLatin1().toStdString();
        std::string plain_text = elgamal->decrypt(cipher_text);

        ui->plaint_text->setPlainText(tr(plain_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename, std::ios_base::in | std::ios_base::binary);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string plain_text= elgamal->decrypt(stream.str());

        std::ofstream file_stream_output(filename + ".dec", std::ios_base::out | std::ios_base::binary);
        file_stream_output << plain_text;
        file_stream_output.close();
    }
}

