#ifndef TITLESDATALIVE_H
#define TITLESDATALIVE_H
#include <QString>

class TitlesDataLive
{
public:
    TitlesDataLive(QString title, QString fpath, QString SE, QString EP)
    {
        TitlesDataLive::title = title;
        TitlesDataLive::fpath = fpath;
        TitlesDataLive::SE = SE;
        TitlesDataLive::EP = EP;
    }
    QString title;
    QString fpath;
    QString SE;
    QString EP;
};

#endif // TITLESDATALIVE_H
