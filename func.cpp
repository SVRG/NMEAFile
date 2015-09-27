#include "func.h"
#include <QVector>
#include <QtMath>

func::func()
{

}
//----------------------------------------------------------------------------------------------------------
// Вычисление контрольной суммы // Есть пропуски - пока отключил
QString func::CRC(QString nmea)
{
    char* str = nmea.toLocal8Bit().data();
    char sum=0;  // Контрольная сумма
    int i=1;     // Начинаем с первого, т.к. нулевой символ $

    if(nmea.contains("*"))
    {
        while (str[i]!='*')
            sum=sum^str[i++]; //рассчет КС

    QString s = "";
    s = QString::number(sum,16); //перевод в шестнадцатиричный формат

    // Костыль? Проверка - если меньше 2 символов то добавить 0
    if(s.count()<2)
        s="0"+s;

    // Перевод в Uppercase
    return(s.toUpper());
    }
    else
        return("NO *");
}

//----------------------------------------------------------------------------------------------------------
// Вычисление среднего значения и СКО
void func::statMID_CEP(QVector<double> &Vector, double& resCEP, double& resMID)
{
    // Сумма значений
    double summ=0.;
    resMID=0.;
    foreach(double var, Vector)
        summ+=var;

    // Среднее арифметическое значение
    resMID=summ/Vector.count();

    // СКО
    resCEP=0.;
    foreach (double var, Vector)
        resCEP+=(var-resMID)*(var-resMID);

    resCEP=resCEP/Vector.count();
    resCEP=sqrt(resCEP);
    return;
}
//----------------------------------------------------------------------------------------------------------
// Прорисовка графика
// При отображении среднего значения не рисуется линия. Ввести параметр?
void func::drawGraph(QCustomPlot *customPlot, QVector<double> &xVal, QVector<double> &yVal,QString xLable, QString yLable, QString graphName)
{
    // Добавляем график
    customPlot->addGraph();

    // Идентифицируем график
    int ID = customPlot->graphCount()-1;

    QPen pen; // Тип маркера

    // Задаем цвет в зависимости от номера
    switch ( ID ) {
    case 0:
      pen.setColor(Qt::red);
      break;
    case 1:
      pen.setColor(Qt::green);
      break;
    case 2:
      pen.setColor(Qt::blue);
      break;
    case 3:
      pen.setColor(Qt::black);
      break;
    case 4:
      pen.setColor(Qt::cyan);
      break;
    default:
      pen.setColor(Qt::red);
      break;
    }

    // Указваем данные
    customPlot->graph(ID)->setData(xVal, yVal);

    // Указываем цвет
    customPlot->graph(ID)->setPen(pen);

    // Название осей
    customPlot->xAxis->setLabel(xLable);
    customPlot->yAxis->setLabel(yLable);

    // Доп настройки
    customPlot->yAxis->setScaleType(QCPAxis::stLinear);
    customPlot->graph(ID)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(ID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    customPlot->graph(ID)->setName(graphName);

    customPlot->rescaleAxes();

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    customPlot->replot();
}
