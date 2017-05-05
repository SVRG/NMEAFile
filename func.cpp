#include "func.h"
#include <QVector>
#include <QtMath>
#include <QtGui>
#include <QtSql>

func::func()
{

}
//----------------------------------------------------------------------------------------------------------
// Вычисление контрольной суммы строки NMEA
// Нужен тест
QString func::CRC(QString nmea)
{
    if(!nmea.contains("*"))
        return("Err: NO *");

    if(nmea.count()<5)
        return("Err: Count <5");

    if(nmea[0]!='$')
        return("Err: First sign not $");

    char* str = nmea.toLocal8Bit().data();
    char sum=0;  // Контрольная сумма
    int i=1;     // Начинаем с первого, т.к. нулевой символ $

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
//----------------------------------------------------------------------------------------------------------
// Проверка строки на контрольную сумму
// Если контрольная сумма не совпадает - выдаем false, если совпадает - true
bool func::CRC_Check(QString nmea)
{
    // Если в строке нет * то выдаем false
    if(!nmea.contains("*"))
        return(false);

    if(nmea.split("*").count()!=2)
        return(false);

    QString CRC="";
    CRC = nmea.split("*")[1];

    if(CRC.count()!=2)
        return(false);

    if(func::CRC(nmea)==CRC)
        return(true);

    return(false);
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
// Прорисовка графика по векторам xVal, yVal. Основная функция.
void func::drawGraph(QCustomPlot *customPlot, QVector<double> &xVal, QVector<double> &yVal, QString xLable, QString yLable, QString graphName)
{
    // Добавляем график
    customPlot->addGraph();

    // Идентифицируем график
    int ID = customPlot->graphCount()-1;

    QPen pen; // Тип маркера

    // Задаем цвет в зависимости от номера графика
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

    // Устанавливаем данные для графика
    customPlot->graph(ID)->setData(xVal, yVal);

    // Указываем цвет
    customPlot->graph(ID)->setPen(pen);

    // Название осей
    customPlot->xAxis->setLabel(xLable);
    customPlot->yAxis->setLabel(yLable);

    bool xTime = false;

    if(xLable.contains("Time"))
        xTime = true;

    // Доп настройки
    func::qcustomPlotSettings(customPlot,xTime,false);

    customPlot->graph(ID)->setLineStyle(QCPGraph::lsNone); // Тип линии - отсутствует
    customPlot->graph(ID)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4)); // Тип точек - кружки, радиус 4
    customPlot->graph(ID)->setName(graphName); // Название графика

    customPlot->replot(); // Перерисовка графиков
}
//----------------------------------------------------------------------------------------------------------
// Прорисовка графика Средней линии. Вспомогательная функция. Переделать - на интервал
void func::drawMidGraph(QCustomPlot *customPlot, double yMidVal, double xMin, double xMax)
{
    // Добавляем график
    customPlot->addGraph();

    // Идентифицируем ID графика
    int ID = customPlot->graphCount()-1;

    QPen pen; // Тип маркера

    pen.setColor(Qt::black);

    QVector<double> xVal, yVal;
    xVal.append(xMin);
    xVal.append(xMax);

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

    customPlot->replot();
}
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует векторы координат X,Y
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

               // Временно! Координаты меток
               // QVector<double> TMC_X, TMC_Y;

               // Счетчик времени (решений)
               int i=0;

               // Пока не достигнем конца файла читаем строчки
               while (!in.atEnd()) {
                  // Считываем строку
                  QString line = in.readLine();

                  // Если строка содежит GGA
                  // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
                  //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
                  if(func::GGA_Check(line) or line.contains("TMC")) // Ищем строку GGA или TMC
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

                          if(line.contains("TMC")) // Второй раз проверяем... Надо оптимизировать
                          {
                              //TMC_X.append(X);
                              //TMC_Y.append(Y);

                              // Временное решение. Чтобы отличить от Int ставим статус 3D
                              nmea[6]="1";
                          }

                          // Разбиваем данные по типу решения
                          func::GGA_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);

                      }

                      // Счетчик для валидных решений
                      i++;

                    }
               }
               inputFile.close();
        }
    }

}
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует массивы времени по типу решения
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

           int i=0; // Счетчик времени (решений)

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит GGA
              // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
              //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
              if(func::GGA_Check(line) or line.contains("TMC")) // Ищем строку GGA / TMC
              {

                  // Разбиваем строку по запятой
                  QStringList nmea = line.split(',');

                  // Если данные не достоверны пропускаем
                  if(nmea[6]=="0")
                      continue;

                  QString timeString=nmea[1]; // Поле время

                  // Если время менее 6 символов - пропускаем
                  if(timeString.count()<6)
                      continue;

                  double X = func::TimeToQDateTime(timeString);
                  double Y = (double)i;

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
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует массивы разниц времени двух валидных точек
// 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
void func::GGA_DiffTime_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5)
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

           int i=0; // Счетчик времени (решений)
           int day=0;
           double curr_time = 0, pred_time=-1;

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит GGA
              // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
              //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
              if(func::GGA_Check(line) or line.contains("TMC")) // Ищем строку GGA
              {

                  // Разбиваем строку по запятой
                  QStringList nmea = line.split(',');

                  // Если данные не достоверны пропускаем
                  if(nmea[6]=="0")
                      continue;

                  curr_time = func::TimeToSeconds(nmea[1]);
                  if(curr_time<pred_time) // Следующий день
                    day++;

                  if(i>1)
                  {
                      double X = func::TimeToQDateTime(nmea[1],day);
                      double Y = curr_time-pred_time;

                  // Разбиваем данные по типу решения
                  func::GGA_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);
                  }

                  pred_time = curr_time;

                  // Счетчик валидных решений
                  i++;
                }
           }
           inputFile.close();
        }
    }
}
//----------------------------------------------------------------------------------------------------------
// Из файла NMEA формирует векторы высот Y_ и времени X_
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
           int day=0;
           double curr_time = 0, pred_time=-1;

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит GGA
              // $GPGGA,093013.20,5544.5527978,N,03731.3594562,E,4,16,0.7,174.254,M,14.760,M,1.2,0002*46
              //    0        1           2     3      4        5 6  7  8    9     10   11  12 13  14
              if(func::GGA_Check(line) or line.contains("TMC")) // Ищем строку GGA
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

                  curr_time = func::TimeToSeconds(nmea[1]);
                  if(curr_time<pred_time) // Следующий день
                    day++;
                  pred_time = curr_time;

                  double X = func::TimeToQDateTime(nmea[1],day); // Время в секундах с начала дня
                  double Y = Altitude.toDouble();

                  if(line.contains("TMC"))
                  {
                      // TODO: Временно! Если строка TMC то меняем тип на 3D
                      nmea[6] = "1";
                  }

                  // Записываем данные XY в зависимости от типа решения в соответствующие векторы
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
double func::TimeToSeconds(QString timeField)
{
    if(timeField.count()<6)
        return(-1.);

    QString Hours="00",Minutes="00",Seconds="00",Fractional="00"; // Часы/Минуты/Секунды/Дробная часть секунды

    Fractional=timeField.split('.')[1]; // Дробная часть

    timeField = timeField.split('.')[0]; // Часы минуты и целая часть секунд

    Hours = timeField.left(2); // Часы
    Minutes = timeField.mid(2,2); // Минуты
    Seconds = timeField.right(2) + '.' + Fractional; // Секунды целая + дробная часть

    return(Hours.toDouble()*3600 + Minutes.toDouble()*60 + Seconds.toDouble());
}
//----------------------------------------------------------------------------------------------------------------
// Перевод "Секунды с начала дня" в формат "Время" ччммсс.сс...
QString func::SecondsToTime(double seconds)
{
    int hours=0, min=0;
    double sec=0;
    QString res = "", S="";
    hours = (int)(seconds / 3600);
    min = (int)((seconds - hours*3600) / 60);
    sec = seconds - (double)hours*3600 - (double)min*60;

    if(sec<10)
        S="0";

    res = QString("%1").arg(hours, 2, 10, QChar('0')) + QString("%1").arg(min, 2, 10, QChar('0')) + S + QString::number(sec);

    return(res);

}
//----------------------------------------------------------------------------------------------------------------
// Перевод Строки "Время" в QDateTime
double func::TimeToQDateTime(QString timeField, int day)
{
    if(timeField.count()<6)
        return(-1.);

    QString Hours="00",Minutes="00",Seconds="00",Fractional="00"; // Часы/Минуты/Секунды/Дробная часть секунды

    Fractional=timeField.split('.')[1]; // Дробная часть секунды

    timeField = timeField.split('.')[0]; // Часы минуты и целая часть секунд

    Hours = timeField.left(2); // Часы
    Minutes = timeField.mid(2,2); // Минуты
    Seconds = timeField.right(2) + "." + Fractional; // Секунды

    QTime time = QTime(Hours.toInt(),Minutes.toInt(),0,0);
    //QTime time = QTime::fromString(timeField,"hhmmss.z");

    return(QDateTime(QDate(2000,1,1+day),time).toTime_t() + Seconds.toDouble());

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
// Рисуем графики 3D/Fix/Float, по оси X - время
void func::drawGraph3D_Fix_Float_Time(QCustomPlot *customPlot, QVector<double> X1, QVector<double> Y1, QVector<double> X4,
                                 QVector<double> Y4, QVector<double> X5, QVector<double> Y5)
{
    // 3D...
    func::drawGraph(customPlot,X1,Y1,"Time","Value","3D");
    // Fix
    func::drawGraph(customPlot,X4,Y4,"Time","Value","RTK");
    // Float
    func::drawGraph(customPlot,X5,Y5,"Time","Value","Float");

}
//----------------------------------------------------------------------------------------------------------------
// Выводим статистику 3D/Fix/Float
void func::Stat3D_Fix_Float(QTextBrowser *textBrowser, QVector<double> *X1, QVector<double> *X4, QVector<double> *X5)
{
    // Выводим статистику
    double Total=0., Fix=0., Float=0., Other=0.;
    Fix = (double)X4->count();
    Float = (double)X5->count();
    Other = (double)X1->count();
    Total = Fix + Float + Other;
    // Общее количество точек
    textBrowser->append("Total points: "+ func::doubleToString(Total));
    // Fix решений
    textBrowser->append("Fix points: "+doubleToString(Fix/Total*100)+"% ("+doubleToString(Fix)+")");
    // Float решений
    textBrowser->append("Float points: "+doubleToString(Float/Total*100)+"% ("+doubleToString(Float)+")");
    // Float решений
    textBrowser->append("3D/Diff/... points: "+doubleToString(Other/Total*100)+"% ("+doubleToString(Other)+")");
    // Невалидныйх решений
    //ui->textBrowser->append("Not Valid points: "+func::doubleToString(notValid));
}
//----------------------------------------------------------------------------------------------------------------
// GGA/RMC - перевод координат в радианы, h - в метры над эллипсоидом (GGA only)
void func::GGA_BLH_Radians(QString GGA_String, double &B, double &L, double &H)
{
    // Разбиваем строку по запятой
    QStringList nmea = GGA_String.split(',');

    // BLH->XYZ Строковые и числовые переменные
    QString BStr="0",LStr="0", HStr="0";

    //$GPGGA,042820.00,5452.9685563,N,08258.4430767,E,4,17,0.7,149.822, M ,-37.031,M  , 1.0,0002*60
    //   0        1        2        3      4        5 6  7  8    9     10   11     12   13   14
    if(GGA_String.contains("GGA"))
    {
        // Широта/Долгота [2] / [4]
        BStr=nmea[2];
        LStr=nmea[4];
        // Высота над эллипсоидом = высота над геоидом + превышение геоида над эллипсоидом
        H = QString(nmea[9]).toDouble()+QString(nmea[11]).toDouble();
    }
    //$GPRMC,042819.80,A,5452.9685321,N,08258.4434049,E,3.37,277.95,250716,0.0,E, R*2F
    //   0        1    2      3       4     5         6  7      8      9    10 11 12
    else if(GGA_String.contains("RMC"))
    {
        // Широта/Долгота [2] / [4]
        BStr=nmea[3];
        LStr=nmea[5];
        H = 0; // в RMC нет высоты
    }

    // Вывод строки GGA в Лог-окно
    //ui->textBrowser->append(line);

    // Широта - Градусы. Два знака слева
    B = QString(BStr.left(2)).toDouble();
    // Долгота - Градусы. Три знака слева
    L = QString(LStr.left(3)).toDouble();

    // Широта/Долгота - Градусы + Минуты. Переводим в радианы.
    B = qDegreesToRadians(B+QString(BStr.mid(2)).toDouble()/60);
    L = qDegreesToRadians(L+QString(LStr.mid(3)).toDouble()/60);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - Перевод BLH в XYZ
void func::BLH_to_XYZ(QString GGA_String, double &X, double &Y, double &Z)
{
    double B=0., L=0., H=0.;
    func::GGA_BLH_Radians(GGA_String,B,L,H);

    //Вычисление XYZ. Уточнить: Значение N можно вычислить 1 раз?
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
// Результат будет записан в переменные X и Y
void func::GGA_BLH_to_XY_0(QString GGA_String, double &X, double &Y, double b_fix, double l_fix, double R_b, double R_l)
{
    double B=0., L=0., H=0.;

    func::GGA_BLH_Radians(GGA_String,B,L,H);

    Y=(B-b_fix)*R_b;
    X=(L-l_fix)*R_l;
}
//----------------------------------------------------------------------------------------------------------------
// GGA - относительный трек. Дополнительно передаются параметры опорной точки
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
    if(GGA_String.split('*').count()==2) // Проверяем, что есть контрольная сумма
    if(GGA_String.split(',').count()==15 and (GGA_String.split('*')[1]==func::CRC(GGA_String))) // Проверка на количество полей, Проверка контрольной суммы
        return(true);
return(false);
}
//----------------------------------------------------------------------------------------------------------------
// GGA - темп. Проверяем темп по файлу. Пока не корректно работает! Переделать. Надо ввести критерий достоверности, например чтобы количество с одним значением преобладало (на 51%)
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
// GGA - трек относитеотно начальной точки. Дополнительно передаются параметры
void func::GGA_XYTime_0_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *Time, QVector<double> *bf_lf_Rb_Rl)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
        {
        // Связываем переменную с физическим файлом
        QFile inputFile(fileName);
        // Если все ОК то открываем файл
        if (inputFile.open(QIODevice::ReadOnly)){

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
                      if(i==1 and bf_lf_Rb_Rl->count()<4) // Первое валидное решение, если не были заданы коэффициенты
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
// Реализация DB
// GGA - трек относитеотно начальной точки. Дополнительно передаются параметры
void func::dbGGA_XYTime_0_Vectors(QString fileName, QString table_name, QVector<double> *bf_lf_Rb_Rl)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
        {
        // Связываем переменную с физическим файлом
        QFile inputFile(fileName);
        // Если все ОК то открываем файл
        if (inputFile.open(QIODevice::ReadOnly)){

            QTextStream in(&inputFile);

            // Счетчик невалидных решений
            int notValid=0;

            // Счетчик времени (решений), дни
            int i=0, day=0;

            double maxTime = -1;

            QSqlDatabase dbase;

            if( QSqlDatabase::contains( "dbConnection" ) )
            {
                dbase = QSqlDatabase::database( "dbConnection" );
            }
            else
            {
                dbase = QSqlDatabase::addDatabase("QSQLITE","dbConnection");

                QString dbPath = QCoreApplication::applicationDirPath() + "/";
                dbase.setDatabaseName(dbPath + "appDB.sqlite");
            }

            if (!dbase.open()) {
                qDebug() << "dbGGA_XYTime_0_Vectors: Не удается открыть БД - " << dbase.lastError();
                QMessageBox::critical(0,"DB Err","dbGGA_XYTime_0_Vectors: "+dbase.lastError().text(),QMessageBox::Ok);
                return;
            }

            QSqlQuery query(dbase);

                    // удаляем таблицу
                    query.prepare("DROP TABLE IF EXISTS "+table_name+";");

                    if (!query.exec()) {
                        qDebug() << "dbGGA_XYTime_0_Vectors: DROP TABLE Err - " << query.lastError();
                    }

                    query.prepare("CREATE TABLE "+table_name+"("
                                    "id_time INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "time doudle,"
                                    "x double,"
                                    "y double,"
                                    "type INTEGER"
                                    ");");

                    if (!query.exec()) {
                        qDebug() << "dbGGA_XYTime_0_Vectors Не удается создать таблицу: " << query.lastError();
                    }

                    // Старт транзакции
                    dbase.transaction();

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

                      QString type = nmea[6];

                      // Вариант трека по приращениям (Storegis)
                      // Начальная точка. Вычисляем коэффициенты один раз?
                      if(i==1 and bf_lf_Rb_Rl->count()<4) // Первое валидное решение, если не были заданы коэффициенты
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


                      double x=0., y=0., time;

                      // Вычисляем относительные координаты
                      func::GGA_BLH_to_XY_0(line, x, y, bf_lf_Rb_Rl->value(0), bf_lf_Rb_Rl->value(1), bf_lf_Rb_Rl->value(2), bf_lf_Rb_Rl->value(3));
                      time = func::timeToSeconds(nmea[1],maxTime,day);

                      // DML
                      query.prepare("INSERT INTO "+table_name+"(time, x, y, type) "
                                    "VALUES (:time, :x, :y, :type);");
                      query.bindValue(":time",time);
                      query.bindValue(":x",x);
                      query.bindValue(":y",y);
                      query.bindValue(":type",type.toInt());

                      if (!query.exec()) {
                            qDebug() << "dbGGA_XYTime_0_Vectors Данные не вставляются: " << query.lastError();
                            continue;
                      }

                      // Счетчик для валидных решений
                      i++;
                    }
               }

            dbase.commit(); // Конец транзакции
            inputFile.close();
            qDebug() << "Вставлено: " << i;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------
// GGA - разности по 2D координатам в двух файлах
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
        // Если первый файл начинается раньше второго
        if(Time1.value(i)<Time2.value(j)) // Если T1 раньше T2 - переводим время вперед
            continue;

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
// Реализация БД
// GGA - разности по 2D координатам в двух файлах
// Есть два массива координаты и время по двум файлам
// Надо синхронизировать время
void func::dbGGA_2Files_Diff(QVector<double> *Time, QVector<double> *Diff)
{
    QVector<double> bf_lf_Rb_Rl;

    QSqlDatabase dbase;

    if( QSqlDatabase::contains( "dbConnection" ) )
    {
        dbase = QSqlDatabase::database( "dbConnection" );
    }
    else
    {
        dbase = QSqlDatabase::addDatabase("QSQLITE","dbConnection");

        QString dbPath = QCoreApplication::applicationDirPath() + "/";
        dbase.setDatabaseName(dbPath + "appDB.sqlite");
    }

    if (!dbase.open()) {
            qDebug() << "dbGGA_2Files_Diff: Не удается подключиться к БД - " << dbase.lastError();
            QMessageBox::critical(0,"DB Err","dbGGA_2Files_Diff: "+dbase.lastError().text(),QMessageBox::Ok);
            return;
    }

    QSqlQuery query(dbase);
    query.prepare("SELECT "
                      "gga.time AS time, "
                      "gga.x,gga.y,gga.type, "
                      "gga1.x AS x1, "
                      "gga1.y AS y1 "
                  "FROM "
                      "gga INNER JOIN gga1 ON gga.time = gga1.time WHERE gga.type=4;"); // todo - задавать тип решения

    if (!query.exec()) {
        qDebug() << "dbGGA_2Files_Diff: SELECT Err - " << query.lastError();
        QMessageBox::critical(0,"SELECT Err","dbGGA_2Files_Diff: "+dbase.lastError().text(),QMessageBox::Ok);
        return;
    }

    // todo - добавить проверку на наличие записей в таблицах
    QSqlRecord rec = query.record();
    double time = 0., x = 0., y=0., x1=0., y1=0.;

    while (query.next()) {
            time = query.value(rec.indexOf("time")).toDouble();
            x = query.value(rec.indexOf("x")).toDouble();
            y = query.value(rec.indexOf("y")).toDouble();
            x1 = query.value(rec.indexOf("x1")).toDouble();
            y1 = query.value(rec.indexOf("y1")).toDouble();

            double x_diff = x-x1;
            double y_diff = y-y1;
            double diff=sqrt(x_diff*x_diff+y_diff*y_diff);

            Time->append(time);
            Diff->append(diff);
    }
}
//----------------------------------------------------------------------------------------------------------------
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
         if(back) // Если поднят флаг - Назад, продолжаем поиск
         {
           if(Start-i>=0) // Если не достигнуто начало
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
// GGA - разности по вектору ошибок от эталонного трека, между текущей точкой и через 900 секунд
// todo - переделать с БД
void func::GGA_2Files_Diff_900(QString fileName1, QString fileName2, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X2, QVector<double> *Y2)
{
    func::GGA_2Files_Diff(fileName1,fileName2,X1,Y1,X2,Y2);

    QVector<double> X, Y;
    int Second_Point=0;
    int Count = X1->count();
    int tempPVT = 1; // Темп решения НЗ. Пока константа, но надо определять по файлу.

    for(int i=0;i<=Count;i++)
    {
        Second_Point=func::Nearest_Time(X1->value(i)+900.,X1,i+900*tempPVT); // Надо еще учитывать темп решения

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
//----------------------------------------------------------------------------------------------------------------
// Статистика по файлу
void func::Statistics(QString fileName, QTextBrowser *textBrowser)
{
    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
        // Очищаем Текст
        textBrowser->setText("");
        //Тест вывод имени файла
        textBrowser->setText("File Name: "+fileName);

        // Связываем переменную с физическим файлом
        QFile inputFile(fileName);

        // Если все ОК то открываем файл
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);

           // Счетчики решений
           int Status_3D=0, Status_3D_Diff=0, Status_Fix=0, Status_Float=0, Status_NA=0;

           int i=0, GGA=0, GGA_Total=0; // Валидных GGA и Всего GGA

           int jump = 0; // Скачки по координатам более 10 м

           int Float2Fix=0, Fix2X=0; // Количество переходов Float->Fix и Fix в любой статус

           QString ver = "";

           QString pStatus="", cStatus="", pLine=""; // Предыдущий и текущий статус, предыдущая строка GGA

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd())
           {
              // Считываем строку
              QString line = in.readLine();

              // Ищем версию
              if(line.contains("VER"))
                  if(!ver.contains(line))
                        ver += line+"\n";

              // Анализ GGA
              if(func::GGA_Check(line))
              {
                  cStatus = line.split(',')[6]; // Текущий статус НЗ

                  // Case Status
                  switch ( cStatus.toInt() ) {
                  case 0:
                    Status_NA++;
                    break;
                  case 1:
                    Status_3D++;
                    break;
                  case 2:
                    Status_3D_Diff++;
                    break;
                  case 3:
                    // ??
                    break;
                  case 4:
                    Status_Fix++;
                    break;
                  case 5:
                    Status_Float++;
                    break;
                  case 6:
                    break;
                  default:
                    Status_NA++;
                    break;
                  }

                  // Переходы Float-Fix
                  if(pStatus=="5" and cStatus=="4")
                      Float2Fix++;

                  // Переходы Fix-X
                  if(pStatus=="4" and cStatus!="4" and cStatus!="0")
                      Fix2X++;

                  if(cStatus=="1" or cStatus=="2" or cStatus=="4" or cStatus=="5")
                  {
                      GGA++;

                      // Скачки по координатам
                      if(pLine!="")
                      {
                          if(func::GGA_2Point_Diff(line,pLine)>4)
                              jump++;
                      }

                      pLine=line;
                  }

                  pStatus = cStatus; // сохраняем как предыдущий статус НЗ

                  GGA_Total++;
              }

              // Анализ RMC
              if(line.contains("RMC") and line.split(',').count()!=13)
              {
                  //rmcFieldError++;
                  // badRMC.append(line);
              }

              i++;
            }

            textBrowser->append("VER: "+ver);
            textBrowser->append("Total Lines: "+func::doubleToString(i));
            textBrowser->append("Total GGA: "+func::doubleToString(GGA_Total));
            textBrowser->append("Valid GGA: "+func::doubleToString(GGA));

            // Количество NA решений
            textBrowser->append("NA count: "+func::doubleToString(Status_NA)+" ("+func::doubleToString((double)Status_NA/(double)GGA*100)+"%)");

            // Количество 3D решений
            textBrowser->append("3D count: "+func::doubleToString(Status_3D)+" ("+func::doubleToString((double)Status_3D/(double)GGA*100)+"%)");

            // Количество 3D Diff
            textBrowser->append("3D Diff count: "+func::doubleToString(Status_3D_Diff)+" ("+func::doubleToString((double)Status_3D_Diff/(double)GGA*100)+"%)");

            // Количество Float
            textBrowser->append("Float count: "+func::doubleToString(Status_Float)+" ("+func::doubleToString((double)Status_Float/(double)GGA*100)+"%)");

            // Количество Fix
            textBrowser->append("Fix count: "+func::doubleToString(Status_Fix)+" ("+func::doubleToString((double)Status_Fix/(double)GGA*100)+"%)");

            // Количество переходов Float-Fix
            textBrowser->append("Float to Fix count: "+func::doubleToString(Float2Fix)+" ("+func::doubleToString((double)Float2Fix/(double)GGA*100)+"%)");

            // Количество переходов Fix-X (любое состояние)
            textBrowser->append("Fix to not Fix count: "+func::doubleToString(Fix2X)+" ("+func::doubleToString((double)Fix2X/(double)GGA*100)+"%)");

            // Количество скачков >10 м
            textBrowser->append("Jump count: "+func::doubleToString(jump)+" ("+func::doubleToString((double)jump/(double)GGA*100)+"%)");

        }
    }
}
//---------------------------------------------------------------
// Расстояние между двумя точками 3D
double func::GGA_2Point_Diff(QString GGA_Line1, QString GGA_Line2)
{
    double X=0, Y=0, Z=0, X0=0, Y0=0, Z0=0;

    func::BLH_to_XYZ(GGA_Line1,X0,Y0,Z0);
    func::BLH_to_XYZ(GGA_Line2,X,Y,Z);

    // Вычисляем расстояние между точками
    double diffXYZ = sqrt((X-X0)*(X-X0)+(Y-Y0)*(Y-Y0)+(Z-Z0)*(Z-Z0));

    return diffXYZ;
}
//----------------------------------------------------------------
// Удаление пробелов
QString func::removeSpaces(QString string)
{
    QString str = string.simplified();
    str.replace( " ", "" );

    return(str);
}
//----------------------------------------------------------------
// Вычисление времени для интервалов более одного дня
double func::timeToSeconds(QString& currentTimeField, double& maxTime, int& day)
{
    double currentTime = func::TimeToSeconds(currentTimeField);

    if(currentTime >= maxTime)
        maxTime = currentTime;
    else
    {
        day++;
        maxTime = 0;
    }

    if(day>=1)
        currentTime += (double)day*86400;

    return(currentTime);
}
//----------------------------------------------------------------
// Настройки графика
void func::qcustomPlotSettings(QCustomPlot* customPlot, bool xTime, bool yTime)
{
    // Доп настройки
    customPlot->xAxis->setScaleType(QCPAxis::stLinear); // Масштабирование
    customPlot->yAxis->setScaleType(QCPAxis::stLinear); // Масштабирование

    // Включаем авто-разбивку по основным
    customPlot->xAxis->setAutoTickStep(true);
    customPlot->yAxis->setAutoTickStep(true);

    //Шкалы - по умолчанию цифровая. Если заданы xTime/yTime - то время
    if(xTime)
    {
        customPlot->xAxis->setDateTimeFormat("hh:mm:ss.z (d)"); // Формат шкалы времени
        customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
        customPlot->xAxis->setAutoTickCount(6);
        customPlot->xAxis->setSubTickCount(5);
    }
    else
    {
        customPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
        customPlot->xAxis->setAutoTicks(true);
        customPlot->xAxis->setAutoTickCount(10);
        customPlot->xAxis->setSubTickCount(4);
    }

    if(yTime)
        customPlot->yAxis->setTickLabelType(QCPAxis::ltDateTime);
    else
        customPlot->yAxis->setTickLabelType(QCPAxis::ltNumber);

    // Авто-разбивка по основным делениям шкалы - примерно 10
    customPlot->yAxis->setAutoTickCount(10);

    // Выключаем авто-разбивку по промежуточным делениям шкалы
    customPlot->xAxis->setAutoSubTicks(false);
    customPlot->yAxis->setAutoSubTicks(false);

    // Устанавливаем разбивку по промежуточным 4 (5 секций |_i_i_i_i_|)
    customPlot->yAxis->setSubTickCount(4);

    customPlot->rescaleAxes(); // Подгоняем размер

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables); // Понять, что это ?
}
//----------------------------------------------------------------------------------------------------------
// Перевод числа в текстовую строку
QString func::doubleToString(double number)
{
    return(QString::number(number,'f',2));
}
//----------------------------------------------------------------------------------------------------------
// RMC Из файла NMEA формирует векторы координат X,Y
// 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5; bf_lf_Rb_Rl - вектор начальных условий
void func::RMC_XY_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5,
                                     QVector<double> *bf_lf_Rb_Rl)
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

               //Координаты начальной точки, коэффициенты
               double b_fix=0., l_fix=0., R_b = 0., R_l = 0.;

               // Временно! Координаты меток
               // QVector<double> TMC_X, TMC_Y;

               // Счетчик времени (решений)
               int i=0;

               // Пока не достигнем конца файла читаем строчки
               while (!in.atEnd()) {
                  // Считываем строку
                  QString line = in.readLine();

                  // Если строка содежит RMC
                  // $GPRMC,042814.20,A,5452.9678093,N,08258.4523483,E,3.29,277.48,250716,0.0,E,  R*2A
                  //    0        1           2       3      4        5 6  7    8     9     10 11  12
                  if(line.contains("RMC")) // Ищем строку RMC
                  {

                      // Разбиваем строку по запятой
                      QStringList nmea = line.split(',');

                      // Если данные не достоверны - пропускаем
                      if(nmea[12].split('*')[0] == "N"){
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

                          if(line.contains("TMC")) // Второй раз проверяем... Надо оптимизировать
                          {
                              //TMC_X.append(X);
                              //TMC_Y.append(Y);

                              // Временное решение. Чтобы отличить от Int ставим статус 3D
                              nmea[6]="1";
                          }

                          // Разбиваем данные по типу решения
                          func::RMC_to_3D_Fix_Float(nmea,X,Y,X1,Y1,X4,Y4,X5,Y5);

                      }

                      // Счетчик для валидных решений
                      i++;

                    }
               }
               inputFile.close();
        }
    }

}
//----------------------------------------------------------------------------------------------------------------
// RMC - Разбивка по типу решения
void func::RMC_to_3D_Fix_Float(QStringList nmea, double X, double Y, QVector<double> *X1, QVector<double> *Y1, QVector<double> *X4, QVector<double> *Y4, QVector<double> *X5, QVector<double> *Y5)
{

    // $GPRMC,042814.20,A,5452.9678093,N,08258.4523483,E,3.29,277.48,250716,0.0,E,  R*2A
    //    0        1           2       3      4        5 6  7    8     9     10 11  12

    QString type = nmea[12].split('*')[0];

    // Разбиваем данные по типу решения
    // Если тип решения RTK Fix
    if(type=="R")
    {
        X4->append(X);
        Y4->append(Y);
    }
    // RTK Float
    else if (type=="F") {
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
// Из файла NMEA формирует массивы времени по типу решения
// 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
void func::PERC_Time_Vectors(QString fileName, QVector<double> *X1, QVector<double> *Y1)
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

           int i=0; // Счетчик времени (решений)

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              //
              //$PERC,GPppr,000002,00000,00050,00,3,0*49
              //   0    1      2     3     4    5 6 7
              if(line.contains("PERC") and func::CRC_Check(line)) // Ищем строку PERC
              {
                  i++;

                  // Разбиваем строку по запятой
                  QStringList nmea = line.split(',');

                  if(nmea.count()<8)
                      continue;

                  // Если данные не достоверны пропускаем
                  if(nmea[6]=="3")
                      continue;

                  QString timeString=nmea[2]; // Поле время

                  // Если время менее 6 символов - пропускаем
                  if(timeString.count()<6)
                      continue;

                  double X = timeString.toDouble();
                  double Y = (double)i;

                  // Добавляем в вектора
                  X1->append(X);
                  Y1->append(Y);

                  // Счетчик валидных решений
                }
           }
           inputFile.close();
        }
    }
}
//----------------------------------------------------------------------------------------------------------------
// Загрузка файла в БД
void func::loadFileToDB(QString fileName)
{
    QSqlDatabase dbase = QSqlDatabase::addDatabase("QSQLITE");
        dbase.setDatabaseName("app_db.sqlite");
        if (!dbase.open()) {
            qDebug() << "Не удается открыть БД";
        }

        QSqlQuery query;

            // todo - добавить проверку на существование таблицы
            query.prepare("DROP TABLE IF EXISTS gga;");

            if (!query.exec()) {
                qDebug() << "DROP TABLE Err";
            }

            query.prepare("CREATE TABLE gga("
                            "id_time INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "time doudle, "
                            "height double"
                            ");");

            if (!query.exec()) {
                qDebug() << "Не удается создать таблицу";
            }

            // Связываем переменную с физическим файлом
            QFile inputFile(fileName);
            // Если все ОК то открываем файл
            if (inputFile.open(QIODevice::ReadOnly))
            {
               QTextStream in(&inputFile);
               // Пока не достигнем конца файла читаем строчки
               while (!in.atEnd()) {
                  // Считываем строку
                  QString line = in.readLine();

                  // Если строка содежит GGA
                  // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
                  //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
                  if(line.contains("GGA"))
                  {

                      // Разбиваем строку по запятой
                      QStringList nmea = line.split(',');

                      // Если решение невалидное - пропускаем
                      if(nmea[6]=="0")
                          continue;

                      // DML
                      query.prepare("INSERT INTO gga(time, height)"
                                "VALUES (:time, :height);");
                      query.bindValue(":time",nmea[1]);
                      query.bindValue(":height",nmea[9]);

                      if (!query.exec()) {
                            qDebug() << "Данные не вставляются";
                      }
                  }
               }
            }

}
//----------------------------------------------------------------------------------------------------------------
// Получение данных из БД
void func::getDataFromDB()
{
    QSqlDatabase dbase = QSqlDatabase::addDatabase("QSQLITE");

    dbase.setDatabaseName("app_db.sqlite");

    if (!dbase.open()) {
            qDebug() << "Не удается подключиться к БД";
        }

    QSqlQuery query;

    query.prepare("SELECT * FROM gga INNER JOIN gga1 ON gga.time = gga1.time");

    if (!query.exec()) {
        qDebug() << "SELECT Err";
    }

    QSqlRecord rec = query.record();
    double time = 0.,
           height = 0.;

    while (query.next()) {
            time = query.value(rec.indexOf("time")).toDouble();
            height = query.value(rec.indexOf("height")).toDouble();

            qDebug() << "time is " << time
                     << ". height is " << height;
    }
}
