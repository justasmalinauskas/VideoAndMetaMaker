#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QFileInfo>
#include <QtGui>
#include <QListWidget>
#include <QPushButton>
#include <initializer_list>
#include <string>
#include <algorithm>
#include <iterator>
#include <QMessageBox>
#include <QCloseEvent>
#include <QtSql>
#include <QTableWidget>
#include "database.h"
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    m_TableHeader<<"Filename"<<"Filepath"<<"IsFoundinDB?"<<"Conversion status";
    ui->ConversionList->setRowCount(0);
    ui->ConversionList->setColumnCount(4);
    ui->ConversionList->setHorizontalHeaderLabels(m_TableHeader);
    ui->ConversionList->verticalHeader()->setVisible(false);
    ui->ConversionList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ConversionList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ConversionList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->ConversionList->setShowGrid(false);
    ui->ConversionList->setStyleSheet("QTableView {selection-background-color: red;}");
    ui->ConversionList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    db.ConnectDB();
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
    foreach (const QUrl &url, e->mimeData()->urls())
    {
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
            qDebug() << "It is video content";

            bool already = false;
            for (int i = 0; i < ConList.size(); i++)
            {
                if(ConList[i].fpath==filepath)
                {
                    qDebug() <<"duplicate";
                    already = true;
                }

            }
            if(!already)
            {
                AddItemConList(filename, filepath);
            }
        }
    }
}

void MainWindow::AddItemConList(QString filename, QString filepath)
{
    int rowpos = ui->ConversionList->rowCount();
    ui->ConversionList->insertRow(rowpos);
    //QTableWidgetItem *newItem = new QTableWidgetItem


    QString title, SE, EP;
    QRegularExpression ret("(?:(?!((_s\\d{2,}_e\\d{2,})|(\\.))).)*");
    QRegularExpressionMatch match1 = ret.match(filename);
    if (match1.hasMatch()) {
        title = match1.captured(0);
        qDebug() << "Title: " << title;
    }
    QRegularExpression res("(?:(\\d{2,})+(?=(_e\\d{2,})))");
    QRegularExpressionMatch match2 = res.match(filename);
    if (match2.hasMatch()) {
        SE = match2.captured(0);
        qDebug() << "Season: " << SE;
    }
    else
    {
        SE = "0";
    }
    QRegularExpression ree("(?:\\d\\d+(?=\\.))");
    QRegularExpressionMatch match3 = ree.match(filename);
    if (match3.hasMatch()) {
        EP = match3.captured(0);
        qDebug() << "Episode: " << EP;
    }
    else
    {
        EP = "0";
    }

    ConList.append(TitlesDataLive(title, filepath, SE, EP));


    ui->ConversionList->setItem(rowpos,0,new QTableWidgetItem(filename));
    ui->ConversionList->setItem(rowpos,1,new QTableWidgetItem(filepath));
    if(db.IfTitleExists(title))
    {
        ui->ConversionList->setItem(rowpos,2,new QTableWidgetItem("Yes"));
    }
    else
    {
        ui->ConversionList->setItem(rowpos,2,new QTableWidgetItem("No"));
    }
    ui->ConversionList->setItem(rowpos,3,new QTableWidgetItem("Not started"));
    qDebug() << "Succesful";
}

void MainWindow::on_ConvertButton_clicked(bool checked)
{
    ui->ConvertButton->setChecked(true);
    if (checked)
    {
        ui->ConvertButton->setText("Cancel");
    } else {

            ifStopConversion();

    }
}

void MainWindow::ifStopConversion()
{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Conversion Cancelation");
        msgBox.setText("Are you sure about that?");
        msgBox.setInformativeText("By doing that you will lose all your progress that you have been made");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Yes:
            //turns off conversion process
            ui->ConvertButton->setText("Convert");
            ui->ConvertButton->setChecked(false);
              break;
          case QMessageBox::No:
            //keeps conversion process going on
            ui->ConvertButton->setChecked(true);
              break;
          default:
              // should never be reached
              break;
        }
}

void MainWindow::closeEvent (QCloseEvent *event)// checking if whether conversion in on going or not when closing application if yes, asking if cancel consverion
{
    if (!ui->ConvertButton->isChecked()) {
        event->accept();
        //event->ignore();
    } else {
        ifStopConversion();
        if (!ui->ConvertButton->isChecked())
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }


    }
}
