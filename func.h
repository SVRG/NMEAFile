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

};

#endif // FUNC_H
