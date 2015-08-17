#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_File_triggered();

    void on_actionScale_triggered();

    void on_actionNAV_Param_triggered();

    void on_actionShow_Legend_triggered();

    void on_actionErrors_triggered();

    void on_actionRMC_triggered();

    void on_actionScale_Y_triggered();

    void on_actionGGA_Position_triggered();

    void on_actionBSS_Distance_triggered();

    void on_actionGGA_Diff_Age_triggered();

    void on_actionY_Logarithmic_triggered();

    void on_pushButton_clicked();

    void on_actionBLS_triggered();

    void on_actionBSS_Total_Valid_triggered();

    void on_actionHDT_Course_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H