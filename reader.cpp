#include "reader.h"
Reader::Reader(QObject *parent) : QObject(parent)
{
    deviceLocation = lookForDevice();
    if(deviceLocation != "")
    {
        device = NULL;
        device = fopen(deviceLocation.toAscii(), "rb");

        fcntl(fileno(device), F_SETFL, O_NONBLOCK);

        if(fileno(device) > 0)
        {
            QTimer::singleShot(200, this, SLOT(ready()));

            notifier = new QSocketNotifier(fileno(device), QSocketNotifier::Read, this);
            notifier->setEnabled(true);
            connect(notifier, SIGNAL(activated(int)), this, SLOT(readData()));
        }
        else
            QTimer::singleShot(200, this, SLOT(notReady()));
    }
    else
        QTimer::singleShot(200, this, SLOT(notReady()));
}

void Reader::readData()
{
    char buff;
    QString raw = "";
    for(int i = 0; i < 1024; i++)
    {
        fread(&buff, 1, 1, device);
        if(buff != '\0')
            raw += QString(buff);
    }
    QString formatted = getCardID(raw);
    QString name      = getCardName(raw);
    if(checkCardID(formatted) && name != "")
    {
        emit sendMessage(name + " Swiped card: " + QString(QCryptographicHash::hash(formatted.toLocal8Bit(), QCryptographicHash::Sha1)));
    }
}

QString Reader::getCardID(QString raw)
{
    QString formatted = "";
    int start = raw.indexOf(QRegExp(id_regexp));
    formatted = raw.mid(start+1, id_length);
    return formatted;
}
bool Reader::checkCardID(QString id)
{
    if(id != "")
    {
        for(int i = 0; i < id.length(); i++)
        {
            if(!id.at(i).isDigit())
                return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}
QString Reader::getCardName(QString raw)
{
    QString formatted = "";
    QRegExp name_reg("%*^*;");
    name_reg.setPatternSyntax(QRegExp::Wildcard);
    if(raw.contains(name_reg))
    {
        formatted = raw.mid(raw.indexOf("%")+1 , raw.indexOf("^") - raw.indexOf("%") -1);
    }
    return formatted;
}

void Reader::ready()
{
    emit sendMessage("Device Ready");
    emit sendMessage("Connected card reader on: " + deviceLocation);
}
void Reader::notReady()
{
    emit sendMessage("Device Not Ready");
}

QString Reader::lookForDevice()
{
    initUdev();
    QString devLoc = "";

    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *path;

        /* Get the filename of the /sys entry for the device
               and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* The device pointed to by dev contains information about
               the hidraw device. In order to get information about the
               USB device, get the parent device with the
               subsystem/devtype pair of "usb"/"usb_device". This will
               be several levels up the tree, but the function will find
               it.*/
        dev = udev_device_get_parent_with_subsystem_devtype(dev,"usb", "usb_device");
        if (!dev)
        {
            // Couldn't get parent info
        }
        else
        {
            if(QString::compare(QString::fromStdString(udev_device_get_sysattr_value(dev,"manufacturer")), deviceManufacturer, Qt::CaseInsensitive) == 0)
            {
                devLoc = QString::fromStdString(path);
                break;
            }
        }
    }

    cleanUdev();

    if(devLoc != "")
    {
        QRegExp dev_hidraw_exp("*hidraw/hidraw*");
        dev_hidraw_exp.setPatternSyntax(QRegExp::Wildcard);
        return "/dev/"+devLoc.right(devLoc.indexOf(dev_hidraw_exp) + 7);
    }
    else
    {
        return "";
    }
}

void Reader::initUdev()
{
    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
        printf("Can't create udev\n");
        exit(1);
    }

    /* Create a list of the devices in the 'hidraw' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
}

void Reader::cleanUdev()
{
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}
