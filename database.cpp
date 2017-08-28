#include "database.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QFileInfo>
#include <QDebug>
#include <QList>
#include <QMessageBox>

void DataBase::ConnectDB()
{
    const QString DRIVER("QSQLITE");
    const QString DBPATH("database.db");
        if(QSqlDatabase::isDriverAvailable(DRIVER))
        {
            db = QSqlDatabase::addDatabase(DRIVER);
            db.setDatabaseName(DBPATH);

            if(!db.open())
                qWarning() << "MainWindow::DatabaseConnect - ERROR: " << db.lastError().text();
            QSqlQuery q;
            q.exec("CREATE TABLE IF NOT EXISTS Clients (id INTEGER PRIMARY KEY AUTOINCREMENT, ClientName STRING NOT NULL, Website STRING)");
            q.exec("CREATE TABLE IF NOT EXISTS Titles (TitleID INTEGER PRIMARY KEY AUTOINCREMENT, ClientID INTEGER REFERENCES Clients (id), TitleName STRING UNIQUE NOT NULL)");
        }
        else
            qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
}

QString DataBase::IfTitleExists(QString title)
{
    QSqlQuery q;
    q.prepare("SELECT TitleName FROM titles WHERE TitleName=\""+title+"\"");
    qDebug() << q.result();
    if (q.exec())
    {
       if (q.next())
       {
        return "Yes";
       }
    }
    if(q.lastError().text()!=" ")
    {
        qDebug() << "IfTitleExists SqLite error:" << q.lastError().text();
    }
    return "No";

}

QList<QString> DataBase::GetClients()
{
    qDebug() << "im getting clients";
    QList<QString> list;
    QSqlQuery q;
    q.exec("SELECT ClientName FROM clients");
    qDebug() << q.result();
    while (q.next()) {
            qDebug() << q.value(0).toString();
            QString res = q.value(0).toString();
            list.append(res);
        }
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
    return list;

}

QString DataBase::GetClientWebsite(QString clientname)
{
    QSqlQuery q;
    q.exec("SELECT Website FROM clients WHERE ClientName=\""+clientname+"\"");
    QString url;
    while (q.next()) {
            url = q.value(0).toString();
            qDebug() << url;
        }
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
    return url;
}

QList<QString> DataBase::GetTitles(QString clientname)
{
    qDebug() << "im getting titles from client: " << clientname;
    QList<QString> list;
    QSqlQuery q;
    q.exec("SELECT TitleName FROM titles INNER JOIN clients on clients.id = titles.ClientID WHERE clients.ClientName=\""+clientname+"\"");
    qDebug() << q.result();
    while (q.next()) {
            qDebug() << q.value(0).toString();
            QString res = q.value(0).toString();
            list.append(res);
        }
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
    return list;

}

void DataBase::AddClient(QString name, QString url)
{
    QSqlQuery q;
    q.prepare("SELECT ClientName FROM clients WHERE ClientName = (:clname)");
    q.bindValue(":clname",name);

    if (q.exec())
    {
       if (q.next())
       {
           QMessageBox msgBox;
           msgBox.setWindowTitle("Warning!");
           msgBox.setText("Client already exists!");
           msgBox.setStandardButtons(QMessageBox::Ok);
           msgBox.setDefaultButton(QMessageBox::Ok);
           msgBox.exec();
           qDebug() << "Client already exists";
       }
       else
       {
           q.prepare("INSERT INTO clients (ClientName, Website) VALUES (:clname, :clurl)");
           q.bindValue(":clname",name);
           q.bindValue(":clurl",url);
           q.exec();
       }
    }
    if(q.lastError().text()!=" ")
    {
        qDebug() << "AddClient SqLite error:" << q.lastError().text();
    }
}

void DataBase::UpdateClient(QString oldclientname, QString newclientname, QString newurl)
{
    QSqlQuery q;
    q.prepare("UPDATE clients SET ClientName=:clname, Website=:clurl WHERE ClientName=\""+oldclientname+"\"");
    q.bindValue(":clname",newclientname);
    q.bindValue(":clurl", newurl);
    q.exec();
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
}

void DataBase::RemoveClient(QString clientname)
{
    QSqlQuery q;
    q.exec("DELETE FROM titles INNER JOIN clients on clients.id = titles.ClientID WHERE clients.ClientName=\""+clientname+"\"");
    q.exec("DELETE FROM clients WHERE ClientName=\""+clientname+"\"");
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
}

void DataBase::RemoveTitle(QString title)
{
    QSqlQuery q;
    q.exec("DELETE FROM titles WHERE TitleName=\""+title+"\"");
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
}

void DataBase::AddTitle(QString client, QString title)
{
    QSqlQuery q;
   // q.prepare("SELECT TitleName FROM titles WHERE TitleName = (:title) AND ClientName= (:client) AND ClientID = clients.id ");
    q.prepare("SELECT TitleName FROM titles INNER JOIN clients ON titles.ClientID=clients.ID WHERE titles.TitleName=\""+title+"\" AND clients.ClientName=\""+client+"\"");
    //q.bindValue(":title", title);
    //q.bindValue(":client", client);

    if (q.exec())
    {
       if (q.next())
       {
           QMessageBox msgBox;
           msgBox.setWindowTitle("Warning!");
           msgBox.setText("Title already exists!");
           msgBox.setStandardButtons(QMessageBox::Ok);
           msgBox.setDefaultButton(QMessageBox::Ok);
           msgBox.exec();
           qDebug() << "Title already exists";
       }
       else
       {
           q.prepare("INSERT INTO titles (TitleName, ClientID) VALUES (:titlename, (SELECT id FROM clients WHERE ClientName=\""+client+"\"))");
           //INSERT INTO titles (TitleName, ClientID) VALUES ("Testas", (SELECT  id FROM clients WHERE ClientName="ass"));
           q.bindValue(":titlename",title);
           q.exec();
       }
    }
    if(q.lastError().text()!=" ")
    {
        qDebug() << "AddTitle SqLite error:" << q.lastError().text();
    }
}

void DataBase::UpdateTitle(QString oldclient, QString client, QString title, QString oldtitle)
{
    QSqlQuery q;
    //q.prepare("UPDATE clients SET ClientName=:clname, Website=:clurl WHERE ClientName=\""+oldclientname+"\"");
    q.prepare("UPDATE titles SET TitleName=:title WHERE TitleName=:oldtitle");
    q.bindValue(":title",title);
    q.bindValue(":oldtitle",oldtitle);
    q.exec();
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
    q.prepare("UPDATE clients SET ClientName=:client WHERE ClientName=:oldclient");
    q.bindValue(":client",client);
    q.bindValue(":oldclient",oldclient);
    q.exec();
    if(q.lastError().text()!=" ")
    {
        qDebug() << "SqLite error:" << q.lastError().text();
    }
}

