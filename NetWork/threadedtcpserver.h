#ifndef THREADEDTCPSERVER_H
#define THREADEDTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QList>

#include "tcpthread.h"
#include "persistentconnection.h"

class ThreadedTCPServer : public QTcpServer
{
        Q_OBJECT
    public:
        explicit ThreadedTCPServer(QObject *parent = nullptr);
        bool encrypted;

        bool init(quint16 port, bool isEncrypted, QString ipMode);
    protected:
        void incomingConnection(qintptr socketDescriptor);

    signals:
        void DataPackageReceived(DataPackage sendDataPackage);
        void toStatusBar(const QString & message, int timeout = 0, bool override = false);
        void DataPackageSent(DataPackage sendDataPackage);


    public slots:
        void DataPackageReceivedECHO(DataPackage sendDataPackage);
        void toStatusBarECHO(const QString & message, int timeout = 0, bool override = false);
        void DataPackageSentECHO(DataPackage sendDataPackage);



    private:
        QList<TCPThread *> threads;


        QList<TCPThread *> tcpthreadList;
        QList<PersistentConnection *> pcList;


};

#endif // THREADEDTCPSERVER_H
