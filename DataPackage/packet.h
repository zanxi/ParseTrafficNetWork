/*
 * This file is part of DataPackage Sender
 *
 * Licensed GPL v2
 * http://DataPackageSender.com/
 *
 * Copyright Dan Nagle
 *
 */
#ifndef DataPackage_H
#define DataPackage_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QNetworkDatagram>
#include <QHash>
#include "datasystems.h"
#include "DataPackage/sendpacketbutton.h"


struct SmartResponseConfig {
    int id;
    QString ifEquals;
    QString replyWith;
    QString encoding;
    bool enabled;
};

class DataPackage
{
    public:
        DataPackage()
        {
            init();
        }
        ~DataPackage();

        DataPackage(const DataPackage &other);

        static QHostAddress IPV4_IPV6_ANY(QString ipMode);

        QString name;
        QString hexString;
        QString requestPath;
        QString fromIP;
        QString toIP;
        QString resolvedIP;
        QString errorString;
        float repeat;
        unsigned int port;
        unsigned int fromPort;
        QString tcpOrUdp;
        unsigned int sendResponse;
        bool incoming;
        void init();
        void clear();
        bool isTCP();
        bool isSSL();
        bool isUDP();
        bool isHTTP();
        bool isHTTPS();
        bool isPOST();

        QDateTime timestamp;

        bool receiveBeforeSend;
        int delayAfterConnect;
        bool persistent;

        static QString ASCIITohex(QString &ascii);
        static QString hexToASCII(QString &hex);
        static QString byteArrayToHex(QByteArray data);
        static QByteArray HEXtoByteArray(QString thehex);
        static QString removeIPv6Mapping(QHostAddress ipv6);
        QByteArray getByteArray();
        QString asciiString();

        void saveToDB();


        static DataPackage fetchFromDB(QString thename);
        static QList<DataPackage> fetchAllfromDB(QString importFile);
        static bool removeFromDB(QString thename);
        static void populateTableWidgetItem(QTableWidgetItem *tItem, DataPackage theDataPackage);
        static DataPackage fetchTableWidgetItemData(QTableWidgetItem *tItem);
        static SmartResponseConfig fetchSmartConfig(int num, QString importFile);
        static QByteArray smartResponseMatch(QList<SmartResponseConfig> smartList, QByteArray data);
        static QByteArray encodingToByteArray(QString encoding, QString data);

        static const int DataPackage_NAME;
        static const int DataPackage_HEX;
        static const int FROM_IP;
        static const int FROM_PORT;
        static const int TCP_UDP;
        static const int TO_PORT;
        static const int TO_IP;

        static const int TIMESTAMP;
        static const int DATATYPE;
        static const int REPEAT;
        static const int INCOMING;
        static const int REQUEST_URL;


        bool operator()(const DataPackage* a, const DataPackage* b) const;

        SendDataPackageButton * getSendButton(QTableWidget *parent);
        QIcon getIcon();
        static void sortByName(QList<DataPackage> &DataPackageList);
        static void sortByTime(QList<DataPackage> &DataPackageList);
        static float oneDecimal(float value);
        static QString macroSwap(QString data);
        static QByteArray ExportJSON(QList<DataPackage> DataPackageList);
        static QList<DataPackage> ImportJSON(QByteArray data);
        void static removeFromDBList(QStringList nameList);
        static void setBoldItem(QTableWidgetItem *tItem, DataPackage theDataPackage);
        static DataPackage fetchFromList(QString thename, QList<DataPackage> DataPackages);
private:
        static int hexToInt(QChar hex);
};

Q_DECLARE_METATYPE(DataPackage)

#endif // DataPackage_H
