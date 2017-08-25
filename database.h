#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QString>
class DataBase
{
public:
    void ConnectDB();
    bool IfTitleExists(QString title);
private:
    QSqlDatabase db;

};

#endif // DATABASE_H
