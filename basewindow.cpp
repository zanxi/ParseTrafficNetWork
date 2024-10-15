#include "basewindow.h"

#include "debug/logger.h"
#include "datasystems.h"
#include "./json/json_w.h"

#include <memory>
#include <QScopedPointer>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>

BaseWindow::BaseWindow(QWidget *parent)
    : QMainWindow(parent)
{

    //***************** Инициализация ****************************//

    //***************** Установка белого цвета приложения и размеров ****************************//
    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, Qt::white);
    this->setPalette(pal);
    //setGeometry(QRect(200, 100, 1600, 800));

    QRect r = QGuiApplication::primaryScreen()->geometry();
    //this->resize(r.width(), r.height());
    setGeometry(QRect(100, 100, 100+r.width()*0.8, 100+r.height()*0.5));
    //***************** End Установка белого цвета приложения и размеров ****************************//

    // !!!!!!!!!!!!!!!! ВНИМАНИЕ - здесь важная часть установок проекта !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //***************** Осчистка логов отладки и дефолтная установка значений параметров системы (БД, наименований, таблиц) ****************************//
    logger::ClearLog();
    DataSystems::Instance().clear();
    //***************** End Осчистка логов отладки и дефолтная установка значений параметров системы (БД, наименований, таблиц) ****************************//

}

// *******************************************

BaseWindow::~BaseWindow()
{

}

void BaseWindow::keyPressEvent(QKeyEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Enter) {
        //qDebug() << ("keyPressEvent: Enter received");
    }
    else if (keyEvent->key() == Qt::Key_A)
    {
        //qDebug() << ("keyPressEvent: A received");
    }
}

void BaseWindow::keyReleaseEvent(QKeyEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Escape)
    {
        //qDebug() << ("keyReleaseEvent: Escape received");
        QMessageBox::information(this,"Спасибо","Вы вышли из программы <<<"+
                   DataSystems::Instance().TitleMainWindow+">>>");
        QApplication::closeAllWindows();
        QApplication::quit();
    }
}



