/*
Roadmap

Добавить возможность выбирать и одновременно отображать данные из нескольких файлов
Разбор склеенных строк? Когда в одной строке может быть несколько предложений. Либо неправильный перенос строки
Частично сделано - Добавить поиск и классификацию ошибок - CRC / Битые строки / Пропуски / Неправильный перенос...
Дабавить точки с управляющими командами PNVGRTK, RZA?, ...
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
//#include <QtMath> // func.h
//#include <QDateTime> // Пока без времени
#include "func.h"


// Имя файла, глобальная переменная
QString fileName="", graphName="";
QStringList fileNames;

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

// ------------------------------------------------------------------------------------------------------
// Открываем файл----------------------------------------------------------------------------------------
void MainWindow::on_actionOpen_File_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
    if(!fileName.isEmpty())
    {
        ui->textBrowser->setText("File Name: "+fileName);
        // определить какой график был открыт. Может переделать на SWITCH CASE?
        if(graphName=="NAV_Param")
          MainWindow::on_actionNAV_Param_triggered();
        else if(graphName=="Errors")
          MainWindow::on_actionErrors_triggered();
        else if(graphName=="RMC")
          MainWindow::on_actionRMC_triggered();
        else if(graphName=="GGA_Position")
          MainWindow::on_actionGGA_Position_triggered();
        else if(graphName=="BSS_Distanc")
          MainWindow::on_actionBSS_Distance_triggered();
        else if(graphName=="GGA_Diff_Age")
          MainWindow::on_actionGGA_Diff_Age_triggered();
        else if(graphName=="BSS_GGA")
          MainWindow::on_pushButton_clicked();
        else if(graphName=="BLS")
          MainWindow::on_actionBLS_triggered();
        else if(graphName=="BSS_Total_Valid")
          MainWindow::on_actionBSS_Total_Valid_triggered();
        else if(graphName=="HDT_Course")
          MainWindow::on_actionHDT_Course_triggered();
        else if(graphName=="BLS_Course_Difference")
          MainWindow::on_actionBLS_Course_Difference_triggered();
        else if(graphName=="Find_Errors")
          MainWindow::on_actionFind_Errors_triggered();

        //else
        //  MainWindow::on_actionGGA_Position_triggered();

    }
    else
    {
        ui->textBrowser->setText("Please Select NMEA File");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }

}

// ------------------------------------------------------------------------------------------------------
// Подгоняем размер под график---------------------------------------------------------------------------
void MainWindow::on_actionScale_triggered()
{
// Проверить! Наверное один раз подогнать ось!
    if(ui->customPlot->graphCount()>0)
    {
    for(int i=0;i<ui->customPlot->graphCount();i++)
        ui->customPlot->graph(i)->rescaleKeyAxis();
    //ui->customPlot->xAxis->setRange(0, ui->customPlot->xAxis->range().size(), Qt::AlignLeft);
    ui->customPlot->replot();
    }
    else
    {
        ui->textBrowser->setText("Error: Grapph Count = 0. Please select correct NMEA File");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }
}

// ------------------------------------------------------------------------------------------------------
// GGA Вывод навигационных параметров. Кол-во спутников в решении
void MainWindow::on_actionNAV_Param_triggered()
{
    graphName = "NAV_Param";
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
            MainWindow::on_actionOpen_File_triggered();

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
    //Тест вывод имени файла
    ui->textBrowser->setText("File Name: "+fileName);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileName);
    // Если все ОК то открываем файл
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);

       // Время, Кол-во спутников
       QVector<double> timeLine1, usedSat1, timeLine4, usedSat4, timeLine5, usedSat5;
       // Счетчик времени
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Ищем строку GGA
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==15 and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
            {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если решение невалидное - пропускаем
              if(nmea[6]=="0")
                  continue;

                  // Кол-во используемых спутников [7]
                  double Sat = nmea[7].toDouble();
                  double time = func::TimeToSeconds(nmea[1]);

                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(nmea[6]=="4")
                  {
                      usedSat4.append(Sat);
                      timeLine4.append(time);
                  }
                  // RTK Float
                  else if (nmea[6]=="5") {
                      usedSat5.append(Sat);
                      timeLine5.append(time);
                  }
                  else
                  // Остальные - 3D/3D Diff/DR ...
                  {
                      usedSat1.append(Sat);
                      timeLine1.append(time);
                  }

                i++;

              } // END if GGA
       }
       inputFile.close();

       if(!usedSat1.isEmpty() || !usedSat4.isEmpty() || !usedSat5.isEmpty())
       {
       //
       //ui->actionNAV_Param


       // Used Sattelites
       ui->customPlot->clearGraphs();
       // 3D...
       func::drawGraph(ui->customPlot,timeLine1,usedSat1,"Time","Value","3D/... Used Sattelites");
       // Fix
       func::drawGraph(ui->customPlot,timeLine4,usedSat4,"Time","Value","RTK Used Sattelites");
       // Float
       func::drawGraph(ui->customPlot,timeLine5,usedSat5,"Time","Value","Float Used Sattelites");
       }
       else
       {
           ui->textBrowser->append("No GGA Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }
    }
}

// ------------------------------------------------------------------------------------------------------
// Показываем / Скрываем легенду-------------------------------------------------------------------------
void MainWindow::on_actionShow_Legend_triggered()
{
    ui->customPlot->legend->setVisible(!ui->customPlot->legend->visible());
    ui->customPlot->replot();
}

// ------------------------------------------------------------------------------------------------------
// GGA Отлавливаем скачки: Дельта по Координатам. В проекте - скорость-----------------------------------
void MainWindow::on_actionErrors_triggered()
{
    graphName = "Errors";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
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
          if(line.contains("GGA")) // Ищем строку GGA
          if(line.split('*').count()==2) // Проверяем, что есть контрольная сумма
          if(line.split(',').count()==15 and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
            {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если решение невалидное - пропускаем
              if(nmea[6]=="0")
                  continue;


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

               double time = func::TimeToSeconds(nmea[1]);

               // Разбиваем данные по типу решения
               // Если тип решения RTK Fix
               if(nmea[6]=="4")
               {
                    diffXY4.append(diffXY);
                    timeXY4.append(time);
               }
               // RTK Float
               else if (nmea[6]=="5") {
                   diffXY5.append(diffXY);
                   timeXY5.append(time);
               }
               else
               // Остальные - 3D/3D Diff/DR ...
               {
                   diffXY1.append(diffXY);
                   timeXY1.append(time);
               }

              // Разница во времени с предыдущего шага
              //    diffTime.append(time.toTime_t() - time0);
              //ui->textBrowser->append(QString::number(time.toTime_t() - time0));

              // Скорость по приращению координат (dS/dt)
              // if(diffTime[i]>0 && time0>0)
              //    speed.append(diffXY[i]/1000/(diffTime[i]/1000/60/60));
              //    ui->textBrowser->append(QString::number(diffXY[i]/1000/(diffTime[i]/1000/60/60)));
              }

              // Сохраняем значения (в след. итерации будут использоваться как предыдущие значения)
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


       // 3D...
       func::drawGraph(ui->customPlot,timeXY1,diffXY1,"Time","Value","3D XY Difference");
       // Fix
       func::drawGraph(ui->customPlot,timeXY4,diffXY4,"Time","Value","RTK XY Difference");
       // Float
       func::drawGraph(ui->customPlot,timeXY5,diffXY5,"Time","Value","Float XY Difference");
       }
       else
       {
           ui->textBrowser->append("No GGA Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

       }
    else
    {
        ui->textBrowser->append("Open File Error");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }

    }
    else
    {
        ui->textBrowser->append("File Name is Empty");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }
}

// ------------------------------------------------------------------------------------------------------
// RMC Скорость, Курс---------------------------------------------------------------------------
void MainWindow::on_actionRMC_triggered()
{
    graphName = "RMC";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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
       QVector<double> speedR, timeR, speedF, timeF, speedA, timeA, timeCourseR, valueCourseR, timeCourseF, valueCourseF, timeCourseA, valueCourseA;
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
          // $GPRMC,121747.10, A,5543.3054228,N,03755.4196557,E, 0.06, 0.00,310815, 0.0,E, A*3E
          // $GPRMC,084202.200,A,5452.3265793,N,08258.8073766,E,000.0,337.6,290515,  ,  , A, S*11 - Что за S? Это в 5707
          //    0      1       2      3       4     5         6   7    8      9    10 11 12 13
          if(line.contains("RMC")) // Ищем строку RMC
          if(line.split('*').count()==2) // Проверяем, что есть контрольная сумма
          if(line.split(',').count()>=13 and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
            {
              // Отбрасываем контрольную сумму
              line = line.split("*")[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если решение невалидное - пропускаем
              if(nmea[12]=="N" or nmea[2]=="V")
                  continue;


              double speed = nmea[7].toDouble()*1.8; // Скорость
              double time = func::TimeToSeconds(nmea[1]); // Время

              double course=(QString(nmea[8]).toDouble())/10; // Курс [8]

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(nmea[12]=="R")
              {
                  speedR.append(speed);
                  timeR.append(time);

                  valueCourseR.append(course);
                  timeCourseR.append(time);
              }
              // RTK Float
              else if (nmea[12]=="F") {
                  speedF.append(speed);
                  timeF.append(time);

                  valueCourseF.append(course);
                  timeCourseF.append(time);
              }
              else
              // Остальные - 3D/3D Diff/DR/(n/a) ...
              {
                  speedA.append(speed);
                  timeA.append(time);

                  valueCourseA.append(course);
                  timeCourseA.append(time);
              }



            // Счетчик
            i++;
            //timeLables.append(nmea[1]);
            }// if RMC

       } //while
       inputFile.close();

       if(timeA.count()>0 || timeR.count()>0 || timeF.count()>0)
       {
       // Выводим статистику
       func::Stat3D_Fix_Float(ui->textBrowser,&timeA,&timeR,&timeF);

       // Очищаем данные Графика
       ui->customPlot->clearGraphs();

       // 3D...
       func::drawGraph(ui->customPlot,timeA,speedA,"Time","Value","Speed 3D");
       // Fix
       func::drawGraph(ui->customPlot,timeR,speedR,"Time","Value","Speed RTK");
       // Float
       func::drawGraph(ui->customPlot,timeF,speedF,"Time","Value","Speed Float");
       // Курс
       func::drawGraph(ui->customPlot,timeCourseA,valueCourseA,"Time","Value","Course 3D");
       func::drawGraph(ui->customPlot,timeCourseR,valueCourseR,"Time","Value","Course RTK");
       func::drawGraph(ui->customPlot,timeCourseF,valueCourseF,"Time","Value","Course Float");
       }
       else
       {
           ui->textBrowser->append("No RMC Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

       }
    else
    {
        ui->textBrowser->append("Open File Error");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }

    }
    else
    {
        ui->textBrowser->append("File Name is Empty");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }

}

// ------------------------------------------------------------------------------------------------------
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
      {
        ui->textBrowser->setText("Grapph Count = 0");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }

}

// -------------------------------------------------------------------------------------------------------
// Вывод трека GGA----------------------------------------------------------------------------------------
void MainWindow::on_actionGGA_Position_triggered()
{
    graphName = "GGA_Position";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1, X4, Y4, X5, Y5, bf_lf_Rb_Rl;

    // Первый файл
    func::GGA_XY_Vectors(fileName,&X1,&Y1,&X4,&Y4,&X5,&Y5,&bf_lf_Rb_Rl);

    // Выводим статистику
    ui->textBrowser->append("File 1 Stat");
    func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

    // Очищаем и рисуем график файла 1
    ui->customPlot->clearGraphs();
    func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

}

// ------------------------------------------------------------------------------------------------------
// BSS + GGA Дистанция до Базы и Вычисление статистики --------------------------------------------------
void MainWindow::on_actionBSS_Distance_triggered()
{
    graphName = "BSS_Distance";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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
       QVector<double> X1, Y1, X4, Y4, X5, Y5;

       // Счетчик. Используется для Тестов.
       int i=0;           // Счетчик
       double time=-1;    // Время
       QString type = ""; // Тип решения

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит GGA - Запоминаем тип решения и время
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(func::GGA_Check(line))
              {
              QStringList nmea = line.split(',');
              type = nmea[6];

              time = func::TimeToSeconds(nmea[1]);
              continue;
              }// if GGA


          if(type=="" or type=="0" or time<0)
              continue;

          // Если строка содежит BSS
          // $PNVGBSS,82301,4,82297,15.196,1.6*66
          //    0        1  2   3      4    5
          if(line.contains("BSS")) // Ищем строку BSS
          if(line.split('*').count()==2) //  и проверяем, что есть контрольная сумма
          if(line.split(',').count()==6)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');
              double distance = QString(nmea[4]).toDouble();

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="4")
                  {
                      X4.append(time);
                      Y4.append(distance);
                  }
              // RTK Float
              else if (type=="5")
                  {
                      X5.append(time);
                      Y5.append(distance);
                  }
              else
              // Остальные - 3D/3D Diff/DR ...
                  {
                      X1.append(time);
                      Y1.append(distance);
                  }


             i++;  // Счетчик
             type="";
             time=-1;
             }// if BSS

       }

       inputFile.close();

       // Если хотябы в одном массиве есть данные
       if(!X1.isEmpty() || !X4.isEmpty() || !X5.isEmpty())
       {
       // Вычисляем статистику
       double cep; // СКО
       double midDistance; // Среднее
       func::statMID_CEP(Y4,cep,midDistance); // Вычисляем Среднее арифметическое и СКО

       ui->textBrowser->append("Mid: "+QString::number(midDistance));
       ui->textBrowser->append("CEP: "+QString::number(cep));

       // Выводим статистику
       func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

       // Рисуем графики
       ui->customPlot->clearGraphs();
      func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

       // Mid
       func::drawMidGraph(ui->customPlot,midDistance,X4[0],X4[X4.count()-1]);
       }
       // Если массивы векторов пустые то сообщаем, что нет GGA данных
       else
       {
           ui->textBrowser->append("No BSS Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }// if inputFile
    else
    {
        ui->textBrowser->append("Please select a file");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }
    }
}

// ------------------------------------------------------------------------------------------------------
// GGA Возраст поправки
void MainWindow::on_actionGGA_Diff_Age_triggered()
{
    graphName = "GGA_Diff_Age";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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
       QVector<double> X1, Y1, X4, Y4, X5, Y5;
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();
          // Возраст поправки

          // Если строка содежит GGA
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Ищем строку GGA
          if(line.split('*').count()==2) // и проверяем, что есть контрольная сумма
          if(line.split(',').count()==15)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
            {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если решение невалидное - пропускаем
              if(nmea[6]=="0")
                  continue;

              double diffAge = nmea[13].toDouble();
              double time = func::TimeToSeconds(nmea[1]);

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(nmea[6]=="4")
              {
                  Y4.append(diffAge);
                  X4.append(time);
              }
              // RTK Float
              else if (nmea[6]=="5") {
                  Y5.append(diffAge);
                  X5.append(time);
              }
              else
              // Остальные - 3D/3D Diff/DR ...
              {
                  Y1.append(diffAge);
                  X1.append(time);
              }

            i++;

            } // if GGA
       }
       inputFile.close();

       if(!X1.isEmpty() || !X4.isEmpty() || !X5.isEmpty())
       {
       ui->customPlot->clearGraphs();

       func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
       }
       else
       {
           ui->textBrowser->append("No GGA Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }
    }
}

// ------------------------------------------------------------------------------------------------------
// Логарифмическая шкала Y
void MainWindow::on_actionY_Logarithmic_triggered()
{
//ui->menuView->
//    if(ui->customPlot->yAxis->AxisTypes.Int != QCPAxis::stLogarithmic)
        ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        ui->customPlot->replot();
//    else
//        ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
}


// ------------------------------------------------------------------------------------------------------
// BSS+GGA Поиск предложений где значения отличаются на 10mm + 1ppm -------------------------------------
void MainWindow::on_pushButton_clicked() //!< Заменить название
{
    graphName = "BSS_GGA";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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

       // Счетчик, количество "Странных" строк
       int i=0, cnt=0;

       // Эталонное значение с чем сравнивать при поиске "Странностей"
       double referenceValue = ui->referenceValue->text().toDouble();
       double referenceValue_Diff=pow(0.01+referenceValue*0.000001,2.);// Вычисляем квадрат значения 1cm +1ppm
       ui->textBrowser->append("Ref Value Diff: "+QString::number(sqrt(referenceValue_Diff))+"m");


       // Тип решени, Строка GGA для вывода в логе
       QString type = "";//, lineGGA="";

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();


          // Если строка содежит GGA - Запоминаем тип решения
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA")) // Ищем строку GGA
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==15)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
              {
              QStringList GGA = line.split(',');
              type = GGA[6];
              //lineGGA = line;
              continue;
          }// if GGA


          // Если строка содежит BSS
          // $PNVGBSS,82301,4,82297,15.196,1.6*66
          //    0        1  2   3      4    5
          if(line.contains("BSS")) // Ищем строку BSS
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==6)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Вывод строки BSS в Лог-окно
              //ui->textBrowser->append(line);

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="4")
              {
              // Проверяем насколько отличается от Эталона
              double diff = nmea[4].toDouble()-referenceValue;
                  if(pow(diff,2) > referenceValue_Diff)
                    {
                      //ui->textBrowser->append("BSS: "+nmea[4]+" Diff="+QString::number(diff));
                      //ui->textBrowser->append("GGA: "+lineGGA);
                      cnt++;
                    }
              // Сбрасываем тип
              type="";
              }
              // RTK Float
              else if (type=="5") {
              }
              else
              // Остальные - 3D/3D Diff/DR ...
              {

              }

              // Счетчик
              i++;

             }// if BSS

       }
       // Закрываем файл
       inputFile.close();

       ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat

       // Выводим общее количество точек
       ui->textBrowser->append("Total: "+QString::number(i));

       // Выводим точки в статистике и процент
       ui->textBrowser->append("Cnt: "+QString::number(cnt)+" ("+QString::number(cnt/(double)i*100)+"%)");
    }// inputFile.open
    }// !fileName.isEmpty()
}

// ------------------------------------------------------------------------------------------------------
// BLS - Базовая линия. Азимут и Статистика -------------------------------------------------------------
void MainWindow::on_actionBLS_triggered()
{
    graphName = "BLS";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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

       // Массив Расстояний и время(номер предложения)
       // QVector<double> distanceBLSA, timeLineA, distanceBLSR, timeLineR, distanceBLSF, timeLineF;

       // Массив Азимутов и время(номер предложения)
       QVector<double> X1, Y1, X4, Y4, X5, Y5, baseLine, timeLine;

       // Счетчик.
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит BLS
          // $PNVGBLS,-0.898,0.438,0.020,0.999,154.01,1.14,R*09
          //    0        1     2     3     4      5    6   7
          if(line.contains("BLS")) //Ищем строку BLS
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==8)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Тип решения
              QString type = nmea[7];

              // Курс
              double course = QString(nmea[5]).toDouble();

              // Добавляем длину базовой линии
              baseLine.append(QString(nmea[4]).toDouble());
              timeLine.append(i);

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="R")
                  {
                      Y4.append(course);
                      X4.append(i);
                  }
              // RTK Float
              else if (type=="F")
                  {
                      Y5.append(course);
                      X5.append(i);
                  }
              else
              // Остальные - 3D/3D Diff/DR/ n/a ...
                  {
                      Y1.append(course);
                      X1.append(i);
                  }

              // Счетчик
              i++;

             }// if BLS

       }

       inputFile.close();

       // Если в массиве есть данные
       if(!X4.isEmpty() || !X5.isEmpty() || !X1.isEmpty())
       {
       // Вычисляем статистику

       double mid=0; // Среднее арифметическое значение
       double cep=0; // СКО

       func::statMID_CEP(Y4,cep,mid); // Вычисляем среднее и СКО
       ui->textBrowser->append("Mid: "+QString::number(mid));
       ui->textBrowser->append("CEP: "+QString::number(cep));// СКО


       // Выводим статистику
       func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

       // Рисуем графики
       ui->customPlot->clearGraphs();
       func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

       // Длина базовой линии
       func::drawGraph(ui->customPlot,timeLine,baseLine,"Time","Value","Baseline length");
       // Среднее
       func::drawMidGraph(ui->customPlot,mid,0.,(double)i);
       }
       // Если массивы векторов пустые то сообщаем, что нет BLS данных
       else
       {
           ui->textBrowser->append("No BLS Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }
    }
}

// ------------------------------------------------------------------------------------------------------
// BSS График счетчика валидных и всех значений
void MainWindow::on_actionBSS_Total_Valid_triggered()
{
    graphName = "BSS_Total_Valid";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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

       // Массив Значений счетчика валидных предложений и время
       QVector<double> X1, Y1, X4, Y4, X5, Y5, diffAge, timeLineDA;
       // Массив значений времени. Доработать - выводится неправильно.
       //QVector<QString> timeLables;

       // Счетчик. Используется для Тестов.
       int i=0;

       // Тип решения
       QString type = "";

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          double time=0.;


          // Если строка содежит GGA - Запоминаем тип решения
          // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
          //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
          if(line.contains("GGA") and line.split('*').count()==2) // Ищем строку GGA и проверяем, что есть контрольная сумма
          if(line.split(',').count()==15)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
              {
                QStringList nmea = line.split(',');
                type = nmea[6];
                //diffAge = line.split(',')[13];
                time = func::TimeToSeconds(nmea[1]);

                continue;
              }// if GGA


          // Если строка содежит BSS
          // $PNVGBSS,82301,4,82297,15.196,1.6*66
          //    0        1  2   3      4    5
          if(line.contains("BSS") and line.split('*').count()==2) // Ищем строку BSS и проверяем, что есть контрольная сумма
          if(line.split(',').count()==6 and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              double diffAge_Val = nmea[5].toDouble();


              // Если нет значения то не добавляем.
              if(nmea[5]!="")
              {
                  diffAge.append(diffAge_Val);
                  timeLineDA.append(time);
              }

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="4")
              {
                  Y4.append(diffAge_Val);
                  X4.append(time);
              }
              // RTK Float
              else if (type=="5") {
                  Y5.append(diffAge_Val);
                  X5.append(time);
              }
              else
              // Остальные - 3D/3D Diff/DR ...
              {
              // Если расстояние адекватное. Доработать!
                  Y1.append(diffAge_Val);
                  X1.append(time);
              }

              // Счетчик
              i++;

             }// if BSS

       }

       inputFile.close();

       // Если в массиве есть данные
       if(!X1.isEmpty() || !X4.isEmpty() || !X5.isEmpty())
       {
       ui->customPlot->clearGraphs();

       // Рисуем графики
       func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

       // Длина базовой линии
       func::drawGraph(ui->customPlot,timeLineDA,diffAge,"Time","Value","Diff Age");
       }
       // Если массивы векторов пустые то сообщаем, что нет GGA данных
       else
       {
           ui->textBrowser->append("No BSS Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }
    }
}


// ------------------------------------------------------------------------------------------------------
// HDT - Азимут и статистика ---------------------------------------------------------------
void MainWindow::on_actionHDT_Course_triggered()
{
    graphName = "HDT_Course";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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

       // Массив Расстояний и время(номер предложения)
       // QVector<double> distanceBLSA, timeLineA, distanceBLSR, timeLineR, distanceBLSF, timeLineF;

       // Массив Азимутов и время(номер предложения)
       QVector<double> courseHDT, timeLine;

       // Счетчик.
       int i=0;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит HDT
          // $GPHDT,153.89,T*03
          //    0      1   2
          if(line.contains("HDT") and line.split('*').count()==2) // Ищем строку HDT и проверяем, что есть контрольная сумма
          if(line.split(',').count()==3)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

                  courseHDT.append(QString(nmea[1]).toDouble());
                  timeLine.append(i);

              // Счетчик
              i++;

             }// if BLS

       }

       inputFile.close();

       // Если в массиве есть данные
       // if(!distanceBLSR.isEmpty() || !distanceBLSF.isEmpty() || !distanceBLSA.isEmpty())
       if(!courseHDT.isEmpty())
       {
       // Вычисляем статистику

       double mid=0; // Среднее арифметическое значение
       double cep=0; // СКО

       func::statMID_CEP(courseHDT,cep,mid);

       ui->textBrowser->append("Mid: "+QString::number(mid));
       ui->textBrowser->append("CEP: "+QString::number(cep));


       // Выводим статистику
       int Total = courseHDT.count();
       // Общее количество точек
       ui->textBrowser->append("Total points: "+QString::number(Total));

       // Рисуем графики
       ui->customPlot->clearGraphs();
       // 3D...
       func::drawGraph(ui->customPlot,timeLine,courseHDT,"Time","Value","3D/... Course");
       // Среднее
       func::drawMidGraph(ui->customPlot,mid,0.,(double)i);
       }
       // Если массивы векторов пустые то сообщаем, что нет BLS данных
       else
       {
           ui->textBrowser->append("No HDT Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }
    }
}
//------------------------------------------------------------------------------------------------------------
// BLS - разница между двумя значениями курса
void MainWindow::on_actionBLS_Course_Difference_triggered()
{
    graphName="BLS_Course_Difference";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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

       // Массив Расстояний и время(номер предложения)
       // QVector<double> distanceBLSA, timeLineA, distanceBLSR, timeLineR, distanceBLSF, timeLineF;

       // Массив Азимутов и время(номер предложения)
       QVector<double> X1, Y1, X4, Y4, X5, Y5;

       // Счетчик.
       int i=0;

       // Начальный курс
       double cource0=0.;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит BLS
          // $PNVGBLS,-0.898,0.438,0.020,0.999,154.01,1.14,R*09
          //    0        1     2     3     4      5    6   7
          if(line.contains("BLS")) //Ищем строку BLS
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==8)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Тип решения
              QString type = nmea[7];

              // Курс
              double course = QString(nmea[5]).toDouble();
              // - переводим в 0-180 для удаления обрывов 0->360 и 360->0
              //if(course>180)
              //      course=360-course;

              if(i>0)
              {
              // Модуль разницы. При переходах 0-360 будут большие значения! Подумать.
              double diff = sqrt((cource0-course)*(cource0-course));

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="R")
              {
               //   distanceBSSR.append(QString(nmea[4]).toDouble());
                  Y4.append(diff);
                  X4.append(i);
              }
              // RTK Float
              else if (type=="F") {
                  //   distanceBSSR.append(QString(nmea[4]).toDouble());
                  Y5.append(diff);
                  X5.append(i);
              }
              else
              // Остальные - 3D/3D Diff/DR/ n/a ...
              {
                  //   distanceBLSR.append(QString(nmea[4]).toDouble());
                  Y1.append(diff);
                  X1.append(i);
              }
              }// END IF i>0
              cource0 = course;
              // Счетчик
              i++;

             }// if BLS

       }

       inputFile.close();

       // Если в массиве есть данные
       // if(!distanceBLSR.isEmpty() || !distanceBLSF.isEmpty() || !distanceBLSA.isEmpty())
       if(!X4.isEmpty() || !X5.isEmpty() || !X1.isEmpty())
       {

       // Вычисляем статистику
       double mid=0; // Среднее арифметическое значение
       double cep=0; // СКО

       func::statMID_CEP(Y4,cep,mid); // Вычисляем среднее и СКО
       ui->textBrowser->append("Mid: "+QString::number(mid));
       ui->textBrowser->append("CEP: "+QString::number(cep));// СКО


       // Выводим статистику
       ui->textBrowser->append("File 1 Stat");
       func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

       // Рисуем графики
       ui->customPlot->clearGraphs();
       func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

       }
       // Если массивы векторов пустые то сообщаем, что нет BLS данных
       else
       {
           ui->textBrowser->append("No BLS Data");
           ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
       }

    }
    }
}

//  --------------------------------------------------------------------------------------
// Подгоняем размер
void MainWindow::on_actionScale_XY_triggered()
{
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}

//  --------------------------------------------------------------------------------------
// Ищем ошибки
void MainWindow::on_actionFind_Errors_triggered()
{
    graphName="Find_Errors";


    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

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

       // Счетчики Общее кол-во строк, нет КС, КС не соответсвует строке, кол-во символов в КС не 2,
       // Не соотвт. кол-ва полей
       int i=0, noCRC=0, badCRC=0, badCountCRC=0, ggaFieldError=0, rmcFieldError=0, Hz=0;

       QStringList badRMC;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          //if(i==1)
          //  {
          //    Hz=func::GGA_Temp(&in);
          //  }

          // Нет контрольной суммы
          if(!line.contains("*"))
              noCRC++;
          // Контрольная сумма не равна 2-м символам
          else if(line.split('*')[1].count()!=2)
              badCountCRC++;
          // Не совпадает контрольная сумма
          else if(line.split('*')[1]!=func::CRC(line))
              badCRC++;

          // Больше одного $

          // Не совпадает количество полей
          if(line.contains("GGA") and line.split(',').count()!=15)
              ggaFieldError++;

          // Не совпадает количество полей
          if(line.contains("RMC") and line.split(',').count()!=13)
          {
              rmcFieldError++;
              badRMC.append(line);
          }

       i++;
       }

    ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat

    ui->textBrowser->append("Total Lines: "+QString::number(i));

    ui->textBrowser->append("No CRC: "+QString::number(noCRC));
    ui->textBrowser->append("Bad CRC: "+QString::number(badCRC));
    ui->textBrowser->append("Bad Count CRC (Not 2 Digits): "+QString::number(badCountCRC));

    // Проверка полей
    ui->textBrowser->append("GGA Fields (Not 15): "+QString::number(ggaFieldError));

    ui->textBrowser->append("RMC Fields (Not 13): "+QString::number(rmcFieldError)); // 5707 - добавлен признак S-Safe (евро). Нужна коррекция
    foreach (QString row, badRMC)
        ui->textBrowser->append(row);

    ui->textBrowser->append("Temp: "+QString::number(Hz));
    }
    }
}

//  --------------------------------------------------------------------------------------
// Открыть второй файл для сравнения с первым
void MainWindow::on_actionOpen_File_2_triggered()
{
//    fileName2 = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
    fileNames = QFileDialog::getOpenFileNames(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
//    fileName = fileNames[0];
//    fileName2 = fileNames[1];
}

//  --------------------------------------------------------------------------------------
// GGA трек двух файлов
void MainWindow::on_actionGGA_Position_1_2_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();
    // Если fileName пустой - открыть Диалог.
    if(fileNames.count()==0)
        MainWindow::on_actionOpen_File_2_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5; bf_lf_Rb_Rl - начальные координаты и коэффициенты
    QVector<double> X1, Y1, X4, Y4, X5, Y5, bf_lf_Rb_Rl;

    // Первый файл--------------------------------------------------
    func::GGA_XY_Vectors(fileName,&X1,&Y1,&X4,&Y4,&X5,&Y5,&bf_lf_Rb_Rl);

    // Выводим статистику
    ui->textBrowser->append("File 1 Stat");
    func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

    // Очищаем и рисуем график файла 1
    ui->customPlot->clearGraphs();
    func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

    int filesCount = fileNames.count();

    for(int i=0;i<filesCount;i++)
    {
        // Очищаем векторы
        X1.clear(); Y1.clear(); X4.clear(); Y4.clear(); X5.clear(); Y5.clear();

        // Второй файл--------------------------------------------------
        func::GGA_XY_0_Vectors(fileNames[i],&X1,&Y1,&X4,&Y4,&X5,&Y5,bf_lf_Rb_Rl[0],bf_lf_Rb_Rl[1],bf_lf_Rb_Rl[2],bf_lf_Rb_Rl[3]);

        // Выводим статистику
        ui->textBrowser->append("File "+ QString::number(i+2) + " Stat");
        ui->textBrowser->append("File: "+ fileNames[i]);
        func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

        // Рисуем график файла 2
        func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
    }
}

//  --------------------------------------------------------------------------------------
// GGA проверка нет ли разрывов во времени
//
void MainWindow::on_actionGGA_Time_Check_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1, X4, Y4, X5, Y5;

    // Собираем векторы
    func::GGA_Time_Vectors(fileName,&X1,&Y1,&X4,&Y4,&X5,&Y5);

    // Выводим статистику
    ui->textBrowser->append("File 1 Stat");
    func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

    // Очищаем и рисуем графики
    ui->customPlot->clearGraphs();
    func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
}

//---------------------------------------------------------------------------------------------
// GGA Высота
void MainWindow::on_actionGGA_Altitude_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1, X4, Y4, X5, Y5;

    // Собираем векторы
    func::GGA_Altitude_Vectors(fileName,&X1,&Y1,&X4,&Y4,&X5,&Y5);

    // Выводим статистику
    func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

    // Очищаем и рисуем графики
    ui->customPlot->clearGraphs();
    func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
}

//---------------------------------------------------------------------------------------------
// GGA Разница по координатам в плоскости
void MainWindow::on_actionGGA_Position_Difference_12_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();
    // Если fileName пустой - открыть Диалог.
    if(fileNames.count()==0)
        MainWindow::on_actionOpen_File_2_triggered();

    ui->customPlot->clearGraphs();
    // Массивы координат
    QVector<double> X1, Y1, X2, Y2;

    int filesCount = fileNames.count(); // Количество файлов для сравнения

    // Выполняем вычисления по всем файлам
    for(int i=0;i<filesCount;i++)
    {
        func::GGA_2Files_Diff(fileName,fileNames[i],&X1,&Y1,&X2,&Y2); // Переделать, чтобы file1 читать один раз

        func::drawGraph(ui->customPlot,X1,Y1,"Time","Diff, m",fileNames[i]);

        X1.clear(); Y1.clear(); X2.clear(); Y2.clear();
    }
}
//---------------------------------------------------------------------------------------------
// GGA Разница по координатам в плоскости
void MainWindow::on_actionGGA_900_sec_Diff_1_2_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();
    // Если fileName пустой - открыть Диалог.
    if(fileNames.count()==0)
        MainWindow::on_actionOpen_File_2_triggered();

    // Очищаем и рисуем график файла 1
    ui->customPlot->clearGraphs();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1, X2, Y2;

    int filesCount = fileNames.count(); // Количество файлов для сравнения

    // Выполняем вычисления по всем файлам
    for(int i=0;i<filesCount;i++)
    {
        func::GGA_2Files_Diff_900(fileName,fileNames[i],&X1,&Y1,&X2,&Y2);

        func::drawGraph(ui->customPlot,X1,Y1,"Time","Diff, m",fileNames[i]);

        X1.clear(); Y1.clear(); X2.clear(); Y2.clear();
    }
}
//---------------------------------------------------------------------------------------------
// CSV - запись данных в файл CSV
void MainWindow::on_actionCreate_CSV_triggered()
{

    // Если fileName пустой - открыть Диалог. Файл откуда читаем
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Файл - куда запишем данные, общее название для всех типов
    QString filenameCSV = fileName;// QFileDialog::getSaveFileName(this, "DialogTitle", fileName, "CSV files (.csv);;All Files (*)", 0, 0);

    // Добавляем окончания для каждого предложения
    QString filenameCSV_GGA = filenameCSV+"_GGA.csv";

    QString filenameCSV_RMC = filenameCSV+"_RMC.csv";

    QString filenameCSV_IMU = filenameCSV+"_IMU.csv";

    QFile file_GGA(filenameCSV_GGA);

    QFile file_RMC(filenameCSV_RMC);

    QFile file_IMU(filenameCSV_IMU);

    // Если файл не открылся то выходим
    if (!file_GGA.open(QFile::WriteOnly|QFile::Truncate))
        return;

    if (!file_RMC.open(QFile::WriteOnly|QFile::Truncate))
        return;

    if (!file_IMU.open(QFile::WriteOnly|QFile::Truncate))
        return;


    // Файлы данных: GGA / RMC / IMU
    QTextStream stream_GGA(&file_GGA), stream_RMC(&file_RMC), stream_IMU(&file_IMU);

    // Формируем заголовки
    stream_GGA
            << "Time_Seconds" << ","
            << "Time" << ","
            << "Lat" << ","
            << "NS" << ","
            << "Lon" << ","
            << "EW" << ","
            << "Mode" << ","
            << "Num_Sat" << ","
            << "HDOP" << ","
            << "Altitude" << ","
            << "M_Alt" << ","
            << "Geoidal_Sep" << ","
            << "M_GS" << ","
            << "Age" << ","
            << "Base_ID" << ","
            << "X" << ","
            << "Y" << ","
            << "Z" << ","
            << "TMC"
            << "\n";

    stream_RMC
            << "Time_Seconds" << ","
            << "Time" << ","
            << "Status" << ","
            << "Lat" << ","
            << "NS" << ","
            << "Lon" << ","
            << "EW" << ","
            << "SoG" << ","
            << "CoG" << ","
            << "Date" << ","
            << "Magnetic" << ","
            << "Magnetic_EW" << ","
            << "Mode"
            << "\n";

    stream_IMU
            << "Time_Seconds" << ","
            << "Time" << ","
            << "Roll" << ","
            << "Pitch" << ","
            << "Yaw" << ","
            << "an" << ","
            << "ae" << ","
            << "ad"
            << "\n";

    // Если имя не пустое то загружаем содержимое
    if (!fileName.isEmpty())
    {
        // Связываем переменную с физическим файлом
        QFile inputFile(fileName);
        // Если все ОК то открываем файл
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);

           QString Time = ""; // Время для предложений без метки времени
           int GGA=0, RMC=0, IMU=0;

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              if(!line.contains("*"))
                  continue;

              if(!(line.split("*").count()==2))
                  continue;

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Если строка содежит GGA
              // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
              //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
              if((line.contains("GGA") or line.contains("TMC")) and (nmea.count()==15)) // Ищем строку GGA
              {
                    // Если данные не достоверны - пропускаем
                    if(nmea[6]=="0")
                        continue;

                    if(line.contains("GGA") and !func::GGA_Check(line))
                        continue;

                    int TMC=0;
                    if(line.contains("TMC"))
                        TMC=1;


                    double X=0.,Y=0.,Z=0.;

                    func::BLH_to_XYZ(line,X,Y,Z);

                    // Пишем данные в файл
                    stream_GGA
                            << func::TimeToSeconds(nmea[1]) << ","  // Time in sec
                            << nmea[1] << ","                       // Time
                            << nmea[2] << ","                       // Lat
                            << nmea[3] << ","                       // NS
                            << nmea[4] << ","                       // Lon
                            << nmea[5] << ","                       // EW
                            << nmea[6] << ","                       // Mode
                            << nmea[7] << ","                       // Num_Sat
                            << nmea[8] << ","                       // HDOP
                            << nmea[9] << ","                       // Altitude
                            << nmea[10] << ","                      // M
                            << nmea[11] << ","                      // Geoidal_Sep
                            << nmea[12] << ","                      // M_GS
                            << nmea[13] << ","                      // Age
                            << nmea[14].split('*')[0]  << ","       // Base_ID
                            << QString::number(X,'f',3) << ","      // X,Y,Z
                            << QString::number(Y,'f',3) << ","      //
                            << QString::number(Z,'f',3) << ","      //
                            << TMC                                  // TMC
                            << "\n";
                   GGA++;
                   Time = nmea[1];
              }

              // $GPRMC,121747.10, A,5543.3054228,N,03755.4196557,E, 0.06, 0.00,310815, 0.0,E, A*3E
              //    0      1       2      3       4     5         6   7    8      9      10 11 12
              if(line.contains("RMC") and (nmea.count()==13))
              {
                  // Пишем данные в файл
                  stream_RMC
                         << func::TimeToSeconds(nmea[1]) << ","     // Time in sec
                            << nmea[1] << ","                       // Time
                            << nmea[2] << ","                       // Status
                            << nmea[3] << ","                       // Lat
                            << nmea[4] << ","                       // N/S
                            << nmea[5] << ","                       // Lon
                            << nmea[6] << ","                       // E/W
                            << nmea[7] << ","                       // SoG
                            << nmea[8] << ","                       // CoG
                            << nmea[9] << ","                       // Date
                            << nmea[10] << ","                      // Magnetic
                            << nmea[11] << ","                      // Magnetic_EW
                            << nmea[12].split('*')[0]               // Mode. Отбрасываем контрольную сумму после *
                            << "\n";
                    RMC++;
                    Time = nmea[1];
              }


              // $PNVGIMU,-81.86,-78.86,104.99,-0.039,-0.096,0.245*6B
              //      0     1       2     3     4       5     6
              if(line.contains("IMU"))
              {
                    // Пишем данные в файл
                    stream_IMU
                            << func::TimeToSeconds(Time) << ","     // Time in sec
                            << Time << ","                          // Time
                            << nmea[1] << ","                       // Roll
                            << nmea[2] << ","                       // Pitch
                            << nmea[3] << ","                       // Yaw
                            << nmea[4] << ","                       // an
                            << nmea[5] << ","                       // ae
                            << nmea[6].split('*')[0]                // ad
                            << "\n";
                    IMU++;
              }
           }
        }
    }
    // Закрываем файл
    file_GGA.close();
    file_RMC.close();
    file_IMU.close();

}
