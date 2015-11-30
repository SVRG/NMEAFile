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
// Прорисовка графика по векторам. Основная функция.
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
      pen.setColor(Qt::darkRed);
      break;
    case 4:
      pen.setColor(Qt::darkGreen);
      break;
    case 5:
      pen.setColor(Qt::darkBlue);
      break;
    case 6:
      pen.setColor(Qt::yellow);
      break;
    case 7:
      pen.setColor(Qt::cyan);
      break;
    case 8:
      pen.setColor(Qt::magenta);
      break;
    case 9:
      pen.setColor(Qt::darkYellow);
      break;
    case 10:
      pen.setColor(Qt::darkCyan);
      break;
    case 11:
      pen.setColor(Qt::darkMagenta);
      break;
    default:
      pen.setColor(Qt::black);
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

    // Доп настройки

    // Включаем авто-разбивку по основным
    customPlot->xAxis->setAutoTickStep(true);
    customPlot->yAxis->setAutoTickStep(true);

    // Авто-разбивка по основным примерно 10
    customPlot->xAxis->setAutoTickCount(10);
    customPlot->yAxis->setAutoTickCount(10);

    // Выключаем авто-разбивку по промежуточным
    customPlot->xAxis->setAutoSubTicks(false);
    customPlot->yAxis->setAutoSubTicks(false);

    // Устанавливаем разбивку по промежуточным 4 (по 5 делений)
    customPlot->xAxis->setSubTickCount(4);
    customPlot->yAxis->setSubTickCount(4);

    customPlot->rescaleAxes();

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    customPlot->replot();
}
//----------------------------------------------------------------------------------------------------------
// Прорисовка графика Средней линии. Вспомогательная функция.
void func::drawMidGraph(QCustomPlot *customPlot, double yMidVal, int xCount)
{
    // Добавляем график
    customPlot->addGraph();

    // Идентифицируем график
    int ID = customPlot->graphCount()-1;

    QPen pen; // Тип маркера

    pen.setColor(Qt::black);

    QVector<double> xVal, yVal;
    xVal.append(0.);
    xVal.append((double)xCount);

    yVal.append(yMidVal);
    yVal.append(yMidVal);
    // Указваем данные
    customPlot->graph(ID)->setData(xVal, yVal);

    // Указываем цвет
    customPlot->graph(ID)->setPen(pen);

    // Доп настройки
    customPlot->yAxis->setScaleType(QCPAxis::stLinear);
    customPlot->graph(ID)->setLineStyle(QCPGraph::lsLine); // Линия
    customPlot->graph(ID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    // Доп настройки
    customPlot->xAxis->setAutoTickStep(true);
    customPlot->yAxis->setAutoTickStep(true);

    customPlot->xAxis->setAutoTickCount(10);
    customPlot->yAxis->setAutoTickCount(10);

    customPlot->xAxis->setAutoSubTicks(false);
    customPlot->yAxis->setAutoSubTicks(false);

    customPlot->xAxis->setSubTickCount(4);
    customPlot->yAxis->setSubTickCount(4);

    customPlot->rescaleAxes();

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    customPlot->replot();
}
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует векторы
// 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5; bf_lf_Rb_Rl - вектор начальных условий
void func::GGA_XY_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5,
                                     QVector<double> *bf_lf_Rb_Rl)
{

    //QVector<double> bf_lf_Rb_Rl;

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Счетчик невалидных решений
       int notValid=0;

       //Координаты начальной точки, коэффициенты
       double b_fix=0., l_fix=0., R_b = 0., R_l = 0.;

       // Счетчик времени (решений)
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(func::GGA_Check(line)) // Ищем строку GGA
          {

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если данные не достоверны - пропускаем
              if(nmea[6]=="0"){
                  notValid++;
                  continue;
              }

              // BLH->XYZ Строковые и числовые переменные
              double B=0.,L=0.,H=0.;

              func::GGA_BLH_Radians(line,B,L,H);

              // Вариант трека по приращениям (Storegis)
              // Начальная точка. Вычисляем коэффициенты один раз?
              if(i==1) // Первое валидное решение
              {
                  func::GGA_R_b_R_l(line,R_b,R_l);
                  b_fix=B; // Начальная широта
                  l_fix=L; // Начальная долгота

                  // Передаем вектор начальных условий и Коэффициенты
                  bf_lf_Rb_Rl->append(b_fix);
                  bf_lf_Rb_Rl->append(l_fix);
                  bf_lf_Rb_Rl->append(R_b);
                  bf_lf_Rb_Rl->append(R_l);
              }

              // Вычисляем разницу между начальными и текущими координатами и умножаем на коэффициенты
              if(i>0)
              {
              double X=0., Y=0.;

              // Вычисляем относительные координаты
              func::GGA_BLH_to_XY_0(line, X, Y, b_fix, l_fix, R_b, R_l);

              // Разбиваем данные по типу решения
              func::GGA_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);

              } // if i>0

              // Счетчик для валидных решений
              i++;

            }
       }
       inputFile.close();
}
}

}
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует векторы временных рядов
// 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
void func::GGA_Time_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       int notValid=0; // Счетчик невалидных решений

       int i=0; // Счетчик времени (решений)

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(func::GGA_Check(line)) // Ищем строку GGA
          {

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если данные не достоверны пропускаем
              if(nmea[6]=="0")
                {
                  notValid++;
                  continue;
                }

              QString timeString=nmea[1]; // Поле время

              // Если время менее 6 символов - пропускаем
              if(timeString.count()<6)
                  continue;

              double X = func::TimeToSeconds(timeString);
              double Y = (double)i;

              // Разбиваем данные по типу решения
              func::GGA_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);

              // Счетчик для валидных решений
              i++;
            }
       }
       inputFile.close();
}
}
}
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует векторы высот
// 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
void func::GGA_Altitude_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Счетчик невалидных решений
       int notValid=0;

       // Счетчик времени (решений)
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,093013.20,5544.5527978,N,03731.3594562,E,4,16,0.7,174.254,M,14.760,M,1.2,0002*46
          //    0        1           2     3      4        5 6  7  8    9     10   11  12 13  14
          if(func::GGA_Check(line)) // Ищем строку GGA
          {

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если данные не достоверны пропускаем
              if(nmea[6]=="0")
                {
                  notValid++;
                  continue;
                }

              QString Altitude=nmea[9]; // Поле высота

              double X = func::TimeToSeconds(nmea[1]); // Время в секундах с начала дня
              //double X = (double)i; // Количество отсчетов
              double Y = Altitude.toDouble();

              // Разбиваем данные по типу решения
              func::GGA_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);

              // Счетчик для валидных решений
              i++;

          }
       }
       inputFile.close();
}
}
}
//----------------------------------------------------------------------------------------------------------------
// Перевод Строки "Время" в секунды с начала дня (00:00)
double func::TimeToSeconds(QString timeString)
{
    if(timeString.count()<6)
        return(0.);

    QString Hours,Minutes,Seconds,Fractional; // Часы/Минуты/Секунды/Дробная часть

    Fractional=timeString.split('.')[1]; // Дробная часть
    if(Fractional.count()<1)
        Fractional="";

    timeString = timeString.split('.')[0]; // Часы минуты и целая часть секунд

    Hours = timeString.left(2); // Часы
    Minutes = timeString.mid(2,2); // Минуты
    Seconds = timeString.right(2) + '.' + Fractional; // Секунды целая + дробная часть

    return(Hours.toDouble()*3600 + Minutes.toDouble()*60 + Seconds.toDouble());
}
//----------------------------------------------------------------------------------------------------------------
// Рисуем графики 3D/Fix/Float
void func::drawGraph3D_Fix_Float(QCustomPlot *customPlot, QVector<double> X1, QVector<double> Y1, QVector<double> X4,
                                 QVector<double> Y4, QVector<double> X5, QVector<double> Y5)
{
    // 3D...
    func::drawGraph(customPlot,X1,Y1,"X","Y","3D");
    // Fix
    func::drawGraph(customPlot,X4,Y4,"X","Y","RTK");
    // Float
    func::drawGraph(customPlot,X5,Y5,"X","Y","Float");

}
//----------------------------------------------------------------------------------------------------------------
// Выводим статистику 3D/Fix/Float
void func::Stat3D_Fix_Float(QTextBrowser *textBrowser, QVector<double> X1, QVector<double> X4, QVector<double> X5)
{
    // Выводим статистику
    double Total=0, Fix=0, Float=0;
    Fix = X4.count();
    Float = X5.count();
    Total = Fix + Float + X1.count();
    // Общее количество точек
    textBrowser->append("Total points: "+QString::number(Total));
    // Fix решений
    textBrowser->append("Fix points: "+QString::number(Fix/Total*100)+"% ("+QString::number(Fix)+")");
    // Float решений
    textBrowser->append("Float points: "+QString::number(Float/Total*100)+"% ("+QString::number(Float)+")");
    // Невалидныйх решений
    //ui->textBrowser->append("Not Valid points: "+QString::number(notValid));
}
//----------------------------------------------------------------------------------------------------------------
// GGA - перевод координат в радианы, h - в метры
void func::GGA_BLH_Radians(QString GGA_String, double &B, double &L, double &H)
{
    // Разбиваем строку по запятой
    QStringList nmea = GGA_String.split(',');

    // BLH->XYZ Строковые и числовые переменные
    QString BStr,LStr;

    // Широта/Долгота [2] / [4]
    BStr=nmea[2];
    LStr=nmea[4];

    // Вывод строки GGA в Лог-окно
    //ui->textBrowser->append(line);

    // Широта - Градусы
    B = QString(BStr.left(2)).toDouble();
    // Долгота - Градусы
    L = QString(LStr.left(3)).toDouble();
    // Высота над эллипсоидом = высота над геоидом + превышение геоида над эллипсоидом
    H = QString(nmea[9]).toDouble()+QString(nmea[11]).toDouble();

    // Широта/Долгота - Градусы + Минуты. Переводим в радианы.
    B = qDegreesToRadians(B+QString(BStr.mid(2)).toDouble()/60);
    L = qDegreesToRadians(L+QString(LStr.mid(3)).toDouble()/60);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - Перевод BLH в XYZ - Пока нигде не используется
void func::BLH_to_XYZ(QString GGA_String, double &X, double &Y, double &Z)
{
    double B=0., L=0., H=0.;
    func::GGA_BLH_Radians(GGA_String,B,L,H);

    //Вычисление XYZ. Значение N можно вычислить 1 раз?
    double N    = 6378137. / sqrt(1. - 0.00669437999 * sin(B) * sin(B));
    X = (N + H) * cos(L) * cos(B);
    Y = (N + H) * sin(L) * cos(B);
    Z = ((1. - 0.00669437999) * N + H) * sin(B);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - Разбивка по типу решения
void func::GGA_to_3D_Fix_Float(QStringList nmea, double X, double Y, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5)
{
    // Разбиваем данные по типу решения
    // Если тип решения RTK Fix
    if(nmea[6]=="4")
    {
        X4->append(X);
        Y4->append(Y);
    }
    // RTK Float
    else if (nmea[6]=="5") {
        X5->append(X);
        Y5->append(Y);
    }
    else
    // Остальные - 3D/3D Diff/DR ...
    {
        X1->append(X);
        Y1->append(Y);
    }
}
//----------------------------------------------------------------------------------------------------------------
// GGA - коэффициенты приращения для пересчета координат в м от базовых координат
void func::GGA_R_b_R_l(QString GGA_Sting, double &R_b, double &R_l)
{
    double H=0.; // Высота - не используется, но нужна в функции

    double b_fix=0.; // Начальная широта
    double l_fix=0.; // Начальная долгота

    func::GGA_BLH_Radians(GGA_Sting,b_fix,l_fix,H);

    // Константы
    double E_2 = 0.00669437999;// Эксцентриситет
    double Ae = 6378137.;// Большая полуось

    // Вычисляем коэффициенты приращения
    double W = sqrt(1 - E_2 * b_fix * b_fix);
    R_b = Ae * (1. - E_2) / (W * W * W) + l_fix;
    R_l = (Ae / W + l_fix) * cos(b_fix);

}
//----------------------------------------------------------------------------------------------------------------
// GGA - переводим координаты в XY отностительно начальных координат
void func::GGA_BLH_to_XY_0(QString GGA_String, double &X, double &Y, double b_fix, double l_fix, double R_b, double R_l)
{
    double B=0., L=0., H=0.;

    func::GGA_BLH_Radians(GGA_String,B,L,H);

    Y=(B-b_fix)*R_b;
    X=(L-l_fix)*R_l;
}
//----------------------------------------------------------------------------------------------------------------
// GGA - относительный трек. Дополнительно передаются параметры первого трека
void func::GGA_XY_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5,
                          double b_fix, double l_fix, double R_b, double R_l)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Счетчик невалидных решений
       int notValid=0;

       // Счетчик времени (решений)
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(func::GGA_Check(line)) // Ищем строку GGA
          {

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если данные не достоверны - пропускаем
              if(nmea[6]=="0"){
                  notValid++;
                  continue;
              }

              double X=0., Y=0.;

              // Вычисляем относительные координаты
              func::GGA_BLH_to_XY_0(line, X, Y, b_fix, l_fix, R_b, R_l);

              // Разбиваем данные по типу решения
              func::GGA_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);

              // Счетчик валидных решений
              i++;

            }
       }
       inputFile.close();
}
}
}
//----------------------------------------------------------------------------------------------------------------
// GGA - проверка
bool func::GGA_Check(QString GGA_String)
{
    if(GGA_String.contains("GGA")) // Ищем строку GGA
//    if(GGA_String.split('*').count()==2) // Проверяем, что есть контрольная сумма
    if(GGA_String.split(',').count()==15)// and (GGA_String.split('*')[1]==func::CRC(GGA_String))) // Проверка на количество полей, Проверка контрольной суммы
        return(true);
return(false);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - темп. Проверяем темп по файлу. Пока не корректно работает! Переделать.
// Предполагается использовать при поиске ближайшего времени
int func::GGA_Temp(QTextStream *in)
{
int i=0, notValid=0;
QVector<double> Time;

    while (!in->atEnd()) {
        QString line = in->readLine();

        if(func::GGA_Check(line)) // Ищем строку GGA
        {
            // Разбиваем строку по запятой
            QStringList nmea = line.split(',');

            // Если данные не достоверны - пропускаем
            if(nmea[6]=="0"){
                notValid++;
                continue;
            }
        QString fractional=nmea[1];
        fractional = "0."+fractional.split('.')[1];
        if(fractional.toDouble()>0)
            Time.append(fractional.toDouble());
        i++; // Счетчик валидных решений

        if(i==10)
            break;
        }
    }
int Hz = (int)(1);
return(Hz);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - относительный трек. Дополнительно передаются параметры первого трека
void func::GGA_XYTime_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *Time, QVector<double> *bf_lf_Rb_Rl)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Счетчик невалидных решений
       int notValid=0;

       // Счетчик времени (решений)
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(func::GGA_Check(line)) // Ищем строку GGA
          {

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если данные не достоверны - пропускаем
              if(nmea[6]=="0"){
                  notValid++;
                  continue;
              }

              // Вариант трека по приращениям (Storegis)
              // Начальная точка. Вычисляем коэффициенты один раз?
              if(i==1 and bf_lf_Rb_Rl->count()<4) // Первое валидное решение
              {
                  // BLH->XYZ Строковые и числовые переменные
                  double B=0.,L=0.,H=0., R_b=0, R_l=0;

                  func::GGA_BLH_Radians(line,B,L,H);

                  func::GGA_R_b_R_l(line,R_b,R_l);
                  double b_fix=B; // Начальная широта
                  double l_fix=L; // Начальная долгота

                  // Передаем вектор начальных условий и Коэффициенты
                  bf_lf_Rb_Rl->append(b_fix);
                  bf_lf_Rb_Rl->append(l_fix);
                  bf_lf_Rb_Rl->append(R_b);
                  bf_lf_Rb_Rl->append(R_l);
              }


              double X=0., Y=0.;

              // Вычисляем относительные координаты
              func::GGA_BLH_to_XY_0(line, X, Y, bf_lf_Rb_Rl->value(0), bf_lf_Rb_Rl->value(1), bf_lf_Rb_Rl->value(2), bf_lf_Rb_Rl->value(3));

              X1->append(X);
              Y1->append(Y);
              Time->append(func::TimeToSeconds(nmea[1]));

              // Счетчик для валидных решений
              i++;

            }
       }
       inputFile.close();
}
}
}
//----------------------------------------------------------------------------------------------------------------
// GGA - разности по координатам
void func::GGA_2Files_Diff(QString fileName1, QString fileName2, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X2, QVector<double> *Y2)
{
    QVector<double> Time1, Time2, X, Y;
    QVector<double> bf_lf_Rb_Rl;

    func::GGA_XYTime_0_Vectors(fileName1,X1,Y1,&Time1,&bf_lf_Rb_Rl);

    func::GGA_XYTime_0_Vectors(fileName2,X2,Y2,&Time2,&bf_lf_Rb_Rl);

    int time1_count=Time1.count(), time2_count = Time2.count(); // Количество записей в массивах
    int j=0; // Счетчик по Time 2

    if(time1_count==0 or time2_count==0)
        return;

    // Есть два массива координаты и время по двум файлам
    // Надо синхронизировать время

    for(int i=0;i<Time1.count();i++)
    {
        // Если первый файл раньше второго
        if(Time1.value(i)<Time2.value(j)) // Если T1 раньше T2 - переводим время вперед
        {
            //i=i+(int)(Time2.value(j)-Time1.value(i)); // Прибавляем разницу по количеству секунд. По идее надо знать темп решения
            continue;
        }

        j=func::Nearest_Time(Time1.value(i),&Time2,j);

        if(j==-1) // Если ближайшее время не было найдено
            continue;

        double x_diff = X1->value(i)-X2->value(j);
        double y_diff = Y1->value(i)-Y2->value(j);

        X.append(Time1[i]);
        Y.append(sqrt(x_diff*x_diff+y_diff*y_diff));
    }
X1->clear();
Y1->clear();

X1->append(X);
Y1->append(Y);
}
//----------------------------------------------------------------------------------------------------------------
// Time - найти ближайшее время. Вынести глубину поиска в параметр?
int func::Nearest_Time(double time, QVector<double> *TimeVector, int Start)
{
    int search_depth = 5000; // Глубина поиска
    int count = TimeVector->count(); // Количество
    bool forw=true, back=true; // Флаги вперед/назад

    for(int i=0;i<search_depth;i++)
    {
        // Поиск вперед
        if(forw) // Если поднят флаг, продолжаем поиск
        {
            if(Start+i<=count)
             {
                double time_f=TimeVector->value(Start+i);

                if(time==time_f) // Если нашли, то возвращаем значение
                      return(Start+i);

                 if(time<time_f) // Если прошли мимо то дальше не имеет смысла искать
                         forw=false;
             }
             else
                 forw=false;
         }//--------------

         // Поиск назад
         if(back) // Если поднят флаг, продолжаем поиск
         {
           if(Start-i>=0)
             {
               double time_b=TimeVector->value(Start-i);

               if(time==time_b) // Если нашли, то возвращаем значение
                      return(Start-i);

                 if(time>time_b) // Если прошли мимо то дальше не имеет смысла искать
                     back=false;
             }
             else
                    back=false;
         }//-------------

         if(!forw and !back) // Если оба флага опущены завершаем поиск
            return(-1);

    }
return(-1);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - разности по вектору ошибок между текущей точкой и через 900 секунд
void func::GGA_2Files_Diff_900(QString fileName1, QString fileName2, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X2, QVector<double> *Y2)
{
    func::GGA_2Files_Diff(fileName1,fileName2,X1,Y1,X2,Y2);

    QVector<double> X, Y;
    int Second_Point=0;
    int Count = X1->count();

    for(int i=0;i<=Count;i++)
    {
        Second_Point=func::Nearest_Time(X1->value(i)+900.,X1,i+900); // Надо еще учитывать темп решения

        if(Second_Point==-1)
            continue;

        double diff=sqrt((Y1->value(Second_Point)-Y1->value(i))*(Y1->value(Second_Point)-Y1->value(i)));

        X.append(X1->value(Second_Point));
        Y.append(diff);
    }
X1->clear();
Y1->clear();

X1->append(X);
Y1->append(Y);
}
