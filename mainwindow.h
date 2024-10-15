#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

#include "basewindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public BaseWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //virtual void keyPressEvent(QKeyEvent * event);
    //virtual void keyReleaseEvent(QKeyEvent * event);
private:
    Ui::MainWindow *ui;

    void ThisStyle(QString color_h);

};
#endif // MAINWINDOW_H
