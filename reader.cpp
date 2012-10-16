#include "reader.h"
Reader::Reader(QString loc, QObject *parent) : QObject(parent)
{
    deviceLocation = loc;
    device = NULL;
    device = fopen(loc.toAscii(), "rb");

    if(device > 0)
    {
        QTimer::singleShot(200, this, SLOT(ready()));
        notifier = new QSocketNotifier(fileno(device), QSocketNotifier::Read, this);
        notifier->setEnabled(true);
        connect(notifier, SIGNAL(activated(int)), this, SLOT(readData()));
    }
    else
        QTimer::singleShot(200, this, SLOT(notReady()));
}

void Reader::readData()
{
    char buff;
    QString raw = "";
    for(int i = 0; i < 256; i++)
    {
        fread(&buff, 1, 1, device);
        if(buff != '\0')
            raw += QString(buff);
    }
    emit sendMessage(getCardID(raw));
}

QString Reader::getCardID(QString raw)
{
    QString formatted = "";
    int start = raw.indexOf(QRegExp(id_regexp));
    formatted = raw.mid(start+1, id_length);
    return formatted;
}

void Reader::ready()
{
    emit sendMessage("Device Ready");
}
void Reader::notReady()
{
    emit sendMessage("Device Not Ready");
}
