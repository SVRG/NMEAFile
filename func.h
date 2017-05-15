#ifndef FUNC_H
#define FUNC_H
#include <QTextStream>
#include "qcustomplot.h"


class func
{
public:
    func();
    static QString CRC(QString nmea);
    static bool CRC_Check(QString nmea);

    static void statMID_CEP(QVector<double> &Vector, double &resCEP, double &resMID);
    static void Stat3D_Fix_Float(QTextBrowser *textBrowser, QVector<double> *X1, QVector<double> *X4, QVector<double> *X5);
    static void Statistics(QString fileName, QTextBrowser *textBrowser);

    static void drawGraph(QCustomPlot *customPlot, QVector<double> &xVal, QVector<double> &yVal,QString xLable, QString yLable, QString graphName);
    static void drawMidGraph(QCustomPlot *customPlot, double yVal, double xMin, double xMax);
    static void drawGraph3D_Fix_Float(QCustomPlot *customPlot, QVector<double> X1, QVector<double> Y1, QVector<double> X4, QVector<double> Y4, QVector<double> X5, QVector<double> Y5);

    static void GGA_XY_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5, QVector<double> *bf_lf_Rb_Rl);
    static void GGA_BLH_Radians(QString GGA_String, double &B, double &L, double &H);
    static void GGA_Time_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static void GGA_DiffTime_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static void GGA_Altitude_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static void GGA_R_b_R_l(QString GGA_Sting, double &R_b, double &R_l);
    static void GGA_BLH_to_XY_0(QString GGA_String, double &X, double &Y, double b_fix, double l_fix, double R_b, double R_l);
    static void GGA_XY_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5,
                              double b_fix, double l_fix, double R_b, double R_l);
    static bool GGA_Check(QString GGA_String);
    static int  GGA_Temp(QTextStream *in);
    static void GGA_XYTime_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *Time, QVector<double> *bf_lf_Rb_Rl);
    static void GGA_2Files_Diff(QString fileName1, QString fileName2, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X2, QVector<double> *Y2);
    static void GGA_2Files_Diff_900(QString fileName1, QString fileName2, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X2, QVector<double> *Y2);
    static void GGA_to_3D_Fix_Float(QStringList nmea, double X, double Y, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static double GGA_2Point_Diff(QString GGA_Line1, QString GGA_Line2);

    static void PERC_Time_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1);

    static double TimeToSeconds(QString timeField);
    static double timeToSeconds(QString& currentTimeField, double& maxTime, int& day);
    static void BLH_to_XYZ(QString GGA_String, double &X, double &Y, double &Z);

    static void RMC_to_3D_Fix_Float(QStringList nmea, double X, double Y, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static void RMC_XY_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5, QVector<double> *bf_lf_Rb_Rl);

    static int Nearest_Time(double time, QVector<double> *TimeVector, int Start);
    static QString removeSpaces(QString string);
    static QString SecondsToTime(double seconds);
    static void qcustomPlotSettings(QCustomPlot* customPlot, bool xTime, bool yTime);
    static void drawGraph3D_Fix_Float_Time(QCustomPlot *customPlot, QVector<double> X1, QVector<double> Y1, QVector<double> X4,
                                     QVector<double> Y4, QVector<double> X5, QVector<double> Y5);
    static double TimeToQDateTime(QString timeField, int day = 0);
    static QString doubleToString(double number);

    // SQLite DB
    static void loadFileToDB(QString fileName); // Тестовая
    static void getDataFromDB();                // Тестовая
    static void dbGGA_XYTime_0_Vectors(QString fileName, QString table_name, QVector<double> *bf_lf_Rb_Rl);
    static void dbGGA_2Files_Diff(QVector<double> *Time, QVector<double> *Diff);

};

#endif // FUNC_H
