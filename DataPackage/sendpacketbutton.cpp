/*
 * This file is part of DataPackage Sender
 *
 * Licensed GPL v2
 * http://DataPackageSender.com/
 *
 * Copyright Dan Nagle
 *
 */

#include <QDebug>
#include "sendpacketbutton.h"

void themeTheButton(QPushButton * button);

SendDataPackageButton::SendDataPackageButton(QWidget *parent) :
    QPushButton(parent)
{

}

void SendDataPackageButton::init()
{
    //QDEBUG() << " sendButton connect attempt:" <<
    connect(this, SIGNAL(clicked()), this, SLOT(sendClicked()));
}


void SendDataPackageButton::sendClicked()
{

    QDEBUG() << " Emit clicked: " << name;

    emit sendDataPackage(name);

}


//used by mainwindow.cpp and panelgenerator.cpp
void themeTheButton(QPushButton * button)
{
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, QColor("#F5F5F5"));
    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->setStyleSheet("QPushButton { color: white; } QPushButton::hover { color: #BC810C; } ");
    button->setFlat(true);
    button->setCursor(Qt::PointingHandCursor);
    button->update();


}
