#ifndef PERSISTENTCONNECTION_H
#define PERSISTENTCONNECTION_H

#include <QDialog>
#include <QTimer>
#include <QList>
#include <QStringList>

#include "../DataPackage/packet.h"
#include "tcpthread.h"

namespace Ui
{
class PersistentConnection;
}

class PersistentConnection : public QDialog
{
        Q_OBJECT

    public:
        explicit PersistentConnection(QWidget *parent = nullptr);
        ~PersistentConnection();
        DataPackage sendDataPackage;
        DataPackage reSendDataPackage;
        TCPThread *thread;

        void init();

        void initWithThread(TCPThread *thethread, quint16 portNum);
    signals:
        void persistentDataPackageSend(DataPackage sendDataPackage);
        void closeConnection();


    public slots:
        void refreshTimerTimeout();
        void aboutToClose();
        void statusReceiver(QString message);

        void DataPackageSentSlot(DataPackage pkt);
        void DataPackageReceivedSlot(DataPackage pkt);
        void socketDisconnected();

    private slots:
        void on_buttonBox_rejected();

        void on_asciiSendButton_clicked();

        void on_DataPackageComboBox_currentIndexChanged(const QString &arg1);

        void on_searchEdit_textEdited(const QString &arg1);

        void on_asciiCheck_clicked(bool checked);

        void on_rawCheck_clicked(bool checked);

        void on_LoadButton_clicked();

        void on_DataPackageComboBox_currentIndexChanged(int index);

        void on_clearButton_clicked();

        void on_sendFileButton_clicked();

        void on_clipboardButton_clicked();

        void on_appendCRcheck_clicked();

private:
        Ui::PersistentConnection *ui;
        QPushButton * stopResendingButton;
        QTimer refreshTimer;
        QList<DataPackage> trafficList;
        QDateTime startTime;
        bool wasConnected;
        bool stopTimer;
        bool useraw;
        bool suppressSlot;
        bool translateMacroSend;
        QStringList previousCommands;


        void loadComboBox();
        void loadTrafficView();
        void cancelResends();
};

#endif // PERSISTENTCONNECTION_H
