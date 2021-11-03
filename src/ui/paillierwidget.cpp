#include <include/ui/paillierwidget.h>
#include <fstream>
#include <iostream>
#include "ui_paillierwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

PaillierWidget::PaillierWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PaillierWidget)
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

    ui->n_textbox->setReadOnly(true);
    ui->lambda_textbox->setReadOnly(true);
    ui->mu_textbox->setReadOnly(true);

    // Setup encrypt decrypt widget
    ui->encrypt_decrypt_select->addItem(tr("encrypt"));
    ui->encrypt_decrypt_select->addItem(tr("decrypt"));
    ui->encrypt_decrypt_source->addItem(tr("keyboard"));
    ui->encrypt_decrypt_source->addItem(tr("file"));

    ui->decrypt_button->setVisible(false);

    paillier = nullptr;
}

PaillierWidget::~PaillierWidget()
{
    delete ui;
    if (paillier != nullptr) {
        delete paillier;
    }
}

void PaillierWidget::read_content_private(std::string filename, std::string& lambda, std::string& mu)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "paillier") {
            throw std::ifstream::failure("Algorithm is not Paillier");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "private") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read lambda
        infile >> dummy >> token;
        std::string local_lambda = token;
        lambda = local_lambda;

        // Read mu
        infile >> dummy >> token;
        std::string local_mu = token;
        mu = local_mu;
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void PaillierWidget::read_content_public(std::string filename, std::string& g, std::string& n)
{
    std::ifstream infile(filename);

    std::string dummy;
    std::string token;
    try {
        // Read algorithm type
        infile >> dummy >> token;

        if (token != "paillier") {
            throw std::ifstream::failure("Algorithm is not Paillier");
        }

        // Read type
        infile >> dummy >> token;

        if (token != "public") {
            throw std::ifstream::failure("Key type must be 'public' or 'private'");
        }

        // Read g
        infile >> dummy >> token;
        std::string local_g = token;
        g = local_g;

        // Read n
        infile >> dummy >> token;
        std::string local_n = token;
        n = local_n;
    } catch (std::ifstream::failure &err) {
        QMessageBox message_box;
        message_box.critical(this, "Error", err.what());
    }

    infile.close();
}

void PaillierWidget::on_key_generate_select_currentIndexChanged(int index)
{
    paillier->set_g((char *) ui->g_textbox->text().toLatin1().toStdString().c_str());

    switch(index) {
        case 0: {
            ui->p_container->setVisible(true);
            ui->q_container->setVisible(true);
            ui->qg_spacer->setVisible(true);
            ui->from_text->setVisible(false);
            ui->key_generate_source->setVisible(false);
            ui->n_textbox->setReadOnly(true);
            ui->lambda_textbox->setReadOnly(true);
            ui->mu_textbox->setReadOnly(true);
            ui->key_browse_widget->setVisible(true);
            ui->private_browse->setVisible(false);
            ui->public_browse->setVisible(false);
            ui->private_save_button->setVisible(true);
            ui->public_save_button->setVisible(true);
            ui->save_key->setVisible(true);
            break;
        }
        case 1: {
            ui->p_container->setVisible(false);
            ui->q_container->setVisible(false);
            ui->qg_spacer->setVisible(false);
            ui->from_text->setVisible(true);
            ui->key_generate_source->setVisible(true);
            ui->n_textbox->setReadOnly(false);
            ui->lambda_textbox->setReadOnly(false);
            ui->mu_textbox->setReadOnly(false);

            ui->private_browse->setVisible(true);
            ui->public_browse->setVisible(true);
            ui->private_save_button->setVisible(false);
            ui->public_save_button->setVisible(false);
            if (ui->key_generate_source->currentIndex() == 0) {
                ui->key_browse_widget->setVisible(false);
            } else {
                ui->key_browse_widget->setVisible(true);
            }
            ui->save_key->setVisible(false);
            break;
        }
    }

    ui->n_textbox->setPlainText(QString(""));
    ui->lambda_textbox->setPlainText(QString(""));
    ui->mu_textbox->setPlainText(QString(""));
    ui->private_key_textbox->setText(tr(""));
    ui->public_key_textbox->setText(tr(""));
}


void PaillierWidget::on_key_generate_source_currentIndexChanged(int index)
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


void PaillierWidget::on_private_browse_clicked()
{
    std::string lambda;
    std::string mu;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);

        this->read_content_private(filename.toStdString(), lambda, mu);
        ui->lambda_textbox->setPlainText(tr(lambda.c_str()));
        ui->mu_textbox->setPlainText(tr(mu.c_str()));

        if (paillier == nullptr) {
            paillier = new Paillier();
        }
        paillier->set_lambda((char *) lambda.c_str());
        paillier->set_mu((char *) mu.c_str());
    }

    delete dialog;
}


