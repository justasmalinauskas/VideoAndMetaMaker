#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "titlesdatalive.h"
#include "database.h"
#include <QAbstractItemModel>
#include <QStringListModel>
#include <clientsdialog.h>
#include <titlesdialog.h>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void insertitemstolist();
    void removeitemsfromlist();
    void ifStopConversion();
    void AddItemConList(QString filename, QString filepath);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_ConvertButton_clicked(bool checked);
    void closeEvent (QCloseEvent *event);
    void on_addClient_clicked();
    void on_addTitle_clicked();

    void on_editTitle_clicked();

    void on_removeTitle_clicked();

    void on_editClient_clicked();
    void on_removeClient_clicked();
    void readyReadStandardOutput();
    void encodingFinished();
    void on_ClientsList_clicked();//(const QModelIndex &index);

private:
    TitlesDialog *addte;
    ClientsDialog *addcl;
    QAbstractItemModel *model;
    Ui::MainWindow *ui;
    QAction *insertAction;
    QAction *removeAction;
    QStringList m_TableHeader;
    void CreateModel();
    QList<TitlesDataLive> ConList;
    DataBase db;
    void AddItem(QString title, QString fpath, QString SE, QString EP, QString ifexist, QString status);
    void AddToScreen(QString filename, QString filepath, QString fexist, QString status);
    void updateClients();
    QStringListModel *clientsmodel;
    QStringListModel *titlesmodel;
    void UpdateTitles();
    void UpdateIfExists();
    void Conversion(QString path,QString filename);
    QString mOutputString;
    QProcess *process;
    QString conpath;
    QString convpath;
    void CreateXML();
};

#endif // MAINWINDOW_H
