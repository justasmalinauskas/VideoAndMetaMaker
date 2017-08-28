#ifndef TITLESDATALIVE_H
#define TITLESDATALIVE_H
#include <QString>

class TitlesDataLive
{
public:
    TitlesDataLive(QString title, QString fpath, QString SE, QString EP, QString ifexist, QString status)
    {
        TitlesDataLive::title = title;
        TitlesDataLive::fpath = fpath;
        TitlesDataLive::SE = SE;
        TitlesDataLive::EP = EP;
        TitlesDataLive::ifexist = ifexist;
        TitlesDataLive::status = status;

    }
    QString title;
    QString fpath;
    QString SE;
    QString EP;
    QString ifexist;
    QString status;

};

#endif // TITLESDATALIVE_H