void PaillierWidget::on_public_browse_clicked()
{
    std::string g;
    std::string n;

    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);

        this->read_content_public(filename.toStdString(), g, n);
        ui->g_textbox->setText(tr(g.c_str()));
        ui->n_textbox->setPlainText(tr(n.c_str()));

        if (paillier == nullptr) {
            paillier = new Paillier();
        }
        paillier->set_g((char *) g.c_str());
        paillier->set_n((char *) n.c_str());
    }

    delete dialog;
}


void PaillierWidget::on_private_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->private_key_textbox->setText(filename);
    }

    delete dialog;
}

void PaillierWidget::on_public_save_button_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getExistingDirectory(this, tr("Choose a directory"));
    if (!filename.isNull()) {
        ui->public_key_textbox->setText(filename);
    }

    delete dialog;
}

void PaillierWidget::on_generate_button_clicked()
{
    if (paillier != nullptr) {
        delete paillier;
    }
    paillier = new Paillier();

    paillier->set_p((char*) ui->p_textbox->text().toLatin1().toStdString().c_str());
    paillier->set_q((char*) ui->q_textbox->text().toLatin1().toStdString().c_str());
    paillier->set_g((char*) ui->g_textbox->text().toLatin1().toStdString().c_str());

    paillier->generate_keys();

    char temp[4096];
    mpz_get_str(temp, 10, (*paillier->get_n()));
    ui->n_textbox->setPlainText(tr(temp));

    mpz_get_str(temp, 10, (*paillier->get_lambda()));
    ui->lambda_textbox->setPlainText(tr(temp));

    mpz_get_str(temp, 10, (*paillier->get_mu()));
    ui->mu_textbox->setPlainText(tr(temp));
}


void PaillierWidget::on_save_key_clicked()
{
    if (paillier != nullptr) {
        paillier->set_g((char *) ui->g_textbox->text().toLatin1().toStdString().c_str());
        paillier->dump_keys_to_file(ui->private_key_textbox->text().toLatin1().toStdString() + "/private.pem", ui->private_key_textbox->text().toLatin1().toStdString() + "/public.pem");

        QMessageBox message_box;
        message_box.information(this, tr("Info"), tr("Keys are saved as private.pem and public.pem"));
    }
}

void PaillierWidget::on_encrypt_decrypt_select_currentIndexChanged(int index)
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
        ui->r_container->setVisible(true);
    } else {
        if (ui->encrypt_decrypt_source->currentIndex() == 0) {
            ui->cipher_text->setReadOnly(false);
        } else {
            ui->cipher_text->setReadOnly(true);
        }
        ui->plaint_text->setReadOnly(true);

        ui->decrypt_button->setVisible(true);
        ui->encrypt_button->setVisible(false);
        ui->r_container->setVisible(false);
    }
}


void PaillierWidget::on_encrypt_decrypt_source_currentIndexChanged(int index)
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


void PaillierWidget::on_browse_source_clicked()
{
    QFileDialog *dialog = new QFileDialog;

    QString filename = dialog->getOpenFileName();
    if (!filename.isNull()) {
        ui->file_source->setText(filename);
    }

    delete dialog;
}

void PaillierWidget::on_encrypt_button_clicked()
{
    paillier->set_g((char *) ui->g_textbox->text().toLatin1().toStdString().c_str());
    if (paillier == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    std::string r = ui->r_textbox->text().toLatin1().toStdString();

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string plain_text = ui->plaint_text->toPlainText().toLatin1().toStdString();
        std::string cipher_text = paillier->encrypt(plain_text, r);

        ui->cipher_text->setPlainText(tr(cipher_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string cipher_text = paillier->encrypt(stream.str(), r);

        std::ofstream file_stream_output(filename + ".enc");
        file_stream_output << cipher_text;
        file_stream_output.close();
    }
}


void PaillierWidget::on_decrypt_button_clicked()
{
    if (paillier == nullptr) {
        QMessageBox message_box;
        message_box.critical(this, tr("Error"), tr("Generate or import a key first"));
        return;
    }

    if (ui->encrypt_decrypt_source->currentIndex() == 0) {
        std::string cipher_text = ui->cipher_text->toPlainText().toLatin1().toStdString();
        std::string plain_text = paillier->decrypt(cipher_text);

        ui->plaint_text->setPlainText(tr(plain_text.c_str()));
    } else {
        std::string filename = ui->file_source->text().toLatin1().toStdString();
        std::ifstream file_stream(filename, std::ios_base::in | std::ios_base::binary);

        std::stringstream stream;
        stream << file_stream.rdbuf();

        file_stream.close();

        std::string plain_text= paillier->decrypt(stream.str());

        std::ofstream file_stream_output(filename + ".dec", std::ios_base::out | std::ios_base::binary);
        file_stream_output << plain_text;
        file_stream_output.close();
    }
}
