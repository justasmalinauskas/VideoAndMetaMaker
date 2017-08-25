#include "database.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QFileInfo>
#include <QDebug>

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
            QSqlQuery query;
            query.exec("CREATE TABLE IF NOT EXISTS Clients (id INTEGER PRIMARY KEY UNIQUE NOT NULL, ClientName STRING NOT NULL, Website STRING)");
            query.exec("CREATE TABLE IF NOT EXISTS Titles (TitleID INTEGER PRIMARY KEY UNIQUE NOT NULL, ClientID INTEGER REFERENCES Clients (id), TitleName STRING UNIQUE NOT NULL)");
        }
        else
            qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
}

bool DataBase::IfTitleExists(QString title)
{
    bool exist;
    ConnectDB();
    QSqlQuery q;
    exist = q.exec("SELECT 1 FROM Titles WHERE TitleName='"+title+"'");
    return exist;
}
