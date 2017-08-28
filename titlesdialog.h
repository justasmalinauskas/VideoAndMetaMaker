#ifndef TITLESDIALOG_H
#define TITLESDIALOG_H

#include <QDialog>
#include <database.h>

namespace Ui {
class TitlesDialog;
}

class TitlesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TitlesDialog(const QString Client, const QString Title, QWidget *parent = 0);
    ~TitlesDialog();

private slots:
    void on_cancel_clicked();

    void on_insertDB_clicked();

private:
    Ui::TitlesDialog *ui;
    QString client;
    QString title;
    DataBase db;
};

#endif // TITLESDIALOG_H
