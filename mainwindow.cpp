#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfo>
#include <QtGui>
#include <QListWidget>
#include <initializer_list>
#include <string>
#include <algorithm>
#include <iterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString filepath = url.toLocalFile();
        QFile f(filepath);
        QFileInfo fileInfo(f.fileName());
        QString filename(fileInfo.fileName());
        QString fileext = fileInfo.suffix();
        qDebug() << "Dropped file path:" << filepath;
        qDebug() << "Dropped filename:" << filename;
        qDebug() << "Dropped file extension:" << fileext;
        std::string goodexts[] = {"avi", "mp4", "flv", "mov", "mkv", "3gp", "mpg", "wmv", "webm", "vob", "ogv", "gifv", "qt", "rm", "m4v" "rmvb"};
        bool matchesext = std::find(std::begin(goodexts), std::end(goodexts), fileext.toStdString()) != std::end(goodexts);
        if(matchesext)
        {
            qDebug() << "It worked";
            if(!ConList.contains(fpath))
            {
                qDebug() << "2xIt worked";
                ConListMem mem = ConListMem();
                mem.fname = filename;
                mem.fpath = filepath;
                ConList << mem;
                //QListWidgetItem* item = mem.fname;
            }

        }


    }
}
