#include <include/ui/mainwindow.h>
#include <include/ui/rsawidget.h>
#include <include/ui/elgamalwidget.h>
#include <include/ui/paillierwidget.h>
#include <include/ui/eccwidget.h>
#include <iostream>
#include "./ui_mainwindow.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new QStandardItemModel;
    model->appendRow(new QStandardItem("RSA"));
    model->appendRow(new QStandardItem("ElGamal"));
    model->appendRow(new QStandardItem("Pailier"));
    model->appendRow(new QStandardItem("ECC"));

    widget = new RSAWidget(this);
    ui->cryptoSelect->setModel(model);
    ui->cryptoSelect->setMinimumWidth(125);
    ui->cryptoSelect->setMaximumWidth(125);
    ui->widgetContainerLayout->addWidget(widget);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete widget;
}


void MainWindow::on_cryptoSelect_clicked(const QModelIndex &index)
{
    ui->widgetContainerLayout->removeWidget(this->widget);
    delete widget;

    switch(index.row()) {
        case 0: {
            this->widget = new RSAWidget(this);
            break;
        }
        case 1: {
            this->widget = new ElGamalWidget(this);
            break;
        }
        case 2: {
            this->widget = new PaillierWidget(this);
            break;
        }
        case 3: {
            this->widget = new ECCWidget(this);
            break;
        }
    }

    ui->widgetContainerLayout->addWidget(this->widget);
}

