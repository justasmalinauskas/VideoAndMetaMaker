#include "clientsdialog.h"
#include "ui_clientsdialog.h"
#include <QDebug>

ClientsDialog::ClientsDialog(const QString Client, QWidget *parent) :
    QDialog(parent),client(Client),
    ui(new Ui::ClientsDialog)
{
    ui->setupUi(this);
    //db.ConnectDB();
    if (client != NULL)
    {
        qDebug() << "Testing: "<< Client;
        QString data;
        data = db.GetClientWebsite(client);
        ui->clName->setText(client);
        ui->clUrl->setText(data);
    }
}

ClientsDialog::~ClientsDialog()
{
    delete ui;
}

void ClientsDialog::on_insertDB_clicked()
{
    if (client == NULL)
        db.AddClient(ui->clName->text(),ui->clUrl->text());
    else
        db.UpdateClient(client, ui->clName->text(), ui->clUrl->text());
    this->setResult(QDialog::Accepted);
    this->close();
}

void ClientsDialog::on_cancel_clicked()
{
    this->setResult(QDialog::Rejected);
    this->close();
}
