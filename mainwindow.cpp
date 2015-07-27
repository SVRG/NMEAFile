/*
Roadmap

Добавить возможность выбирать и отображать данные из нескольких файлов
Разбор склеенных строк?

*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QtMath>
//#include <QDateTime>

// Имя файла, глобальная переменная
QString fileName="";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Открываем файл----------------------------------------------------------------------------------------
void MainWindow::on_actionOpen_File_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
    //Тест вывод имени файла
    if(!fileName.isEmpty())
    {
        ui->textBrowser->setText("File Name: "+fileName);
        MainWindow::on_actionGGA_Position_triggered();
    }
    else
        ui->textBrowser->setText("Please Select NMEA File");

}

// Подгоняем размер под график---------------------------------------------------------------------------
void MainWindow::on_actionScale_triggered()
{
// Проверить! Наверное один раз подогнать ось!
    if(ui->customPlot->graphCount()>0)
    {
    //for(int i=0;i<ui->customPlot->graphCount();i++)
        ui->customPlot->graph(1)->rescaleKeyAxis();
    //ui->customPlot->xAxis->setRange(0, ui->customPlot->xAxis->range().size(), Qt::AlignLeft);
    ui->customPlot->replot();
    }
    else
        ui->textBrowser->setText("Error: Grapph Count = 0. Please select correct NMEA File");
}

// Вывод навигационных параметров. Кол-во спутников в решении -------------------------------------------
void MainWindow::on_actionNAV_Param_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        {
        fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
        }

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Очищаем Текст
    ui->textBrowser->setText("");
    //Тест вывод имени файла
    ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Возраст поправки, Время, Тип решения, Кол-во спутников
       QVector<double> timeLine1, usedSat1, timeLine4, usedSat4, timeLine5, usedSat5;
       //double diffAgeMax=0;
       // Массив значений времени. Доработать - выводится неправильно.
       //QVector<QString> timeLables;

       // Счетчик времени
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>14) // Проверка на количество полей
            {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Необходима проверка на корректность строки, наличии всех полей.
              if(nmea.count()>12)
              {
                  // Кол-во исаользуемых спутников [7]
                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(nmea[6]=="4")
                  {
                      usedSat4.append(QString(nmea[7]).toDouble());
                      timeLine4.append(i);
                  }
                  // RTK Float
                  else if (nmea[6]=="5") {
                      usedSat5.append(QString(nmea[7]).toDouble());
                      timeLine5.append(i);
                  }
                  else
                  // Остальные - 3D/3D Diff/DR ...
                  {
                      usedSat1.append(QString(nmea[7]).toDouble());
                      timeLine1.append(i);
                  }

                //timeLables.append(nmea[1]);
                i++;
              }
            } // if GGA
       }
       inputFile.close();

       if(!usedSat1.isEmpty() || !usedSat4.isEmpty() || !usedSat5.isEmpty())
       {
       // Used Sattelites
       ui->customPlot->clearGraphs();

       ui->customPlot->addGraph();
       ui->customPlot->graph(0)->setData(timeLine1, usedSat1);
       // give the axes some labels:
       ui->customPlot->graph(0)->setPen(QPen(Qt::red));
       ui->customPlot->xAxis->setLabel("Time");
       ui->customPlot->yAxis->setLabel("Value");
       ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
       //ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
       //ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
       ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(0)->setName("3D/... Used Sattelites");

       // Used Sattelites
       ui->customPlot->addGraph();
       ui->customPlot->graph(1)->setData(timeLine4, usedSat4);
       ui->customPlot->graph(1)->setPen(QPen(Qt::green));
       ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(1)->setName("RTK Used Sattelites");

       // Used Sattelites
       ui->customPlot->addGraph();
       ui->customPlot->graph(2)->setData(timeLine5, usedSat5);
       ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
       ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(2)->setName("Float Used Sattelites");

       // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
       ui->customPlot->graph(0)->rescaleAxes();
       //ui->customPlot->graph(1)->rescaleAxes();

       // Note: we could have also just called customPlot->rescaleAxes(); instead
       // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
       ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
       //ui->customPlot->xAxis->setAutoTickLabels(false);
       //ui->customPlot->xAxis->setTickVectorLabels(timeLables);

       ui->customPlot->replot();
       }
       else
           ui->textBrowser->append("No GGA Data");

    }
    }
}

// Показываем / Скрываем легенду-------------------------------------------------------------------------
void MainWindow::on_actionShow_Legend_triggered()
{
    ui->customPlot->legend->setVisible(!ui->customPlot->legend->visible());
    ui->customPlot->replot();
}

// Отлавливаем скачки: GGA Дельта по Координатам. В проекте - скорость----------------------------------------------
void MainWindow::on_actionErrors_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Очищаем Текст
    ui->textBrowser->setText("");
    //Тест вывод имени файла
    ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Массивы расстояний в режимах 3D, RTK, Float
       QVector<double> diffXY1, timeXY1, diffXY4, timeXY4, diffXY5, timeXY5;

       // Массив значений времени. Доработать - выводится неправильно.
       //QVector<QString> timeLables;

       // Переменные для пред. значений
       double X0,Y0;//,time0;

       // Счетчик. Используется для Тестов.
       int i=0;
       // Тест для 10 строчек
       //for(i=0;i<10;i++)
       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>14) // Проверка на количество полей
            {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // BLH->XYZ Строковые и числовые переменные
              double B=0,L=0,H=0, X=0, Y=0;
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
              // Высота над элипсойдом
              H = QString(nmea[9]).toDouble()+QString(nmea[11]).toDouble();

              // Широта/Долгота - Градусы + Минуты. Переводим в радианы.
              B = qDegreesToRadians(B+QString(BStr.mid(2)).toDouble()/60);
              L = qDegreesToRadians(L+QString(LStr.mid(3)).toDouble()/60);

              // Вывод значений в лог
              //ui->textBrowser->append("B="+QString::number(B));
              //ui->textBrowser->append("L="+QString::number(L));

              // Время [1]
              //timeLine.append(QString(nmea[1]).toDouble());
              //ui->textBrowser->append(nmea[1].mid(0,2)+" "+nmea[1].mid(2,2)+" "+nmea[1].mid(4));
              //QDateTime time;
              //time.setTime(QTime::QTime(QString(nmea[1].mid(0,2)).toDouble(),QString(nmea[1].mid(2,2)).toDouble(),QString(nmea[1].mid(4)).toDouble()));
              //time.setDate(QDate::QDate(2015,6,3));
              //time.setTime(QTime::QTime(i,2,3));
              //timeLine.append(time.toTime_t());
              //timeLine.append(i);

              // Вычисление XYZ. Уточнить - значение N можно вычислить 1 раз?
              double N    = 6378137. / sqrt(1. - 0.00669437999 * sin(B) * sin(B));
              X = (N + H) * cos(L) * cos(B);
              Y = (N + H) * sin(L) * cos(B);
              //Z = ((1. - 0.00669437999) * N + H) * sin(B);

              // Вычисляем разницу между текущей и предыдущей координатой
              if(i>0)
              {
               // Вычисляем расстояние между точками
               double diffXY = sqrt((X-X0)*(X-X0)+(Y-Y0)*(Y-Y0));

               // Разбиваем данные по типу решения
               // Если тип решения RTK Fix
               if(nmea[6]=="4")
               {
                    diffXY4.append(diffXY);
                    timeXY4.append(i);
               }
               // RTK Float
               else if (nmea[6]=="5") {
                   diffXY5.append(diffXY);
                   timeXY5.append(i);
               }
               else
               // Остальные - 3D/3D Diff/DR ...
               {
                   diffXY1.append(diffXY);
                   timeXY1.append(i);
               }

              // Разница во времени с предыдущего шага
              //    diffTime.append(time.toTime_t() - time0);
              //ui->textBrowser->append(QString::number(time.toTime_t() - time0));

              // Скорость
              // if(diffTime[i]>0 && time0>0)
              //    speed.append(diffXY[i]/1000/(diffTime[i]/1000/60/60));
              //    ui->textBrowser->append(QString::number(diffXY[i]/1000/(diffTime[i]/1000/60/60)));
              }

              // Сохраняем значения для (в след. итерации будут использоваться как предыдущие значения)
              X0=X; Y0=Y;// time0=time.toTime_t();

            // Значения времени
            //timeLables.append(nmea[1]);

            // Счетчик для индекса значений
            i++;
            }// if GGA

       } //while
       inputFile.close();

       if(diffXY1.count()>0 || diffXY4.count()>0 || diffXY5.count()>0)
       {
       // diffXY Graph
       ui->customPlot->clearGraphs();

       ui->customPlot->addGraph();
       ui->customPlot->graph(0)->setData(timeXY1, diffXY1);
       // give the axes some labels:
       ui->customPlot->graph(0)->setPen(QPen(Qt::red));
       ui->customPlot->xAxis->setLabel("Time");
       ui->customPlot->yAxis->setLabel("Value");
       ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
       ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(0)->setName("3D XY Difference");

       ui->customPlot->addGraph();
       ui->customPlot->graph(1)->setData(timeXY5, diffXY5);
       // give the axes some labels:
       ui->customPlot->graph(1)->setPen(QPen(Qt::blue));
       ui->customPlot->xAxis->setLabel("Time");
       ui->customPlot->yAxis->setLabel("Value");
       ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(1)->setName("Float XY Difference");

       ui->customPlot->addGraph();
       ui->customPlot->graph(2)->setData(timeXY4, diffXY4);
       // give the axes some labels:
       ui->customPlot->graph(2)->setPen(QPen(Qt::green));
       ui->customPlot->xAxis->setLabel("Time");
       ui->customPlot->yAxis->setLabel("Value");
       ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(2)->setName("RTK XY Difference");


       // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
       ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
       // Установка значений времени
       //ui->customPlot->xAxis->setAutoTickLabels(false);
       //ui->customPlot->xAxis->setTickVectorLabels(timeLables);

       ui->customPlot->graph(0)->rescaleAxes();
       ui->customPlot->replot();

       }
       else
           ui->textBrowser->append("No GGA Data");

       }
    else
        ui->textBrowser->append("Open File Error");
    }
    else
        ui->textBrowser->append("File Name is Empty");
}

// Рабзобр RMC. Скорость, Курс-----------------------------------------------------------------------------
void MainWindow::on_actionRMC_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
     // Очищаем Текст
     ui->textBrowser->setText("");
     //Тест вывод имени файла
     ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Массивы: Скорость RTK, время RTK, -//- Float, 3D, Время Курса, значение Курса
       QVector<double> speedR, timeR, speedF, timeF, speedA, timeA, timeCourse, valueCourse;
       // Массив значений времени. Доработать - выводится неправильно.
       //QVector<QString> timeLables;

       // Счетчик. Используется для Тестов.
       int i=0;
       // Тест для 10 строчек
       //for(i=0;i<10;i++)
       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит RMC
          // $GPRMC,084202.200,A,5452.3265793,N,08258.8073766,E,000.0,337.6,290515,  ,  , A, S*11
          //    0      1       2      3       4     5         6   7    8      9    10 11 12 13
          if(line.contains("RMC")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>12) // Проверка на количество полей
            {

              line = line.split("*")[0];
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              //ui->textBrowser->append(line);

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(nmea[12]=="R")
              {
                  speedR.append(QString(nmea[7]).toDouble()*1.8);
                  timeR.append(i);
              }
              // RTK Float
              else if (nmea[12]=="F") {
                  speedF.append(QString(nmea[7]).toDouble()*1.8);
                  timeF.append(i);
              }
              else
              // Остальные - 3D/3D Diff/DR/(n/a) ...
              {
                  speedA.append(QString(nmea[7]).toDouble()*1.8);
                  timeA.append(i);
              }

             // Курс [8]
             double course=QString(nmea[8]).toDouble();
             // Переводим курс в диапазон 0-180. Для непрерывного перехода 360-0 / 0-360
             if(course>180)
                    course=360-course;
             valueCourse.append(course/10);
             timeCourse.append(i);


            // Счетчик для тестов
            i++;
            //timeLables.append(nmea[1]);
            }// if RMC

       } //while
       inputFile.close();

       if(timeA.count()>0 || timeR.count()>0 || timeF.count()>0)
       {
       // diffXY Graph

       // Очищаем данные
       ui->customPlot->clearGraphs();
       ui->customPlot->clearItems();

       // Скорость 3D
       ui->customPlot->addGraph();
       ui->customPlot->graph(0)->setData(timeA, speedA);
       // give the axes some labels:
       ui->customPlot->graph(0)->setPen(QPen(Qt::red));
       ui->customPlot->xAxis->setLabel("Num");
       ui->customPlot->yAxis->setLabel("Value");
       ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
       //ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
       //ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
       ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(0)->setName("Speed 3D/...");

       // Скорость RTK
       ui->customPlot->addGraph();
       ui->customPlot->graph(1)->setData(timeR, speedR);
       ui->customPlot->graph(1)->setPen(QPen(Qt::green));
       ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(1)->setName("Speed RTK");


       // Скорость Float
       ui->customPlot->addGraph();
       ui->customPlot->graph(2)->setData(timeF, speedF);
       ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
       ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(2)->setName("Speed Float");

       // Курс
       ui->customPlot->addGraph();
       ui->customPlot->graph(3)->setData(timeCourse, valueCourse);
       ui->customPlot->graph(3)->setPen(QPen(Qt::black));
       ui->customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(3)->setName("Course");


       // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
       ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
       // Установка значений времени
       //ui->customPlot->xAxis->setAutoTickLabels(false);
       //ui->customPlot->xAxis->setTickVectorLabels(timeLables);

       ui->customPlot->graph(0)->rescaleAxes();
       ui->customPlot->replot();

       }
       else
           ui->textBrowser->append("No RMC Data");

       }
    else
        ui->textBrowser->append("Open File Error");
    }
    else
        ui->textBrowser->append("File Name is Empty");
}

// Масштабирование по Y. Переделать!
void MainWindow::on_actionScale_Y_triggered()
{
    if(ui->customPlot->graphCount()>0)
    {
    for(int i=0;i<=(ui->customPlot->graphCount()-1);i++)
        ui->customPlot->graph(i)->rescaleKeyAxis();
    ui->customPlot->replot();
    }
    else
      ui->textBrowser->setText("Grapph Count = 0");
}

// Вывод трека GGA----------------------------------------------------------------------------------------
void MainWindow::on_actionGGA_Position_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Очищаем Текст
    ui->textBrowser->setText("");
    //Тест вывод имени файла
    ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
       QVector<double> X1, Y1, X4, Y4, X5, Y5;
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
          if(line.contains("GGA")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>14) // Проверка на количество полей
          {

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если данные достоверны
              if(nmea[6]!="0")
              {

              // BLH->XYZ Строковые и числовые переменные
              double B=0,L=0,H=0, X=0, Y=0;
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
              // Высота над элипсойдом
              H = QString(nmea[9]).toDouble()+QString(nmea[11]).toDouble();

              // Широта/Долгота - Градусы + Минуты. Переводим в радианы.
              B = qDegreesToRadians(B+QString(BStr.mid(2)).toDouble()/60);
              L = qDegreesToRadians(L+QString(LStr.mid(3)).toDouble()/60);

              // Вывод значений в лог
              //ui->textBrowser->append("B="+QString::number(B));
              //ui->textBrowser->append("L="+QString::number(L));

              // Вычисление XYZ. Значение N можно вычислить 1 раз?
              /*double N    = 6378137. / sqrt(1. - 0.00669437999 * sin(B) * sin(B));
              X = (N + H) * cos(L) * cos(B);
              Y = (N + H) * sin(L) * cos(B);
              //Z = ((1. - 0.00669437999) * N + H) * sin(B);
              */

              // Вариант трека по приращениям (Storegis)
              // Начальная точка. Вычисляем коэффициенты один раз?
              if(i==1)
              {
              // Константы
              // Эксцентриситет
              double E_2=0.00669437999;

              // Большая полуось
              double Ae= 6378137.;

              // Фиксируем начальные координаты
              b_fix=B;
              l_fix=L;

              // Вычисляем коэффициенты приращения
              double W = sqrt(1 - E_2 * b_fix * b_fix);
              R_b = Ae * (1. - E_2) / (W * W * W) + l_fix;
              R_l = (Ae / W + l_fix) * cos(b_fix);
              }

              // Вычисляем разницу между начальными и текущими координатами и умножаем на коэффициенты
              if(i>0)
              {
              Y=(B-b_fix)*R_b;
              X=(L-l_fix)*R_l;

              // Test
              //X0.append(X);
              //Y0.append(Y);

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(nmea[6]=="4")
              {
                  X4.append(X);
                  Y4.append(Y);
              }
              // RTK Float
              else if (nmea[6]=="5") {
                  X5.append(X);
                  Y5.append(Y);
              }
              else
              // Остальные - 3D/3D Diff/DR ...
              {
                  X1.append(X);
                  Y1.append(Y);
              }
              } // if i>0

              // Счетчик для валидных решений
              i++;
              }// if GGA Valid
              else {
                  notValid++;
              }
            }
       }
       inputFile.close();

       if(X1.count()>0 || X4.count()>0 || X5.count()>0)
       {
       // Выводим статистику
       double Total=0, Fix=0, Float=0;
       Fix = X4.count();
       Float = X5.count();
       Total = Fix + Float + X1.count();
       // Общее количество точек
       ui->textBrowser->append("Total points: "+QString::number(Total));
       // Fix решений
       ui->textBrowser->append("Fix points: "+QString::number(Fix/Total*100)+"% ("+QString::number(Fix)+")");
       // Float решений
       ui->textBrowser->append("Float points: "+QString::number(Float/Total*100)+"% ("+QString::number(Float)+")");
       // Невалидныйх решений
       ui->textBrowser->append("Not Valid points: "+QString::number(notValid));

       ui->customPlot->clearGraphs();
       ui->customPlot->clearItems();
       // Рисуем графики
       // 3D/3D Diff
       // create graph and assign data to it:
       ui->customPlot->addGraph();
       ui->customPlot->graph(0)->setData(X1, Y1);
       // give the axes some labels:
       ui->customPlot->graph(0)->setPen(QPen(Qt::red));
       ui->customPlot->xAxis->setLabel("X");
       ui->customPlot->yAxis->setLabel("Y");
       ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
       ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(0)->setName("3D/3D Diff");

       // RTK Fix
       ui->customPlot->addGraph();
       ui->customPlot->graph(1)->setData(X4, Y4);
       ui->customPlot->graph(1)->setPen(QPen(Qt::green));
       ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(1)->setName("RTK Fix");

       // RTK Float
       ui->customPlot->addGraph();
       ui->customPlot->graph(2)->setData(X5, Y5);
       ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
       ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(2)->setName("RTK Float");

       // Test
       /*ui->customPlot->addGraph();
       ui->customPlot->graph(3)->setData(X0, Y0);
       ui->customPlot->graph(3)->setPen(QPen(Qt::black));
       ui->customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(3)->setName("PR");
       */

       // Подгоняем размер. Вероятно нужно оставить одну строку. Проверить!
       ui->customPlot->graph(0)->rescaleAxes();
       ui->customPlot->graph(1)->rescaleAxes();

       // Note: we could have also just called customPlot->rescaleAxes(); instead
       // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
       ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

       //ui->customPlot->yAxis->setNumberPrecision(0);
       //ui->customPlot->xAxis->setNumberPrecision(0);

       ui->customPlot->yAxis->setAutoTickCount(10);
       ui->customPlot->xAxis->setAutoTickCount(10);

       ui->customPlot->yAxis->setSubTickCount(4);
       ui->customPlot->xAxis->setSubTickCount(4);

       ui->customPlot->yAxis->setAutoSubTicks(false);
       ui->customPlot->xAxis->setAutoSubTicks(false);

       //ui->customPlot->yAxis->setTickStep(10);
       //ui->customPlot->xAxis->setTickStep(10);
       ui->customPlot->replot();
       }
       // Если массивы векторов пустые то сообщаем, что нет GGA данных
       else {
           ui->textBrowser->append("No GGA Data");
       }

    }
    }
}

