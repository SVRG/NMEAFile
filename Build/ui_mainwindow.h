/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_File;
    QAction *actionScale;
    QAction *actionNAV_Param;
    QAction *actionShow_Legend;
    QAction *actionErrors;
    QAction *actionRMC;
    QAction *actionScale_Y;
    QAction *actionGGA_Position;
    QAction *actionBSS_Distance;
    QAction *actionGGA_Diff_Age;
    QAction *actionY_Logarithmic;
    QAction *actionBLS;
    QAction *actionBSS_Total_Valid;
    QAction *actionHDT_Course;
    QAction *actionBLS_Course_Difference;
    QAction *actionScale_XY;
    QAction *actionFind_Errors;
    QAction *actionOpen_File_2;
    QAction *actionGGA_Position_1_2;
    QAction *actionGGA_Time_Check;
    QAction *actionGGA_Altitude;
    QAction *actionGGA_Position_Difference_12;
    QAction *actionGGA_900_sec_Diff_1_2;
    QAction *actionCreate_CSV;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_Graph;
    QVBoxLayout *verticalLayout_4;
    QCustomPlot *customPlot;
    QWidget *tab_Stat;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton;
    QLabel *label;
    QLineEdit *referenceValue;
    QTextBrowser *textBrowser;
    QMenuBar *menuBar;
    QMenu *menuNMEA;
    QMenu *menuFile;
    QMenu *menuView;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1028, 525);
        actionOpen_File = new QAction(MainWindow);
        actionOpen_File->setObjectName(QStringLiteral("actionOpen_File"));
        actionScale = new QAction(MainWindow);
        actionScale->setObjectName(QStringLiteral("actionScale"));
        actionNAV_Param = new QAction(MainWindow);
        actionNAV_Param->setObjectName(QStringLiteral("actionNAV_Param"));
        actionShow_Legend = new QAction(MainWindow);
        actionShow_Legend->setObjectName(QStringLiteral("actionShow_Legend"));
        actionErrors = new QAction(MainWindow);
        actionErrors->setObjectName(QStringLiteral("actionErrors"));
        actionRMC = new QAction(MainWindow);
        actionRMC->setObjectName(QStringLiteral("actionRMC"));
        actionScale_Y = new QAction(MainWindow);
        actionScale_Y->setObjectName(QStringLiteral("actionScale_Y"));
        actionGGA_Position = new QAction(MainWindow);
        actionGGA_Position->setObjectName(QStringLiteral("actionGGA_Position"));
        actionGGA_Position->setCheckable(true);
        actionBSS_Distance = new QAction(MainWindow);
        actionBSS_Distance->setObjectName(QStringLiteral("actionBSS_Distance"));
        actionGGA_Diff_Age = new QAction(MainWindow);
        actionGGA_Diff_Age->setObjectName(QStringLiteral("actionGGA_Diff_Age"));
        actionY_Logarithmic = new QAction(MainWindow);
        actionY_Logarithmic->setObjectName(QStringLiteral("actionY_Logarithmic"));
        actionBLS = new QAction(MainWindow);
        actionBLS->setObjectName(QStringLiteral("actionBLS"));
        actionBSS_Total_Valid = new QAction(MainWindow);
        actionBSS_Total_Valid->setObjectName(QStringLiteral("actionBSS_Total_Valid"));
        actionHDT_Course = new QAction(MainWindow);
        actionHDT_Course->setObjectName(QStringLiteral("actionHDT_Course"));
        actionBLS_Course_Difference = new QAction(MainWindow);
        actionBLS_Course_Difference->setObjectName(QStringLiteral("actionBLS_Course_Difference"));
        actionScale_XY = new QAction(MainWindow);
        actionScale_XY->setObjectName(QStringLiteral("actionScale_XY"));
        actionFind_Errors = new QAction(MainWindow);
        actionFind_Errors->setObjectName(QStringLiteral("actionFind_Errors"));
        actionOpen_File_2 = new QAction(MainWindow);
        actionOpen_File_2->setObjectName(QStringLiteral("actionOpen_File_2"));
        actionGGA_Position_1_2 = new QAction(MainWindow);
        actionGGA_Position_1_2->setObjectName(QStringLiteral("actionGGA_Position_1_2"));
        actionGGA_Time_Check = new QAction(MainWindow);
        actionGGA_Time_Check->setObjectName(QStringLiteral("actionGGA_Time_Check"));
        actionGGA_Altitude = new QAction(MainWindow);
        actionGGA_Altitude->setObjectName(QStringLiteral("actionGGA_Altitude"));
        actionGGA_Position_Difference_12 = new QAction(MainWindow);
        actionGGA_Position_Difference_12->setObjectName(QStringLiteral("actionGGA_Position_Difference_12"));
        actionGGA_900_sec_Diff_1_2 = new QAction(MainWindow);
        actionGGA_900_sec_Diff_1_2->setObjectName(QStringLiteral("actionGGA_900_sec_Diff_1_2"));
        actionCreate_CSV = new QAction(MainWindow);
        actionCreate_CSV->setObjectName(QStringLiteral("actionCreate_CSV"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setMinimumSize(QSize(0, 469));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_Graph = new QWidget();
        tab_Graph->setObjectName(QStringLiteral("tab_Graph"));
        verticalLayout_4 = new QVBoxLayout(tab_Graph);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        customPlot = new QCustomPlot(tab_Graph);
        customPlot->setObjectName(QStringLiteral("customPlot"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(customPlot->sizePolicy().hasHeightForWidth());
        customPlot->setSizePolicy(sizePolicy);
        customPlot->setMinimumSize(QSize(0, 200));

        verticalLayout_4->addWidget(customPlot);

        tabWidget->addTab(tab_Graph, QString());
        tab_Stat = new QWidget();
        tab_Stat->setObjectName(QStringLiteral("tab_Stat"));
        horizontalLayout_2 = new QHBoxLayout(tab_Stat);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        groupBox = new QGroupBox(tab_Stat);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout_3 = new QHBoxLayout(groupBox);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_3->addWidget(pushButton);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_3->addWidget(label);

        referenceValue = new QLineEdit(groupBox);
        referenceValue->setObjectName(QStringLiteral("referenceValue"));

        horizontalLayout_3->addWidget(referenceValue);


        horizontalLayout_2->addWidget(groupBox);

        textBrowser = new QTextBrowser(tab_Stat);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy1);
        textBrowser->setMinimumSize(QSize(0, 100));
        textBrowser->setMaximumSize(QSize(16777215, 16777207));
        textBrowser->setReadOnly(true);

        horizontalLayout_2->addWidget(textBrowser);

        tabWidget->addTab(tab_Stat, QString());

        horizontalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1028, 22));
        menuNMEA = new QMenu(menuBar);
        menuNMEA->setObjectName(QStringLiteral("menuNMEA"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        mainToolBar->setEnabled(false);
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuNMEA->menuAction());
        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuFile->addAction(actionOpen_File);
        menuFile->addAction(actionOpen_File_2);
        menuFile->addAction(actionCreate_CSV);
        menuView->addAction(actionScale);
        menuView->addAction(actionScale_Y);
        menuView->addAction(actionScale_XY);
        menuView->addAction(actionY_Logarithmic);
        menuView->addAction(actionShow_Legend);
        menuView->addSeparator();
        menuView->addAction(actionGGA_Position);
        menuView->addAction(actionNAV_Param);
        menuView->addAction(actionErrors);
        menuView->addAction(actionGGA_Diff_Age);
        menuView->addAction(actionGGA_Time_Check);
        menuView->addAction(actionGGA_Altitude);
        menuView->addSeparator();
        menuView->addAction(actionGGA_Position_1_2);
        menuView->addAction(actionGGA_Position_Difference_12);
        menuView->addAction(actionGGA_900_sec_Diff_1_2);
        menuView->addSeparator();
        menuView->addAction(actionRMC);
        menuView->addAction(actionBSS_Distance);
        menuView->addAction(actionBSS_Total_Valid);
        menuView->addSeparator();
        menuView->addAction(actionBLS);
        menuView->addAction(actionBLS_Course_Difference);
        menuView->addAction(actionHDT_Course);
        menuView->addAction(actionFind_Errors);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "NMEA File Viewer", 0));
        actionOpen_File->setText(QApplication::translate("MainWindow", "Open File", 0));
        actionScale->setText(QApplication::translate("MainWindow", "Scale X", 0));
        actionNAV_Param->setText(QApplication::translate("MainWindow", "GGA Sattelites in use", 0));
        actionShow_Legend->setText(QApplication::translate("MainWindow", "Show Legend", 0));
        actionErrors->setText(QApplication::translate("MainWindow", "GGA Position Difference", 0));
        actionRMC->setText(QApplication::translate("MainWindow", "RMC Speed & Course", 0));
        actionScale_Y->setText(QApplication::translate("MainWindow", "Scale Y", 0));
        actionGGA_Position->setText(QApplication::translate("MainWindow", "GGA Position", 0));
        actionBSS_Distance->setText(QApplication::translate("MainWindow", "BSS Distance", 0));
        actionGGA_Diff_Age->setText(QApplication::translate("MainWindow", "GGA Diff Age", 0));
        actionY_Logarithmic->setText(QApplication::translate("MainWindow", "Y Logarithmic", 0));
        actionBLS->setText(QApplication::translate("MainWindow", "BLS Course", 0));
        actionBSS_Total_Valid->setText(QApplication::translate("MainWindow", "BSS Total & Valid", 0));
        actionHDT_Course->setText(QApplication::translate("MainWindow", "HDT Course", 0));
        actionBLS_Course_Difference->setText(QApplication::translate("MainWindow", "BLS Course Difference", 0));
        actionScale_XY->setText(QApplication::translate("MainWindow", "Scale XY", 0));
        actionFind_Errors->setText(QApplication::translate("MainWindow", "Find Errors", 0));
        actionOpen_File_2->setText(QApplication::translate("MainWindow", "Open File 2", 0));
        actionGGA_Position_1_2->setText(QApplication::translate("MainWindow", "GGA Position 1 2", 0));
        actionGGA_Time_Check->setText(QApplication::translate("MainWindow", "GGA Time Check", 0));
        actionGGA_Altitude->setText(QApplication::translate("MainWindow", "GGA Altitude", 0));
        actionGGA_Position_Difference_12->setText(QApplication::translate("MainWindow", "GGA Position Difference 1/2", 0));
        actionGGA_900_sec_Diff_1_2->setText(QApplication::translate("MainWindow", "GGA 900 sec Diff 1/2", 0));
        actionCreate_CSV->setText(QApplication::translate("MainWindow", "Create *.CSV", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_Graph), QApplication::translate("MainWindow", "Graph", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "Options", 0));
        pushButton->setText(QApplication::translate("MainWindow", "Find", 0));
        label->setText(QApplication::translate("MainWindow", "Value", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_Stat), QApplication::translate("MainWindow", "Stat", 0));
        menuNMEA->setTitle(QApplication::translate("MainWindow", "NMEA", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
