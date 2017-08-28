#ifndef CLIENTSDIALOG_H
#define CLIENTSDIALOG_H

#include <QDialog>
#include <database.h>

namespace Ui {
class ClientsDialog;
}

class ClientsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientsDialog(const QString Client, QWidget *parent = 0);
    ~ClientsDialog();

private slots:
    void on_insertDB_clicked();

    void on_cancel_clicked();

private:
    QString client;
    Ui::ClientsDialog *ui;

    DataBase db;
};

#endif // CLIENTSDIALOG_H