// Дистанция до Базы и Вычисление статистики ----------------------------------------------------------------------
void MainWindow::on_actionBSS_Distance_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Очищаем Текст
    ui->textBrowser->setText("");
    //Тест вывод имени файла
    ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Массив Расстояний и время
       QVector<double> distanceBSS1, timeLine1, distanceBSS4, timeLine4,distanceBSS5, timeLine5;
       // Массив значений времени. Доработать - выводится неправильно.
       //QVector<QString> timeLables;

       // Счетчик. Используется для Тестов.
       int i=0;

       // Тип решения
       QString type = "";

       // Возраст поправки в GGA
       //QString diffAge = "";

       // Тест для 10 строчек
       //for(i=0;i<10;i++)
       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();


          // Если строка содежит GGA - Запоминаем тип решения
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>14)
              {
              QStringList GGA = line.split(',');
              type = GGA[6];
              //diffAge = line.split(',')[13];

              // Значения времени
              //timeLables.append(GGA[1]);

              continue;
          }// if GGA


          // Если строка содежит BSS
          // $PNVGBSS,82301,4,82297,15.196,1.6*66
          //    0        1  2   3      4    5
          if(line.contains("BSS")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>5) // Проверка на количество полей
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Вывод строки BSS в Лог-окно
              //ui->textBrowser->append(line);

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="4")
              {
              // Фильтр решений по возрасту поправки. Оптимизировать!
                if(QString(nmea[5]).toFloat()<5 && QString(nmea[5]).toFloat()>0){
                  distanceBSS4.append(QString(nmea[4]).toDouble());
                  timeLine4.append(i);
                }
              }
              // RTK Float
              else if (type=="5") {
                  distanceBSS5.append(QString(nmea[4]).toDouble());
                  timeLine5.append(i);
              }
              else
              // Остальные - 3D/3D Diff/DR ...
              {
              // Если расстояние адекватное. Доработать!
              if(QString(nmea[4]).toDouble()<20000)
                  {
                  distanceBSS1.append(QString(nmea[4]).toDouble());
                  timeLine1.append(i);
                  }
              }

              // Счетчик
              i++;

             }// if BSS

       }

       inputFile.close();

       // Если в массиве есть данные
       if(!distanceBSS1.isEmpty() || !distanceBSS4.isEmpty() || !distanceBSS5.isEmpty())
       {
       ui->customPlot->clearGraphs();

       // Вычисляем статистику
       // Сумма значений
       double summDistance=0;
       foreach(double dist, distanceBSS4)
           summDistance+=dist;

       // Среднее арифметическое значение
       double midDistance=summDistance/distanceBSS4.count();
       ui->textBrowser->append("Mid: "+QString::number(midDistance));

       // СКО
       double cep=0;
       foreach (double dist, distanceBSS4) {
           cep+=(dist-midDistance)*(dist-midDistance);
       }
       cep=cep/distanceBSS4.count();
       cep=sqrt(cep);
       ui->textBrowser->append("CEP: "+QString::number(cep));

       // Количество точек в статистике
       ui->textBrowser->append("Stat Points: "+QString::number(distanceBSS4.count()));

       /*
       //Среднее арифметическое с фильтром. Выкидываем все решения, которые отклоняются от среднего на X мм
       summDistance=0.;
       int j=0;
       foreach(double dist, distanceBSS4){
       // Вычисляем разницу
       double diff=dist-midDistance;

       // Если разница меньше нуля, умножаем на -1
       if(diff<0)
           diff=-diff;

       // Если разница меньше 5 мм
       if(diff<0.0025){
           summDistance+=dist;
           j++;
           }
       }

       // Среднее арифметическое значение с фильтром
       midDistance=summDistance/j;
       ui->textBrowser->append("Mid (5мм): "+QString::number(midDistance));
       */

       // Создаем вектор для средней линии
       QVector<double> xMid, yMid;
       yMid.append(midDistance);
       yMid.append(midDistance);
       xMid.append(0);
       xMid.append(i);

       // Рисуем графики
       // Distance 3D/...
       // create graph and assign data to it:
       ui->customPlot->addGraph();
       ui->customPlot->graph(0)->setData(timeLine1, distanceBSS1);
       // give the axes some labels:
       ui->customPlot->graph(0)->setPen(QPen(Qt::red));
       ui->customPlot->xAxis->setLabel("Time");
       ui->customPlot->yAxis->setLabel("m");
       ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
       ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(0)->setName("3D/... Distance");

       // Distance Fix
       ui->customPlot->addGraph();
       ui->customPlot->graph(1)->setData(timeLine4, distanceBSS4);
       // give the axes some labels:
       ui->customPlot->graph(1)->setPen(QPen(Qt::green));
       ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(1)->setName("RTK Distance");

       // Distance Float
       ui->customPlot->addGraph();
       ui->customPlot->graph(2)->setData(timeLine5, distanceBSS5);
       // give the axes some labels:
       ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
       ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(2)->setName("Float Distance");

       // Distance Среднее
       ui->customPlot->addGraph();
       ui->customPlot->graph(3)->setData(xMid, yMid);
       // give the axes some labels:
       ui->customPlot->graph(3)->setPen(QPen(Qt::black));
       ui->customPlot->graph(3)->setLineStyle(QCPGraph::lsLine);
       ui->customPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(3)->setName("RTK Middle");


       // Подгоняем размер. Вероятно нужно оставить одну строку. Проверить!
       ui->customPlot->graph(1)->rescaleAxes();

       // Note: we could have also just called customPlot->rescaleAxes(); instead
       // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
       ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

       // Значения времени
       //ui->customPlot->xAxis->setAutoTickLabels(false);
       //ui->customPlot->xAxis->setTickVectorLabels(timeLables);

       ui->customPlot->replot();
       }
       // Если массивы векторов пустые то сообщаем, что нет GGA данных
       else {
           ui->textBrowser->append("No GGA Data");
       }

    }
    }
}

