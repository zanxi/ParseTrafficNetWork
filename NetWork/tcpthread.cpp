/*
 * This file is part of DataPackage Sender
 *
 * Licensed GPL v2
 * http://DataPackageSender.com/
 *
 * Copyright Dan Nagle
 *
 */

#include "tcpthread.h"
#include "../datasystems.h"
#include "../DataPackage/packet.h"
#include <QDataStream>
#include <QDebug>
#include <QSettings>
#include <QDesktopServices>
#include <QDir>

#include <QSsl>
#include <QSslKey>
#include <QSslSocket>
#include <QSslCipher>
#include <QSslConfiguration>
#include <QTemporaryFile>


TCPThread::TCPThread(int socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{

    init();
    sendFlag = false;
    incomingPersistent = false;
    sendDataPackage.clear();
    insidePersistent = false;
    isSecure = false;


}

TCPThread::TCPThread(DataPackage sendDataPackage, QObject *parent)
    : QThread(parent), sendDataPackage(sendDataPackage)
{
    sendFlag = true;
    incomingPersistent = false;
    insidePersistent = false;
    isSecure = false;
}

void TCPThread::sendAnother(DataPackage sendDataPackage)
{

    QDEBUG() << "Send another DataPackage to " << sendDataPackage.port;
    this->sendDataPackage = sendDataPackage;

}


void TCPThread::loadSSLCerts(QSslSocket * sock, bool allowSnakeOil)
{
    QSettings settings(DataSystems::Instance().SETTINGSFILE, QSettings::IniFormat);


    if (!allowSnakeOil) {

        // set the ca certificates from the configured path
        if (!settings.value("sslCaPath").toString().isEmpty()) {
            //sock-> setCaCertificates(QSslCertificate::fromPath(settings.value("sslCaPath").toString()));
        }

        // set the local certificates from the configured file path
        if (!settings.value("sslLocalCertificatePath").toString().isEmpty()) {
            sock->setLocalCertificate(settings.value("sslLocalCertificatePath").toString());
        }

        // set the private key from the configured file path
        if (!settings.value("sslPrivateKeyPath").toString().isEmpty()) {
            sock->setPrivateKey(settings.value("sslPrivateKeyPath").toString());
        }

    } else  {


        QString defaultCertFile = DataSystems::Instance().CERTFILE;
        QString defaultKeyFile = DataSystems::Instance().KEYFILE;
        QFile certfile(defaultCertFile);
        QFile keyfile(defaultKeyFile);

        /*
        #ifdef __APPLE__
                QString certfileS("/Users/dannagle/github/DataPackageSender/src/ps.pem");
                QString keyfileS("/Users/dannagle/github/DataPackageSender/src/ps.key");
        #else
                QString certfileS("C:/Users/danie/github/DataPackageSender/src/ps.pem");
                QString keyfileS("C:/Users/danie/github/DataPackageSender/src/ps.key");
        #endif

                defaultCertFile = certfileS;
                defaultKeyFile = keyfileS;
        */

        QDEBUG() << "Loading" << defaultCertFile << defaultKeyFile;

        certfile.open(QIODevice::ReadOnly);
        QSslCertificate certificate(&certfile, QSsl::Pem);
        certfile.close();
        if (certificate.isNull()) {
            QDEBUG() << "Bad cert. delete it?";
        }

        keyfile.open(QIODevice::ReadOnly);
        QSslKey sslKey(&keyfile, QSsl::Rsa, QSsl::Pem);
        keyfile.close();
        if (sslKey.isNull()) {
            QDEBUG() << "Bad key. delete it?";
        }


        sock->setLocalCertificate(certificate);
        sock->setPrivateKey(sslKey);

    }

}

void TCPThread::init()
{

}


void TCPThread::wasdisconnected()
{

    QDEBUG();
}

void TCPThread::writeResponse(QSslSocket *sock, DataPackage tcpDataPackage)
{

    QSettings settings(DataSystems::Instance().SETTINGSFILE, QSettings::IniFormat);
    bool sendResponse = settings.value("sendReponse", false).toBool();
    bool sendSmartResponse = settings.value("smartResponseEnableCheck", false).toBool();
    QList<SmartResponseConfig> smartList;
    smartList.clear();
    smartList.append(DataPackage::fetchSmartConfig(1, DataSystems::Instance().SETTINGSFILE));
    smartList.append(DataPackage::fetchSmartConfig(2, DataSystems::Instance().SETTINGSFILE));
    smartList.append(DataPackage::fetchSmartConfig(3, DataSystems::Instance().SETTINGSFILE));
    smartList.append(DataPackage::fetchSmartConfig(4, DataSystems::Instance().SETTINGSFILE));
    smartList.append(DataPackage::fetchSmartConfig(5, DataSystems::Instance().SETTINGSFILE));



    QString responseData = (settings.value("responseHex", "")).toString();
    int ipMode = settings.value("ipMode", 4).toInt();

    QByteArray smartData;
    smartData.clear();

    if (sendSmartResponse) {
        smartData = DataPackage::smartResponseMatch(smartList, tcpDataPackage.getByteArray());
    }


    if (sendResponse || !smartData.isEmpty()) {
        DataPackage tcpDataPackagereply;
        tcpDataPackagereply.timestamp = QDateTime::currentDateTime();
        tcpDataPackagereply.name = "Reply to " + tcpDataPackage.timestamp.toString(DataSystems::Instance().DATETIMEFORMAT);
        tcpDataPackagereply.tcpOrUdp = "TCP";
        if (sock->isEncrypted()) {
            tcpDataPackagereply.tcpOrUdp = "SSL";
        }
        tcpDataPackagereply.fromIP = "You (Response)";
        if (ipMode < 6) {
            tcpDataPackagereply.toIP = DataPackage::removeIPv6Mapping(sock->peerAddress());
        } else {
            tcpDataPackagereply.toIP = (sock->peerAddress()).toString();
        }
        tcpDataPackagereply.port = sock->peerPort();
        tcpDataPackagereply.fromPort = sock->localPort();
        QByteArray data = DataPackage::HEXtoByteArray(responseData);
        tcpDataPackagereply.hexString = DataPackage::byteArrayToHex(data);

        QString testMacro = DataPackage::macroSwap(tcpDataPackagereply.asciiString());
        tcpDataPackagereply.hexString = DataPackage::ASCIITohex(testMacro);

        if (!smartData.isEmpty()) {
            tcpDataPackagereply.hexString = DataPackage::byteArrayToHex(smartData);
        }
        sock->write(tcpDataPackagereply.getByteArray());
        sock->waitForBytesWritten(2000);
        QDEBUG() << "DataPackageSent " << tcpDataPackagereply.name << tcpDataPackagereply.hexString;
        emit DataPackageSent(tcpDataPackagereply);

    }

}


void TCPThread::persistentConnectionLoop()
{
    QDEBUG() << "Entering the forever loop";
    int ipMode = 4;
    QHostAddress theAddress(sendDataPackage.toIP);
    if (QAbstractSocket::IPv6Protocol == theAddress.protocol()) {
        ipMode = 6;
    }

    int count = 0;
    while (clientConnection->state() == QAbstractSocket::ConnectedState && !closeRequest) {
        insidePersistent = true;


        if (sendDataPackage.hexString.isEmpty() && sendDataPackage.persistent && (clientConnection->bytesAvailable() == 0)) {
            count++;
            if (count % 10 == 0) {
                //QDEBUG() << "Loop and wait." << count++ << clientConnection->state();
                emit connectStatus("Connected and idle.");
            }
            clientConnection->waitForReadyRead(200);
            continue;
        }

        if (clientConnection->state() != QAbstractSocket::ConnectedState && sendDataPackage.persistent) {
            QDEBUG() << "Connection broken.";
            emit connectStatus("Connection broken");

            break;
        }

        if (sendDataPackage.receiveBeforeSend) {
            QDEBUG() << "Wait for data before sending...";
            emit connectStatus("Waiting for data");
            clientConnection->waitForReadyRead(500);

            DataPackage tcpRCVDataPackage;
            tcpRCVDataPackage.hexString = DataPackage::byteArrayToHex(clientConnection->readAll());
            if (!tcpRCVDataPackage.hexString.trimmed().isEmpty()) {
                QDEBUG() << "Received: " << tcpRCVDataPackage.hexString;
                emit connectStatus("Received " + QString::number((tcpRCVDataPackage.hexString.size() / 3) + 1));

                tcpRCVDataPackage.timestamp = QDateTime::currentDateTime();
                tcpRCVDataPackage.name = QDateTime::currentDateTime().toString(DataSystems::Instance().DATETIMEFORMAT);
                tcpRCVDataPackage.tcpOrUdp = "TCP";
                if (clientConnection->isEncrypted()) {
                    tcpRCVDataPackage.tcpOrUdp = "SSL";
                }

                if (ipMode < 6) {
                    tcpRCVDataPackage.fromIP = DataPackage::removeIPv6Mapping(clientConnection->peerAddress());
                } else {
                    tcpRCVDataPackage.fromIP = (clientConnection->peerAddress()).toString();
                }


                QDEBUGVAR(tcpRCVDataPackage.fromIP);
                tcpRCVDataPackage.toIP = "You";
                tcpRCVDataPackage.port = sendDataPackage.fromPort;
                tcpRCVDataPackage.fromPort =    clientConnection->peerPort();
                if (tcpRCVDataPackage.hexString.size() > 0) {
                    emit DataPackageSent(tcpRCVDataPackage);
                }

            } else {
                QDEBUG() << "No pre-emptive receive data";
            }

        } // end receive before send


        //sendDataPackage.fromPort = clientConnection->localPort();
        emit connectStatus("Sending data:" + sendDataPackage.asciiString());
        QDEBUG() << "Attempting write data";
        clientConnection->write(sendDataPackage.getByteArray());
        emit DataPackageSent(sendDataPackage);

        DataPackage tcpDataPackage;
        tcpDataPackage.timestamp = QDateTime::currentDateTime();
        tcpDataPackage.name = QDateTime::currentDateTime().toString(DataSystems::Instance().DATETIMEFORMAT);
        tcpDataPackage.tcpOrUdp = "TCP";
        if (clientConnection->isEncrypted()) {
            tcpDataPackage.tcpOrUdp = "SSL";
        }

        if (ipMode < 6) {
            tcpDataPackage.fromIP = DataPackage::removeIPv6Mapping(clientConnection->peerAddress());

        } else {
            tcpDataPackage.fromIP = (clientConnection->peerAddress()).toString();

        }
        QDEBUGVAR(tcpDataPackage.fromIP);

        tcpDataPackage.toIP = "You";
        tcpDataPackage.port = sendDataPackage.fromPort;
        tcpDataPackage.fromPort =    clientConnection->peerPort();

        clientConnection->waitForReadyRead(500);
        emit connectStatus("Waiting to receive");
        tcpDataPackage.hexString.clear();

        while (clientConnection->bytesAvailable()) {
            tcpDataPackage.hexString.append(" ");
            tcpDataPackage.hexString.append(DataPackage::byteArrayToHex(clientConnection->readAll()));
            tcpDataPackage.hexString = tcpDataPackage.hexString.simplified();
            clientConnection->waitForReadyRead(100);
        }


        if (!sendDataPackage.persistent) {
            emit connectStatus("Disconnecting");
            clientConnection->disconnectFromHost();
        }

        QDEBUG() << "DataPackageSent " << tcpDataPackage.name << tcpDataPackage.hexString.size();

        if (sendDataPackage.receiveBeforeSend) {
            if (!tcpDataPackage.hexString.isEmpty()) {
                emit DataPackageSent(tcpDataPackage);
            }
        } else {
            emit DataPackageSent(tcpDataPackage);
        }



        emit connectStatus("Reading response");
        tcpDataPackage.hexString  = clientConnection->readAll();

        tcpDataPackage.timestamp = QDateTime::currentDateTime();
        tcpDataPackage.name = QDateTime::currentDateTime().toString(DataSystems::Instance().DATETIMEFORMAT);


        if (tcpDataPackage.hexString.size() > 0) {
            emit DataPackageSent(tcpDataPackage);
        }



        if (!sendDataPackage.persistent) {
            break;
        } else {
            sendDataPackage.clear();
            sendDataPackage.persistent = true;
            QDEBUG() << "Persistent connection. Loop and wait.";
            continue;
        }
    } // end while connected

    if (closeRequest) {
        clientConnection->close();
        clientConnection->waitForDisconnected(100);
    }

}


void TCPThread::closeConnection()
{
    QDEBUG() << "Closing connection";
    clientConnection->close();
}


void TCPThread::run()
{
    closeRequest = false;

    //determine IP mode based on send address.
    int ipMode = 4;
    QHostAddress theAddress(sendDataPackage.toIP);
    if (QAbstractSocket::IPv6Protocol == theAddress.protocol()) {
        ipMode = 6;
    }

    if (sendFlag) {
        QDEBUG() << "We are threaded sending!";
        clientConnection = new QSslSocket(this);

        sendDataPackage.fromIP = "You";
        sendDataPackage.timestamp = QDateTime::currentDateTime();
        sendDataPackage.name = sendDataPackage.timestamp.toString(DataSystems::Instance().DATETIMEFORMAT);
        bool portpass = false;

        portpass = clientConnection->bind(); //use random port.
        if (portpass) {
            sendDataPackage.fromPort = clientConnection->localPort();
        }

        // SSL Version...

        if (sendDataPackage.isSSL()) {
            QSettings settings(DataSystems::Instance().SETTINGSFILE, QSettings::IniFormat);

            loadSSLCerts(clientConnection, false);

            if (ipMode > 4) {
                clientConnection->connectToHostEncrypted(sendDataPackage.toIP,  sendDataPackage.port, QIODevice::ReadWrite, QAbstractSocket::IPv6Protocol);

            } else {
                clientConnection->connectToHostEncrypted(sendDataPackage.toIP,  sendDataPackage.port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

            }


            if (settings.value("ignoreSSLCheck", true).toBool()) {
                QDEBUG() << "Telling SSL to ignore errors";
                clientConnection->ignoreSslErrors();
            }


            QDEBUG() << "Connecting to" << sendDataPackage.toIP << ":" << sendDataPackage.port;
            QDEBUG() << "Wait for connected finished" << clientConnection->waitForConnected(5000);
            QDEBUG() << "Wait for encrypted finished" << clientConnection->waitForEncrypted(5000);

            QDEBUG() << "isEncrypted" << clientConnection->isEncrypted();

            const QList<QSslError> sslErrorsList; // = clientConnection->sslErrors();
            //clientConnection->sslErrors(sslErrorsList);


            DataPackage errorDataPackage = sendDataPackage;
            if (sslErrorsList.size() > 0) {
                QSslError sError;
                foreach (sError, sslErrorsList) {
                    DataPackage errorDataPackage = sendDataPackage;
                    errorDataPackage.hexString.clear();
                    errorDataPackage.errorString = sError.errorString();
                    emit DataPackageSent(errorDataPackage);
                }
            }

            if (clientConnection->isEncrypted()) {
                QSslCipher cipher = clientConnection->sessionCipher();
                DataPackage errorDataPackage = sendDataPackage;
                errorDataPackage.hexString.clear();
                errorDataPackage.errorString = "Encrypted with " + cipher.encryptionMethod();
                emit DataPackageSent(errorDataPackage);

                errorDataPackage.hexString.clear();
                errorDataPackage.errorString = "Authenticated with " + cipher.authenticationMethod();
                QDEBUGVAR(cipher.encryptionMethod());
                emit DataPackageSent(errorDataPackage);

                errorDataPackage.hexString.clear();
                errorDataPackage.errorString = "Peer Cert issued by " +  clientConnection->peerCertificate().issuerInfo(QSslCertificate::CommonName).join("\n");
                QDEBUGVAR(cipher.encryptionMethod());
                emit DataPackageSent(errorDataPackage);

                errorDataPackage.hexString.clear();
                errorDataPackage.errorString = "Our Cert issued by " +  clientConnection->localCertificate().issuerInfo(QSslCertificate::CommonName).join("\n");
                QDEBUGVAR(cipher.encryptionMethod());
                emit DataPackageSent(errorDataPackage);



            } else {
                DataPackage errorDataPackage = sendDataPackage;
                errorDataPackage.hexString.clear();
                errorDataPackage.errorString = "Not Encrypted!";
            }


        } else {


            if (ipMode > 4) {
                clientConnection->connectToHost(sendDataPackage.toIP,  sendDataPackage.port, QIODevice::ReadWrite, QAbstractSocket::IPv6Protocol);

            } else {
                clientConnection->connectToHost(sendDataPackage.toIP,  sendDataPackage.port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);

            }

            clientConnection->waitForConnected(5000);


        }


        if (sendDataPackage.delayAfterConnect > 0) {
            QDEBUG() << "sleeping " << sendDataPackage.delayAfterConnect;
            QObject().thread()->usleep(1000 * sendDataPackage.delayAfterConnect);
        }

        QDEBUGVAR(clientConnection->localPort());

        if (clientConnection->state() == QAbstractSocket::ConnectedState) {
            emit connectStatus("Connected");
            sendDataPackage.port = clientConnection->peerPort();
            sendDataPackage.fromPort = clientConnection->localPort();

            persistentConnectionLoop();

            emit connectStatus("Not connected.");
            QDEBUG() << "Not connected.";

        } else {


            //qintptr sock = clientConnection->socketDescriptor();

            //sendDataPackage.fromPort = clientConnection->localPort();
            emit connectStatus("Could not connect.");
            QDEBUG() << "Could not connect";
            sendDataPackage.errorString = "Could not connect";
            emit DataPackageSent(sendDataPackage);

        }

        QDEBUG() << "DataPackageSent " << sendDataPackage.name;
        if (clientConnection->state() == QAbstractSocket::ConnectedState) {
            clientConnection->disconnectFromHost();
            clientConnection->waitForDisconnected(1000);
            emit connectStatus("Disconnected.");

        }
        clientConnection->close();
        clientConnection->deleteLater();

        return;
    }


    QSslSocket sock;
    sock.setSocketDescriptor(socketDescriptor);

    //isSecure = true;

    if (isSecure) {

        QSettings settings(DataSystems::Instance().SETTINGSFILE, QSettings::IniFormat);


        //Do the SSL handshake
        QDEBUG() << "supportsSsl" << sock.supportsSsl();

        loadSSLCerts(&sock, settings.value("serverSnakeOilCheck", true).toBool());

        sock.setProtocol(QSsl::AnyProtocol);

        //suppress prompts
        bool envOk = false;
        const int env = qEnvironmentVariableIntValue("QT_SSL_USE_TEMPORARY_KEYCHAIN", &envOk);
        if ((env == 0)) {
            QDEBUG() << "Possible prompting in Mac";
        }

        if (settings.value("ignoreSSLCheck", true).toBool()) {
            sock.ignoreSslErrors();
        }
        sock.startServerEncryption();
        sock.waitForEncrypted();

        QList<QSslError> sslErrorsList;//  = sock.sslErrors();

        DataPackage errorDataPackage;
        errorDataPackage.init();
        errorDataPackage.timestamp = QDateTime::currentDateTime();
        errorDataPackage.name = errorDataPackage.timestamp.toString(DataSystems::Instance().DATETIMEFORMAT);
        errorDataPackage.toIP = "You";
        errorDataPackage.port = sock.localPort();
        errorDataPackage.fromPort = sock.peerPort();
        errorDataPackage.fromIP = sock.peerAddress().toString();

        if (sock.isEncrypted()) {
            errorDataPackage.tcpOrUdp = "SSL";
        }


        QDEBUGVAR(sock.isEncrypted());

        QDEBUGVAR(sslErrorsList.size());

        if (sslErrorsList.size() > 0) {

            QSslError sError;
            foreach (sError, sslErrorsList) {
                errorDataPackage.hexString.clear();
                errorDataPackage.errorString = sError.errorString();
                emit DataPackageSent(errorDataPackage);
            }

        }


        if (sock.isEncrypted()) {
            QSslCipher cipher = sock.sessionCipher();
            errorDataPackage.hexString.clear();
            errorDataPackage.errorString = "Encrypted with " + cipher.encryptionMethod();
            QDEBUGVAR(cipher.encryptionMethod());
            emit DataPackageSent(errorDataPackage);

            errorDataPackage.hexString.clear();
            errorDataPackage.errorString = "Authenticated with " + cipher.authenticationMethod();
            QDEBUGVAR(cipher.encryptionMethod());
            emit DataPackageSent(errorDataPackage);

            errorDataPackage.hexString.clear();
            errorDataPackage.errorString = "Peer cert issued by " +  sock.peerCertificate().issuerInfo(QSslCertificate::CommonName).join("\n");
            QDEBUGVAR(cipher.encryptionMethod());
            emit DataPackageSent(errorDataPackage);

            errorDataPackage.hexString.clear();
            errorDataPackage.errorString = "Our Cert issued by " +  sock.localCertificate().issuerInfo(QSslCertificate::CommonName).join("\n");
            QDEBUGVAR(cipher.encryptionMethod());
            emit DataPackageSent(errorDataPackage);


        }


       // QDEBUG() << "Errors" << sock.sslErrors();

    }

    connect(&sock, SIGNAL(disconnected()),
            this, SLOT(wasdisconnected()));

    //connect(&sock, SIGNAL(readyRead())

    DataPackage tcpDataPackage;
    QByteArray data;

    data.clear();
    tcpDataPackage.timestamp = QDateTime::currentDateTime();
    tcpDataPackage.name = tcpDataPackage.timestamp.toString(DataSystems::Instance().DATETIMEFORMAT);
    tcpDataPackage.tcpOrUdp = sendDataPackage.tcpOrUdp;

    if (ipMode < 6) {
        tcpDataPackage.fromIP = DataPackage::removeIPv6Mapping(sock.peerAddress());
    } else {
        tcpDataPackage.fromIP = (sock.peerAddress()).toString();
    }

    tcpDataPackage.toIP = "You";
    tcpDataPackage.port = sock.localPort();
    tcpDataPackage.fromPort = sock.peerPort();

    sock.waitForReadyRead(5000); //initial DataPackage
    data = sock.readAll();
    tcpDataPackage.hexString = DataPackage::byteArrayToHex(data);
    if (sock.isEncrypted()) {
        tcpDataPackage.tcpOrUdp = "SSL";
    }
    emit DataPackageSent(tcpDataPackage);
    writeResponse(&sock, tcpDataPackage);



    if (incomingPersistent) {
        clientConnection = &sock;
        QDEBUG() << "We are persistent incoming";
        sendDataPackage =  tcpDataPackage;
        sendDataPackage.persistent = true;
        sendDataPackage.hexString.clear();
        sendDataPackage.port = clientConnection->peerPort();
        sendDataPackage.fromPort = clientConnection->localPort();
        persistentConnectionLoop();
    }



    /*

        QDateTime twentyseconds = QDateTime::currentDateTime().addSecs(30);

        while ( sock.bytesAvailable() < 1 && twentyseconds > QDateTime::currentDateTime()) {
            sock.waitForReadyRead();
            data = sock.readAll();
            tcpDataPackage.hexString = DataPackage::byteArrayToHex(data);
            tcpDataPackage.timestamp = QDateTime::currentDateTime();
            tcpDataPackage.name = tcpDataPackage.timestamp.toString(DATETIMEFORMAT);
            emit DataPackageSent(tcpDataPackage);

            writeResponse(&sock, tcpDataPackage);
        }
    */
    insidePersistent = false;
    sock.disconnectFromHost();
    sock.close();
}

bool TCPThread::isEncrypted()
{
    if (insidePersistent && !closeRequest) {
        return clientConnection->isEncrypted();
    } else {
        return false;
    }
}

void TCPThread::sendPersistant(DataPackage sendDataPackage)
{
    if ((!sendDataPackage.hexString.isEmpty()) && (clientConnection->state() == QAbstractSocket::ConnectedState)) {
        QDEBUGVAR(sendDataPackage.hexString);
        clientConnection->write(sendDataPackage.getByteArray());
        sendDataPackage.fromIP = "You";

        QSettings settings(DataSystems::Instance().SETTINGSFILE, QSettings::IniFormat);
        int ipMode = settings.value("ipMode", 4).toInt();


        if (ipMode < 6) {
            sendDataPackage.toIP = DataPackage::removeIPv6Mapping(clientConnection->peerAddress());
        } else {
            sendDataPackage.toIP = (clientConnection->peerAddress()).toString();
        }

        sendDataPackage.port = clientConnection->peerPort();
        sendDataPackage.fromPort = clientConnection->localPort();
        if (clientConnection->isEncrypted()) {
            sendDataPackage.tcpOrUdp = "SSL";
        }
        emit DataPackageSent(sendDataPackage);
    }
}
