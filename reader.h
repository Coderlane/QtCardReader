#ifndef READER_H
#define READER_H

#include <QObject>
#include <QSocketNotifier>
#include <QRegExp>
#include <QTimer>
#include <stdio.h>
#include <qstring.h>

const int id_length = 16;
const QString id_regexp = ";[0-9]{16,16}=";

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QString loc, QObject *parent = 0);

private:
    QString deviceLocation;

    FILE *device;
    QSocketNotifier *notifier;

    void send(QString);
    QString getCardID(QString raw);
    
signals:
    void sendMessage(QString mess);
    
public slots:
    void readData();
    void ready();
    void notReady();
    
};

#endif // READER_H
