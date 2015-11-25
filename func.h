#ifndef FUNC_H
#define FUNC_H
#include <QTextStream>
#include "qcustomplot.h"


class func
{
public:
    func();
    static QString CRC(QString nmea);
    static void statMID_CEP(QVector<double> &Vector, double &resCEP, double &resMID);
    static void drawGraph(QCustomPlot *customPlot, QVector<double> &xVal, QVector<double> &yVal,QString xLable, QString yLable, QString graphName);
    static void drawMidGraph(QCustomPlot *customPlot, double yVal, int xCount);
    static void GGA_XY_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5, QVector<double> *bf_lf_Rb_Rl);
    static void GGA_Time_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static void GGA_Altitude_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static double TimeToSeconds(QString timeString);
    static void drawGraph3D_Fix_Float(QCustomPlot *customPlot, QVector<double> X1, QVector<double> Y1, QVector<double> X4, QVector<double> Y4, QVector<double> X5, QVector<double> Y5);
    static void Stat3D_Fix_Float(QTextBrowser *textBrowser, QVector<double> X1, QVector<double> X4, QVector<double> X5);
    static void GGA_BLH_Radians(QString GGA_String, double &B, double &L, double &H);
    static void BLH_to_XYZ(QString GGA_String, double &X, double &Y, double &Z);
    static void GGA_to_3D_Fix_Float(QStringList nmea, double X, double Y, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5);
    static void GGA_R_b_R_l(QString GGA_Sting, double &R_b, double &R_l);
    static void GGA_BLH_to_XY_0(QString GGA_String, double &X, double &Y, double b_fix, double l_fix, double R_b, double R_l);
    static void GGA_XY_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5,
                              double b_fix, double l_fix, double R_b, double R_l);
    static bool GGA_Check(QString GGA_String);
    static int GGA_Temp(QTextStream *in);
    static void GGA_XYTime_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *Time, QVector<double> *bf_lf_Rb_Rl);
    static void GGA_2Files_Diff(QString fileName1, QString fileName2, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X2, QVector<double> *Y2);
    static int Nearest_Time(double time, QVector<double> *TimeVector, int Start);

};

#endif // FUNC_H
