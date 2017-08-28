#include "titlesdialog.h"
#include "ui_titlesdialog.h"
#include <QDebug>

TitlesDialog::TitlesDialog(const QString Client, const QString Title, QWidget *parent) :
    QDialog(parent),client(Client),title(Title),
    ui(new Ui::TitlesDialog)
{
    ui->setupUi(this);
    //db.ConnectDB();
    ui->clientCombo->addItems(db.GetClients());
    ui->clientCombo->setCurrentIndex(ui->clientCombo->findData(Client, Qt::DisplayRole));
    if (title != NULL)
    {
        qDebug() << "Testing: "<< Client << " " << Title;
        ui->titleinput->setText(Title);
    }
}

TitlesDialog::~TitlesDialog()
{
    delete ui;
}

void TitlesDialog::on_cancel_clicked()
{
    this->setResult(QDialog::Rejected);
    this->close();
}

void TitlesDialog::on_insertDB_clicked()
{
    if (title == NULL)
        db.AddTitle(ui->clientCombo->itemText(ui->clientCombo->currentIndex()), ui->titleinput->text());
    else
        db.UpdateTitle(client, ui->clientCombo->itemText(ui->clientCombo->currentIndex()), ui->titleinput->text(), title);
    this->setResult(QDialog::Accepted);
    this->close();
}
