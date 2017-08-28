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
#include <QCoreApplication>
#include <QXmlStreamWriter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    m_TableHeader<<"Filename"<<"Filepath"<<"Is Found in DB?"<<"Conversion status";
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
    updateClients();
    process = new QProcess(this);
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(process, SIGNAL(finished(int)), this, SLOT(encodingFinished()));
    CreateXML();
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
                {DataBase db;
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
        SE = "";
    }
    QRegularExpression ree("(?:\\d\\d+(?=\\.))");
    QRegularExpressionMatch match3 = ree.match(filename);
    if (match3.hasMatch()) {
        EP = match3.captured(0);
        qDebug() << "Episode: " << EP;
    }
    else
    {
        EP = "";
    }
    QString fexist = db.IfTitleExists(title);
    QString statusbegin = "Not started";
    ConList.append(TitlesDataLive(title, filepath, SE, EP, fexist, statusbegin));
    AddToScreen(filename, filepath, fexist, statusbegin);
}

void MainWindow::AddToScreen(QString filename, QString filepath, QString fexist, QString status)
{
    int rowpos = ui->ConversionList->rowCount();
    ui->ConversionList->insertRow(rowpos);
    ui->ConversionList->setItem(rowpos,0,new QTableWidgetItem(filename));
    ui->ConversionList->setItem(rowpos,1,new QTableWidgetItem(filepath));
    ui->ConversionList->setItem(rowpos,2,new QTableWidgetItem(fexist));
    ui->ConversionList->setItem(rowpos,3,new QTableWidgetItem(status));
    qDebug() << "Succesful";
}

