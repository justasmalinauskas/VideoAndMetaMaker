#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "titlesdatalive.h"
#include "database.h"

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

private:

    Ui::MainWindow *ui;
    QAction *insertAction;
    QAction *removeAction;
    QStringList m_TableHeader;

    QList<TitlesDataLive> ConList;
    DataBase db;
};

#endif // MAINWINDOW_H
