/*
 * This file is part of DataPackage Sender
 *
 * Licensed GPL v2
 * http://DataPackageSender.com/
 *
 * Copyright Dan Nagle
 *
 */
#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QThread>
#include <QSslSocket>
#include "../DataPackage/packet.h"

class TCPThread : public QThread
{
        Q_OBJECT

    public:
        TCPThread(int socketDescriptor, QObject *parent);
        TCPThread(DataPackage sendDataPackage, QObject *parent);
        void sendAnother(DataPackage sendDataPackage);
        static void loadSSLCerts(QSslSocket *sock, bool allowSnakeOil);

        void run();
        bool sendFlag;
        bool incomingPersistent;
        bool closeRequest;
        bool isSecure;
        bool isEncrypted();

    signals:
        void error(QSslSocket::SocketError socketError);

        void DataPackageReceived(DataPackage sendDataPackage);
        void toStatusBar(const QString & message, int timeout = 0, bool override = false);
        void DataPackageSent(DataPackage sendDataPackage);
        void connectStatus(QString message);

    public slots:
        void sendPersistant(DataPackage sendDataPackage);

        void closeConnection();
    private slots:
        void wasdisconnected();

    private:
        int socketDescriptor;
        QString text;
        DataPackage sendDataPackage;
        void init();
        void writeResponse(QSslSocket *sock, DataPackage tcpDataPackage);
        QSslSocket * clientConnection;
        bool insidePersistent;

        void persistentConnectionLoop();
};

#endif // TCPTHREAD_H