void MainWindow::on_ConvertButton_clicked(bool checked)
{
    ui->ConvertButton->setChecked(true);
    if (checked)
    {
        ui->ConvertButton->setText("Cancel");
        for (int i = 0; i < ui->ConversionList->rowCount(); i++)
        {
            Conversion(ui->ConversionList->item(i, 1)->text(),ui->ConversionList->item(i, 0)->text());
            ui->ConversionList->item(i, 3)->setText("Conversion started");
            UpdateIfExists();
        }

    }
    else
    {
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
            //needs to kill conversion(ffmpeg) process
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

void MainWindow::on_addClient_clicked()
{
    addcl = new ClientsDialog(NULL);
    addcl->exec();
    updateClients();
    UpdateIfExists();
}


void MainWindow::on_addTitle_clicked()
{
    addte = new TitlesDialog(NULL, NULL);
    addte->exec();
    UpdateTitles();
    UpdateIfExists();
}

void MainWindow::on_editTitle_clicked()
{
    QModelIndexList templatelist = ui->ClientsList->selectionModel()->selectedIndexes();
    QStringList stringlist;
    foreach (const QModelIndex index, templatelist){
    stringlist.append(index.data(Qt::DisplayRole).toString());
    }
    QModelIndexList templatelistt = ui->TitlesList->selectionModel()->selectedIndexes();
    QStringList stringlistt;
    foreach (const QModelIndex index, templatelistt){
    stringlistt.append(index.data(Qt::DisplayRole).toString());
    }
    addte = new TitlesDialog(stringlist.join(","), stringlistt.join(","));
    addte->exec();
    UpdateTitles();
    UpdateIfExists();
}

void MainWindow::on_removeTitle_clicked()
{
    QModelIndexList templatelist = ui->TitlesList->selectionModel()->selectedIndexes();
    QStringList stringlist;
    foreach (const QModelIndex index, templatelist){
    stringlist.append(index.data(Qt::DisplayRole).toString());
    }
    db.RemoveTitle(stringlist.join(","));
    UpdateTitles();
    UpdateIfExists();
}

void MainWindow::on_editClient_clicked()
{
    QModelIndexList templatelist = ui->ClientsList->selectionModel()->selectedIndexes();
    QStringList stringlist;
    foreach (const QModelIndex index, templatelist){
    stringlist.append(index.data(Qt::DisplayRole).toString());
    }
    addcl = new ClientsDialog(stringlist.join(","));
    addcl->exec();
    updateClients();
    UpdateIfExists();
}

void MainWindow::on_removeClient_clicked()
{
    QModelIndexList templatelist = ui->ClientsList->selectionModel()->selectedIndexes();
    QStringList stringlist;
    foreach (const QModelIndex index, templatelist){
    stringlist.append(index.data(Qt::DisplayRole).toString());
    }
    db.RemoveClient(stringlist.join(","));
    updateClients();
    UpdateTitles();
    UpdateIfExists();
}

void MainWindow::updateClients()
{
    QList<QString> clientslist = db.GetClients();
    qDebug() << clientslist;
    clientsmodel = new QStringListModel(clientslist, parent());//= new QStringListModel(clientslist, parent);
    ui->ClientsList->setModel(clientsmodel);
    ui->ClientsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    UpdateIfExists();
}

void MainWindow::on_ClientsList_clicked()
{
    UpdateTitles();
    UpdateIfExists();
}

void MainWindow::UpdateTitles()
{
    QModelIndexList templatelist = ui->ClientsList->selectionModel()->selectedIndexes();
    QStringList stringlist;
    foreach (const QModelIndex index, templatelist){
    stringlist.append(index.data(Qt::DisplayRole).toString());
    }
    //qDebug() << stringlist.join(",");
    QList<QString> titleslist = db.GetTitles(stringlist.join(","));
    //titlesmodel = new QStringListModel(titleslist, parent);
    titlesmodel = new QStringListModel(titleslist, parent());
    ui->TitlesList->setModel(titlesmodel);
    ui->TitlesList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    UpdateIfExists();
}

void MainWindow::UpdateIfExists()
{
    for (int i = 0; i < ui->ConversionList->rowCount(); i++)
    {
        ui->ConversionList->item(i, 2)->setText(db.IfTitleExists(ConList[i].title));
        qDebug() << ConList[i].title;
    }
}

void MainWindow::Conversion(QString path, QString filename)
{

    qDebug() << "Conversion";
    QString ffmpegpath=qApp->applicationDirPath()+"/ffmpeg";
    std::string title = (filename.toStdString()).std::string::substr(0, (filename.toStdString()).std::string::find("."));
    QString contitle = QString::fromStdString(title);
    QStringList args;
    args << "-i" << path;
    args << "-c:v" << "libx264";
    args << "-crf" << "21";
    args << "-preset" << "fast";
    args << "-s" << "768x432";
    args << "-pix_fmt" << "yuv420p";
    args << "-c:a" << "aac";
    args << "-b:a" << "128k";
    args << "-ac" << "2";
    args << "-f" << "flv";
    args << qApp->applicationDirPath()+"/videos/"+contitle+".flv";
    conpath = path;
    convpath = qApp->applicationDirPath()+"/videos/"+contitle+".flv";
    QString Client = db.GetClient();
    //CreateXML(title, );
    //process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->start(ffmpegpath, args);
    process->waitForStarted();
    //process->close();
    //qDebug() << process->readAllStandardOutput();
    qDebug() << "ffmpegpath: " << ffmpegpath;
    qDebug() << "contitle: " << contitle;
    qDebug() << "ARGS: " << args;

}
void MainWindow::CreateXML(QString FileName, QString client, QString title, QString season, QString episode)
{
    QFile *xmlFile = new QFile(qApp->applicationDirPath()+"/"+FileName+".xml");
            if (!xmlFile->open(QIODevice::ReadWrite | QIODevice::Text)) {
                    QMessageBox::critical(this,"Load XML File Problem",
                    "Couldn't open xmlfile.xml to load settings for download",
                    QMessageBox::Ok);
                    return;
            }
      QXmlStreamWriter stream(xmlFile);
      stream.setAutoFormatting(true);
      stream.writeStartDocument();
      stream.writeStartElement("ClientData");
      stream.writeAttribute("Client", client);
      stream.writeTextElement("Filename", FileName);
      stream.writeTextElement("Title", title);
      stream.writeTextElement("Season", season);
      stream.writeTextElement("Episode", episode);
      stream.writeEndElement();
      stream.writeEndDocument();
      xmlFile->close();
}

void MainWindow::encodingFinished()
{
    int rows = ui->ConversionList->rowCount();
    qDebug() << convpath;
    for(int i = 0; i < rows; ++i)
    {
        qDebug() << "loop found";
        if(ui->ConversionList->item(i, 1)->text() == conpath)
        {
            qDebug() << "var found";
            if(QFile::exists(convpath))
            {
                qDebug() << "file found";
                ui->ConversionList->item(i, 3)->setText("Conversion sucessful");
            }
            else
            {
                ui->ConversionList->item(i, 3)->setText("Conversion failed");
            }
        }
    }
}

void MainWindow::readyReadStandardOutput()
{
    mOutputString.append(process->readAllStandardOutput());
    qDebug() << mOutputString << "/n";
}