// Возраст поправки---------------------------------------------------------------------------------------------
void MainWindow::on_actionGGA_Diff_Age_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        {
        fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
        }

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    // Очищаем Текст
    ui->textBrowser->setText("");
    //Тест вывод имени файла
    ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Возраст поправки, Время, Тип решения, Кол-во спутников
       QVector<double> diffAge1, timeLine1, diffAge4, timeLine4, diffAge5, timeLine5;
       //double diffAgeMax=0;
       // Массив значений времени. Доработать - выводится неправильно.
       //QVector<QString> timeLables;

       // Тест для 10 строчек
       int i=0;
       //for(i=0;i<10;i++)
       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();
          // Возраст поправки


          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Добавить проверку с контрольной суммой
          if(line.split(',').count()>14) // Проверка на количество полей
            {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(nmea[6]=="4")
              {
                  diffAge4.append(QString(nmea[13]).toDouble());
                  timeLine4.append(i);
              }
              // RTK Float
              else if (nmea[6]=="5") {
                  diffAge5.append(QString(nmea[13]).toDouble());
                  timeLine5.append(i);
              }
              else
              // Остальные - 3D/3D Diff/DR ...
              {
                  diffAge1.append(QString(nmea[13]).toDouble());
                  timeLine1.append(i);
              }

            i++;

            // Значения времени
            //timeLables.append(nmea[1]);
            } // if GGA
       }
       inputFile.close();

       if(!diffAge1.isEmpty() || !diffAge4.isEmpty() || !diffAge5.isEmpty())
       {
       ui->customPlot->clearGraphs();

       // Diff Age
       ui->customPlot->addGraph();
       ui->customPlot->graph(0)->setData(timeLine1, diffAge1);
       // give the axes some labels:
       ui->customPlot->graph(0)->setPen(QPen(Qt::red));
       ui->customPlot->xAxis->setLabel("Time");
       ui->customPlot->yAxis->setLabel("Value");
       ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
       //ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
       //ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
       ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(0)->setName("3D/... Diff Age");

       // Diff Age
       ui->customPlot->addGraph();
       ui->customPlot->graph(1)->setData(timeLine4, diffAge4);
       ui->customPlot->graph(1)->setPen(QPen(Qt::green));
       ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(1)->setName("RTK Diff Age");

       // Diff Age
       ui->customPlot->addGraph();
       ui->customPlot->graph(2)->setData(timeLine5, diffAge5);
       ui->customPlot->graph(2)->setPen(QPen(Qt::blue));
       ui->customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
       ui->customPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
       ui->customPlot->graph(2)->setName("Float Diff Age");

       // Доработать масштабирование!
       ui->customPlot->graph(0)->rescaleAxes();
       //ui->customPlot->graph(1)->rescaleAxes();

       // Note: we could have also just called customPlot->rescaleAxes(); instead
       // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
       ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

       // Значение времени
       //ui->customPlot->xAxis->setAutoTickLabels(false);
       //ui->customPlot->xAxis->setTickVectorLabels(timeLables);

       ui->customPlot->replot();
       }
       else
           ui->textBrowser->append("No GGA Data");

    }
    }
}

void MainWindow::on_actionY_Logarithmic_triggered()
{
//ui->menuView->
//    if(ui->customPlot->yAxis->AxisTypes.Int != QCPAxis::stLogarithmic)
        ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        ui->customPlot->replot();
//    else
//        ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
}
