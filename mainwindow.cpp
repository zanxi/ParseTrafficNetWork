#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "datasystems.h"

#include <QPushButton>

//#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : BaseWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //***************** Инициализация установок приложения по умолчанию в классе BaseWindow ****************************//

    setWindowTitle(DataSystems::Instance().TitleMainWindow);
    QRect r = QGuiApplication::primaryScreen()->geometry();
    setGeometry(QRect(100, 100, 100+r.width()*0.8, 100+r.height()*0.5));    

    ThisStyle(DataSystems::Instance().settings___color_header);

    }

MainWindow::~MainWindow()
{
    delete ui;
}

// *******************************************

void MainWindow::ThisStyle(QString color_h)
{

    QList<QPushButton*> allButton = findChildren<QPushButton*>();
    for(int i=0;i<allButton.size();i++)
    {
        allButton[i]->setStyleSheet(
            "background-color:"+color_h+";"
                                            "color: white;"
                                            "padding: 4 50 4 10;");
    }

    QList<QGroupBox*> allGroup = findChildren<QGroupBox*>();
    for(int i=0;i<allGroup.size();i++)
    {
        allGroup[i]->setStyleSheet(
            "QGroupBox {"
            "background-color: white;"
            "}"
            "QGroupBox::title {"
            "color: white;"
            "background-color:"+color_h+";"
                        "padding: 4 20000 4 10;"
                        "}"
            );
    }

    //    ui->tableWidget_list_cows->setStyleSheet("QTableView"
    //                               "{"
    //                               "   background-color:rgb"+DataSystems::Instance().settings___color_header___decimal+";"
    //                                                                                             "}"

    //                                                                                             "QTabWidget:tab-bar"
    //                                                                                             "{"
    //                                                                                             "    alignment: center;"
    //                                                                                             "}"

    //                                                                                             "QTabBar:tab"
    //                                                                                             "{"
    //                                                                                             "   width: "+DataSystems::Instance().settings___tabwidget_width+";"
    //                                                                                      "   height: 30px;"
    //                                                                                      "}"

    //                                                                                      "QTabBar:selected"
    //                                                                                      "{"
    //                                                                                      "    background-color:rgb"+DataSystems::Instance().settings___color_header___decimal+";"
    //                                                                                             "    color.rgb"+DataSystems::Instance().settings___color_header___decimal+";"
    //                                                                                             "}"

    //                                                                                             "QTabBar:tab:!selected"
    //                                                                                             "{"
    //                                                                                             "    color.rgb"+DataSystems::Instance().settings___color_header___decimal+";"
    //                                                                                             "}"


    //                                                                                             "QTabBar:tab:!selected:hover"
    //                                                                                             "{"
    //                                                                                             "    background-color:rgb"+DataSystems::Instance().settings___color_header___decimal+";"
    //                                                                                             "    color.rgb(255,255,255);"
    //                                                                                             "}"
    //                                                                                             ")");


}
