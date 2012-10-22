#ifndef READER_H
#define READER_H

#include <QObject>
#include <QSocketNotifier>
#include <QRegExp>
#include <QTimer>
#include <QCryptographicHash>

#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

const int           id_length = 15;
const QString       id_regexp = ";[0-9]{16,16}=";
const QString       name_regexp = "^%*/*\\^*?$";
const QString       deviceManufacturer = "Mag-Tek";


class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QObject *parent = 0);

private:
    QString deviceLocation;

    FILE *device;
    QSocketNotifier *notifier;

    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    void send(QString);
    QString getCardID(QString raw);
    bool checkCardID(QString id);
    QString getCardName(QString raw);
    void initUdev();
    void cleanUdev();
    
signals:
    void sendMessage(QString mess);
    
public slots:
    void readData();
    void ready();
    void notReady();
    

    QString lookForDevice();
};

#endif // READER_H
