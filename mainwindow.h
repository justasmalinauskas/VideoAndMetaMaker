#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    struct ConListMem { QString fname; QString fpath;};
    QList<ConListMem> ConList;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    Ui::MainWindow *ui;
    QAction *insertAction;
    QAction *removeAction;
};

#endif // MAINWINDOW_H
