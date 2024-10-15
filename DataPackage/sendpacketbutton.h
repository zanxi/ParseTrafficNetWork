/*
 * This file is part of DataPackage Sender
 *
 * Licensed GPL v2
 * http://DataPackageSender.com/
 *
 * Copyright Dan Nagle
 *
 */
#ifndef SENDDataPackageBUTTON_H
#define SENDDataPackageBUTTON_H

#include <QPushButton>
#include "datasystems.h"
class SendDataPackageButton : public QPushButton
{
        Q_OBJECT
    public:
        explicit SendDataPackageButton(QWidget *parent = 0);
        QString name;

        void init();
    signals:
        void sendDataPackage(QString name);

    public slots:
        void sendClicked();

};

#endif // SENDDataPackageBUTTON_H
