#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QString>
class DataBase
{
public:
    void ConnectDB();
    QString IfTitleExists(QString title);
    void AddClient(QString name, QString url);
    QList<QString> GetClients();
    QList<QString> GetTitles(QString clientname);
    QString GetClientWebsite(QString clientname);
    void UpdateClient(QString oldclientname, QString newclientname, QString newurl);
    void RemoveClient(QString clientname);
    void AddTitle(QString client, QString title);
    void UpdateTitle(QString oldclient, QString client, QString title, QString oldtitle);
    void RemoveTitle(QString title);
private:
    QSqlDatabase db;

};

#endif // DATABASE_H
