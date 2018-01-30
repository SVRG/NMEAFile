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

    //void on_pushButton_clicked();

    void on_actionBLS_triggered();

    void on_actionBSS_Total_Valid_triggered();

    void on_actionHDT_Course_triggered();

    void on_actionBLS_Course_Difference_triggered();

    void on_actionScale_XY_triggered();

    void on_actionFind_Errors_triggered();

    void on_actionOpen_File_2_triggered();
    
    void on_actionGGA_Position_1_2_triggered();

    void on_actionGGA_Time_Check_triggered();

    void on_actionGGA_Altitude_triggered();

    void on_actionGGA_Position_Difference_12_triggered();

    void on_actionGGA_900_sec_Diff_1_2_triggered();

    void on_actionCreate_CSV_triggered();

    void on_ButtonBLH2XYZ_clicked();

    void on_actionGPX2NMEA_triggered();

    void on_actionStatistics_triggered();

    void on_actionRZD_RMS_Error_triggered();

    void on_actionGGA_Ref_Point_Diff_triggered();

    void on_actionGGA_Coord_Average_triggered();

    void on_actionRMC_Date_Check_triggered();

    void on_actionSDP_Standard_Deviation_triggered();

    void on_Sec2Time_clicked();

    void on_actionRMC_Position_triggered();

    void on_actionGGA_Diff_Time_Check_triggered();

    void on_pushButton_clicked();

    void on_NMEA_String_returnPressed();

    void on_actionPERC_Time_Check_triggered();

    void on_commandLine_returnPressed();

    void on_actionGGA_Distance_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
