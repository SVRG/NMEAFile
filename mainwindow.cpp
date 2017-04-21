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
#include "func.h"

//------------------------------------------------------------------------------------------------------
// Глобальные переменные
QString fileName="", graphName=""; // Путь к файлу, Название графика

QStringList fileNames; // Список файлов

double greferenceValue = 0.; // Эталонное значение для сравнения и статистики

//QString referencePointGGA = "$GPGGA,040148.40,5544.5523183,N,03731.3598778,E,4,14,0.7,174.288,M,14.760,M,0.4,0017*40"; // Эталонная строка координат Кабель 1. Контрольная сумма - неправильная
QString referencePointGGA =  "$GPGGA,190747.00,5544.5518312,N,03731.3602986,E,4,15,0.7,174.286,M,14.760,M,1.0,0017*4A"; // Эталонная строка координат Кабель 2

QString fileTypes = "NMEA LOG-Files (*.nme *.log *.txt *.gpx);;All Files (*)"; // Типы файлов

//------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    // todo - очистить таблицы?
    delete ui;
}

// ------------------------------------------------------------------------------------------------------
// Открываем файл----------------------------------------------------------------------------------------
void MainWindow::on_actionOpen_File_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), fileTypes);
    if(!fileName.isEmpty())
    {
        /*
         * Если был открыт график то при загрузке файла надо запустить процедуру повторно
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
        //else if(graphName=="BSS_GGA")
          //MainWindow::on_pushButton_clicked();
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
        else
            ui->customPlot->clearGraphs();

        //else
        //  MainWindow::on_actionGGA_Position_triggered();
        */
        ui->customPlot->clearGraphs(); // Очищаем графики
        fileNames.clear(); // Очищаем файлы
        return;

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
    if(ui->customPlot->graphCount()>0)
    {
        ui->customPlot->graph(0)->rescaleKeyAxis();

        for(int i=1;i<ui->customPlot->graphCount();i++)
            ui->customPlot->graph(i)->rescaleKeyAxis(true); // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):

        ui->customPlot->replot();
    }
    else
    {
        ui->textBrowser->setText("Error: Grapph Count = 0. Please select correct NMEA File");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }
}
// ------------------------------------------------------------------------------------------------------
// Масштабирование по Y
void MainWindow::on_actionScale_Y_triggered()
{
    if(ui->customPlot->graphCount()>0)
    {
        ui->customPlot->graph(0)->rescaleValueAxis();

        for(int i=1;i<ui->customPlot->graphCount();i++)
            ui->customPlot->graph(i)->rescaleValueAxis(true); // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):

        ui->customPlot->replot();
    }
    else
    {
        ui->textBrowser->setText("Error: Grapph Count = 0. Please select correct NMEA File");
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
    }

}

// ------------------------------------------------------------------------------------------------------
// Логарифмическая шкала Y
void MainWindow::on_actionY_Logarithmic_triggered()
{

    if(ui->actionY_Logarithmic->isChecked())
    {
        ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        ui->actionY_Logarithmic->setChecked(true);
    }
    else
    {
        ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
        ui->actionY_Logarithmic->setChecked(false);
    }

    ui->customPlot->replot();
}

// ------------------------------------------------------------------------------------------------------
// Показываем / Скрываем легенду-------------------------------------------------------------------------
void MainWindow::on_actionShow_Legend_triggered()
{
    ui->customPlot->legend->setVisible(!ui->customPlot->legend->visible());
    ui->customPlot->replot();
}

// ------------------------------------------------------------------------------------------------------
// GGA Кол-во спутников в решении
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

       QVector<double> timeLine1, usedSat1, timeLine4, usedSat4, timeLine5, usedSat5, snr_summ, snr_time; // Время, Кол-во спутников, Суммарный С/Ш

       int i=0;                             // Счетчик времени
       int day = 0;                         // День, если лог на несколько дней
       double pred_time = -1, curr_time=0;  // Предыдущее и текущее время
       double snr=0;                        // Переменная для суммирования С/Ш
       bool valid=false;                    // Флаг валидного решения (из GGA)

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Строка GSV
          // $GPGSV,5,1,18,7,7,104,40,8,13,32,42,11,18,69,36,13,66,240,50*4A
          //    0   1 2  3 4 5  6   7 8  9 10 11 12 13 14 15 16 17 18  19 20
          if(line.contains("GSV") and valid)
          if(line.split('*').count()==2)
          if(line.split('*')[1]==func::CRC(line))
          {
              line = line.split('*')[0];
              QStringList nmea = line.split(',');
              double sat_in_use = nmea[3].toDouble();

              for(int j=0;(j+4)<nmea.count();j+=4)
              {
                  if(nmea[3+j].toDouble()>33 and sat_in_use>0) // Ограничение на минимальный уровень С/Ш
                      snr+=nmea[3+j].toDouble()/sat_in_use;
              }

              continue;
          }

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
                  {
                      valid=false; // Если невалидное то ставим флаг. Нужно чтобы не копить лишние суммы С/Ш
                      continue;
                  }

                  valid = true;

                  // Кол-во используемых спутников [7]
                  double Sat = nmea[7].toDouble();

                  curr_time = func::TimeToSeconds(nmea[1]);

                  if(curr_time<pred_time) // Следующий день
                    day++;
                  pred_time = curr_time;

                  double time = func::TimeToQDateTime(nmea[1],day);

                  // Суммарный уровень С/Ш
                  if(snr>0)
                  {
                      snr_summ.append(snr);
                      snr_time.append(time);
                      snr = 0;
                  }

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

       // SNR
       func::drawGraph(ui->customPlot,snr_time,snr_summ,"Time","Value","SNR");
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
// GGA Дельта по Координатам P[i]-P[i-1].
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

           // Счетчик. Используется для Тестов.
           int i=0;
           int day = 0;
           double pred_time = -1, curr_time=0;

           QString pLine = ""; // Предыдущая строка GGA

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

                      // Вычисляем разницу между текущей и предыдущей координатой
                      if(i>0)
                      {

                          double diffXY=0;

                          // Вычисляем расстояние между точками
                          if(pLine!="")
                                diffXY = func::GGA_2Point_Diff(line, pLine);

                          pLine = line;

                          curr_time = func::TimeToSeconds(nmea[1]);

                          if(curr_time<pred_time) // Следующий день
                            day++;

                          pred_time = curr_time;

                          double time = func::TimeToQDateTime(nmea[1], day);

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

                      }
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
               func::drawGraph3D_Fix_Float_Time(ui->customPlot,timeXY1,diffXY1,timeXY4,diffXY4,timeXY5,diffXY5);

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
       int day=0;
       double pred_time = -1, curr_time = 0;

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

              double speed = nmea[7].toDouble()*1.8; // Скорость в км/ч
              double course=(QString(nmea[8]).toDouble())/10; // Курс [8]

              curr_time = func::TimeToSeconds(nmea[1]);
              if(curr_time<pred_time) // Следующий день
                day++;

              pred_time = curr_time;

              double time = func::TimeToQDateTime(nmea[1], day);

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
           int day=0;
           double time=-1, pred_time = -1, curr_time = 0;    // Время
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

                  curr_time = func::TimeToSeconds(nmea[1]);
                  if(curr_time<pred_time) // Следующий день
                    day++;

                  pred_time = curr_time;

                  time = func::TimeToQDateTime(nmea[1], day);
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

                  if(nmea[4]=="")
                      continue;

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
                func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

                // Mid
                if(X4.count()>2)
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
// GGA - Diff Age, Возраст поправки
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
       int day=0;
       double pred_time = -1, curr_time = 0;
       double underLimitTime = 0; // Время свыше лимита

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

              curr_time = func::TimeToSeconds(nmea[1]);
              if(curr_time<pred_time) // Следующий день
                day++;

              if(diffAge>10 and curr_time!=0) // !ToDo - надо как то защититься от разницы 0-24ч
                  underLimitTime+=curr_time-pred_time;

              pred_time = curr_time;

              double time = func::TimeToQDateTime(nmea[1], day);

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

       func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

       ui->textBrowser->append("Under Limit Time (Summ DF>10sec): "+func::doubleToString(underLimitTime/3600));
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
// BSS+GGA Поиск предложений где значения отличаются на 10mm + 1ppm -------------------------------------
/*
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
       double referenceValue = greferenceValue;
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
*/
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
       QVector<double> X1, Y1, X4, Y4, X5, Y5, baseLine, timeBaseline, pitch;

       // Счетчик.
       int i=0;

       // День
       int day=0;
       double curr_time=0, pred_time=-1;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит BLS
          // $PNVGBLS,124538.60,1.974,1.196,0.272,2.324,31.20,6.73,R*17
          //    0        1         2     3     4     5    6    7    8
          if(line.contains("BLS")) //Ищем строку BLS
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==9)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Тип решения
              QString type = nmea[8];

              // Курс
              double course = QString(nmea[6]).toDouble();

              // Время
              curr_time = func::TimeToSeconds(nmea[1]);

              if(curr_time<pred_time) // Следующий день
                day++;
              pred_time = curr_time;

              double time = func::TimeToQDateTime(nmea[1],day);

              // Добавляем длину базовой линии
              baseLine.append(QString(nmea[5]).toDouble());
              timeBaseline.append(time);

              // Добавляем Тангаж
              pitch.append(QString(nmea[7]).toDouble());

              // Разбиваем данные по типу решения
              // Если тип решения RTK Fix
              if(type=="R")
                  {
                      Y4.append(course);
                      X4.append(time);
                  }
              // RTK Float
              else if (type=="F")
                  {
                      Y5.append(course);
                      X5.append(time);
                  }
              else
              // Остальные - 3D/3D Diff/DR/ n/a ...
                  {
                      Y1.append(course);
                      X1.append(time);
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
           func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

           func::drawGraph(ui->customPlot,timeBaseline,pitch,"Time","Baseline(m)/Heading(grad)","Pitch");
           func::drawGraph(ui->customPlot,timeBaseline,baseLine,"Time","Baseline(m)/Heading(grad)","Baseline");

           // Длина базовой линии
           //func::drawGraph(ui->customPlot,timeLine,baseLine,"Time","Value","Baseline length");
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

           // Массивы Значений счетчика валидных предложений RTCM и время
           QVector<double> X1, Y1T, X4, Y4T, X5, Y5T, Y1V, Y4V, Y5V;

           int i=0; // Счетчик
           int day=0; // День
           double pred_time = -1, curr_time = 0, time=-1; // Время

           // Тип решения
           QString type = "";

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит GGA - Запоминаем тип решения
              // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
              //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
              if(line.contains("GGA") and line.split('*').count()==2) // Ищем строку GGA и проверяем, что есть контрольная сумма
              if(line.split(',').count()==15)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
                  {
                    QStringList nmea = line.split(',');
                    type = nmea[6];

                    if(type=="0")
                        continue;

                    curr_time = func::TimeToSeconds(nmea[1]);
                    if(curr_time<pred_time) // Следующий день
                      day++;

                    pred_time = curr_time;

                    time = func::TimeToQDateTime(nmea[1], day);

                    continue;
                  }// if GGA


              // Если строка содежит BSS
              // $PNVGBSS,82301,4,82297,15.196,1.6*66
              //    0        1  2   3      4    5
              if(line.contains("BSS") and line.split('*').count()==2) // Ищем строку BSS и проверяем, что есть контрольная сумма
              if(line.split(',').count()==6 and (func::CRC_Check(line))) // Проверка на количество полей, Проверка контрольной суммы
              {
                  // Отбрасываем контрольную сумму
                  line = line.split('*')[0];

                  // Разбиваем строку по запятой
                  QStringList nmea = line.split(',');

                  double total = nmea[1].toDouble();
                  double valid = nmea[3].toDouble();
                  // double err = nmea[2].toDouble();

                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(type=="4")
                  {
                      Y4T.append(total);
                      Y4V.append(valid);
                      X4.append(time);
                  }
                  // RTK Float
                  else if (type=="5")
                  {
                      Y5T.append(total);
                      Y5V.append(valid);
                      X5.append(time);
                  }
                  // Остальные - 3D/3D Diff/DR ...
                  else
                  {
                      Y1T.append(total);
                      Y1V.append(valid);
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
           // Total
           func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1T,X4,Y4T,X5,Y5T);
           // Valid
           func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1V,X4,Y4V,X5,Y5V);
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

       // День
       int day=0;
       double curr_time=0, pred_time=-1;

       // Начальный курс
       double cource0=0.;

       // Пока не достигнем конца файла читаем строчки
       while (!in.atEnd()) {
          // Считываем строку
          QString line = in.readLine();

          // Если строка содежит BLS
          // $PNVGBLS,124538.60,1.974,1.196,0.272,2.324,31.20,6.73,R*17
          //    0        1         2     3     4     5    6    7    8
          if(line.contains("BLS")) //Ищем строку BLS
          if(line.split('*').count()==2) // проверяем, что есть контрольная сумма
          if(line.split(',').count()==9)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
          {
              // Отбрасываем контрольную сумму
              line = line.split('*')[0];

              // Разбиваем строку по запятой
              QStringList nmea = line.split(',');

              // Тип решения
              QString type = nmea[8];

              // Курс
              double course = QString(nmea[6]).toDouble();
              // - переводим в 0-180 для удаления обрывов 0->360 и 360->0
              //if(course>180)
              //      course=360-course;

              curr_time = func::TimeToSeconds(nmea[1]);

              if(curr_time<pred_time) // Следующий день
                day++;
              pred_time = curr_time;

              double time = func::TimeToQDateTime(nmea[1],day);

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
                  X4.append(time);
              }
              // RTK Float
              else if (type=="F") {
                  //   distanceBSSR.append(QString(nmea[4]).toDouble());
                  Y5.append(diff);
                  X5.append(time);
              }
              else
              // Остальные - 3D/3D Diff/DR/ n/a ...
              {
                  //   distanceBLSR.append(QString(nmea[4]).toDouble());
                  Y1.append(diff);
                  X1.append(time);
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
       func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

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
           int i=0, noCRC=0, badCRC=0, badCountCRC=0, ggaFieldError=0, rmcFieldError=0, lowCount=0, noFirst=0;//, Hz=0;
           int doubleTime=0; // Два раза встречается одно время

           QString timeP=""; // Предыдущее время
           QString badCRCstr = "";

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              //Мало символов
              if(line.count()<5)
              {
                  lowCount++;
                  //ui->textBrowser->append(line);
                  continue;
              }

              if(line[0]!='$')
              {
                  noFirst++;
                  //ui->textBrowser->append(line);
                  continue;
              }

              // Нет контрольной суммы
              if(!line.contains('*'))
              {
                  noCRC++;
                  //ui->textBrowser->append(line);
                  continue;
              }

              // Контрольная сумма не равна 2-м символам
              else if(line.split("*")[1].count()!=2)
              {
                  badCountCRC++;
                  //ui->textBrowser->append(line);
                  continue;
              }

              // Не совпадает контрольная сумма
              else if(!func::CRC_Check(line))
              {
                  badCRC++;
                  //ui->textBrowser->append(line);
                  badCRCstr+=line+ "\n";
                  continue;
              }

              // Больше одного $

              // Анализ GGA
              if(line.contains("GGA")){

                  QStringList nmea = line.split(',');

                  if(nmea.count()!=15) // Не совпадает количество полей
                  {
                      ggaFieldError++;
                      //ui->textBrowser->append(line);
                      continue;
                  }

                  if(nmea[6]=="0")
                      continue;

                  if(nmea[1]==timeP)
                  {
                      doubleTime++;
                      //ui->textBrowser->append(line);
                  }

                  timeP=nmea[1];


              }

              // Не совпадает количество полей
              if(line.contains("RMC") and line.split(',').count()!=13)
              {
                  rmcFieldError++;
                  //ui->textBrowser->append(line);
              }

              i++;
           }

        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat

        QString delim = "----------------------------------";
        ui->textBrowser->append(delim);

        ui->textBrowser->append("Total Lines: "+QString::number(i));
        ui->textBrowser->append(delim);

        ui->textBrowser->append("No CRC: "+QString::number(noCRC));
        ui->textBrowser->append(delim);

        ui->textBrowser->append("Low count (<5): "+QString::number(lowCount));
        ui->textBrowser->append(delim);

        ui->textBrowser->append("First sign not $: "+QString::number(noFirst));
        ui->textBrowser->append(delim);

        ui->textBrowser->append("Bad CRC: "+QString::number(badCRC));
        ui->textBrowser->append(badCRCstr);
        ui->textBrowser->append(delim);

        ui->textBrowser->append("Bad Count CRC (Not 2 Digits): "+QString::number(badCountCRC));
        ui->textBrowser->append(delim);

        ui->textBrowser->append("Double Time: "+QString::number(doubleTime));
        ui->textBrowser->append(delim);

        // Проверка полей
        ui->textBrowser->append("GGA Fields (Not 15): "+QString::number(ggaFieldError)); // несоответствие количества полей в GGA
        ui->textBrowser->append(delim);

        ui->textBrowser->append("RMC Fields (Not 13): "+QString::number(rmcFieldError));
        ui->textBrowser->append(delim);

        }
    }
}

//  --------------------------------------------------------------------------------------
// Открыть второй файл для сравнения с первым
void MainWindow::on_actionOpen_File_2_triggered()
{
//    fileName2 = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), tr("NMEA LOG-Files (*.nme *.log);;All Files (*)"));
    fileNames = QFileDialog::getOpenFileNames(this, tr("Open File..."), QString(), fileTypes);
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

        // Рисуем график файла i
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
    func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

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
    func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
}

//---------------------------------------------------------------------------------------------
// GGA Разница по двум файлам по координатам в плоскости
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
    QVector<double> Time, Diff, param;

    int filesCount = fileNames.count(); // Количество файлов для сравнения

    func::dbGGA_XYTime_0_Vectors(fileName,"gga",&param);

    // Выполняем вычисления по всем файлам
    for(int i=0;i<filesCount;i++)
    {
        func::dbGGA_XYTime_0_Vectors(fileNames[i],"gga1",&param);
        func::dbGGA_2Files_Diff(&Time,&Diff);

        func::drawGraph(ui->customPlot,Time,Diff,"Time","Diff, m",fileNames[i]);

        Time.clear(); Diff.clear();
    }
}
//---------------------------------------------------------------------------------------------
// GGA Разница по двум файлам по координатам в плоскости
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

    // Добавляем окончания файлов для каждого предложения
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
            << "dX" << ","
            << "dY" << ","
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

           double R_b=0., R_l=0.; // Коэффициенты пересчета относительных координат
           double b_fix=0., l_fix=0.; // Начальные координаты

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

                    double X=0.,Y=0.,Z=0.; // Переменные для записи геоцентрических координат
                    func::BLH_to_XYZ(line,X,Y,Z);

                    double dX=0., dY=0.; // Переменные для записи относительных, плоских координат (Storegis)
                    if(GGA==0)
                    {
                        double H;
                        func::GGA_R_b_R_l(line,R_b,R_l);
                        func::GGA_BLH_Radians(line,b_fix,l_fix,H);
                    }

                    func::GGA_BLH_to_XY_0(line, dX, dY, b_fix, l_fix, R_b, R_l);

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
                            << QString::number(dX,'f',3) << ","      //
                            << QString::number(dY,'f',3) << ","      //
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
// ------------------------------------------------------------------------------------------------
// Конвертер BLH -> XYZ
void MainWindow::on_ButtonBLH2XYZ_clicked()
{
    // Удаляем пробелы
    ui->lineEdit_BLH->setText(func::removeSpaces(ui->lineEdit_BLH->text()));

    QString line = ui->lineEdit_BLH->text();

    //Координаты точки 1
    double X=0., Y=0., Z=0.;

    if(line.contains("GGA") or line.contains("TMC"))
    {
        if(func::GGA_Check(line) or func::CRC_Check(line))
            func::BLH_to_XYZ(line,X,Y,Z);
    }

    ui->lineEdit_XYZ->setText(QString::number(X,'f',3)+","+QString::number(Y,'f',3)+","+QString::number(Z,'f',3));

    // Если задана вторая точка то вычисляем разницы по координатам
    if(ui->lineEdit_BLH2->text()!="")
    {
        // Удаляем пробелы
        ui->lineEdit_BLH2->setText(func::removeSpaces(ui->lineEdit_BLH2->text()));

        QString line2 = ui->lineEdit_BLH2->text();

        double X1=0., Y1=0., Z1=0.;

        if(line2.contains("GGA") or line2.contains("TMC"))
        {
            if(func::GGA_Check(line2) or func::CRC_Check(line2))
            {
                func::BLH_to_XYZ(line2,X1,Y1,Z1);

                // Коэф. пересчета, начальная и конечная точка
                double R_b=0., R_l=0., B_fix=0., L_fix=0., H_fix=0., B=0, L=0, H=0;

                func::GGA_BLH_Radians(line,B_fix,L_fix,H_fix);
                func::GGA_BLH_Radians(line2,B,L,H);

                func::GGA_R_b_R_l(line,R_b, R_l);

                // Разница по координатам XYZ и BLH
                double dX=0., dY=0., dZ=0., dB=0., dL=0., dH=0.;

                dB = (B-B_fix)*R_b;
                dL = (L-L_fix)*R_l;
                dH = H-H_fix;

                dX = X-X1; dY = Y-Y1; dZ = Z-Z1;

                double dBLH = sqrt(dB*dB+dL*dL+dH*dH);
                double dXYZ = sqrt(dX*dX+dY*dY+dZ*dZ);

                ui->lineEdit_XYZ_Diff->setText(QString::number(dX,'f',3)+","+QString::number(dY,'f',3)+","+QString::number(dZ,'f',3)+","+ QString::number(dXYZ,'f',3));

                ui->lineEdit_XYZ2->setText(QString::number(X1,'f',3)+","+QString::number(Y1,'f',3)+","+QString::number(Z1,'f',3));

                ui->lineEdit_BLH_Diff->setText(QString::number(dB,'f',3)+","+QString::number(dL,'f',3)+","+QString::number(dH,'f',3)+","+ QString::number(dBLH,'f',3));
            }
        }
    }
}
//---------------------------------------------------------------------------------------------------
// Конвертер GPX в NMEA GGA
void MainWindow::on_actionGPX2NMEA_triggered()
{

    QString fileNameGPX = QFileDialog::getOpenFileName(this, tr("Open File..."), QString(), "GPX LOG-Files (*.gpx)");

    if(!fileNameGPX.contains("gpx"))
    {
        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat
        ui->textBrowser->setText("Please select GPX File");
        return;
    }

    // Добавляем окончания файла
    QString filenameNMEA_GGA = fileNameGPX + "_GGA.nme";

    // Файл, куда запишем
    QFile file_GGA(filenameNMEA_GGA);

    // Если файл не открылся то выходим
    if (!file_GGA.open(QFile::WriteOnly|QFile::Truncate))
        return;

    // Файлы данных: GGA
    QTextStream stream_GGA(&file_GGA);

    // Связываем переменную с физическим файлом
    QFile inputFile(fileNameGPX);

        // Если все ОК то открываем файл
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);

           QString time = ""; // Время
           QString lat = ""; // Широта
           QString lon = ""; // Долгота
           QString ele = ""; // Высота


           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd())
           {
              // Считываем строку
              QString line = in.readLine();

              /*

              Вариант 1
              <wpt lat="55.90941747" lon="37.80284067">
                <time>2016-01-29T09:21:34.000Z</time>
                <ele>173.607</ele>
              </wpt>

              Вариант 2
              <trkpt lat="62.011712" lon="32.589648" />

              */

              // Lat / Lon
              if(line.contains("lat"))
              {
                  // Разбиваем строку по знаку " и берем 1 и 3 компоненту
                  // <wpt lat="55.90941747" lon="37.80284067">
                  //     0         1        2        3       4
                  lat = line.split("\"")[1];
                  lon = line.split("\"")[3];

                  // Переводим в формат ГрадМин.долиМин
                  lat = lat.split(".")[0]+QString::number((("0." + lat.split(".")[1]).toDouble())*60, 'f', 7);
                  lon = "0"+ lon.split(".")[0]+QString::number((("0." + lon.split(".")[1]).toDouble())*60, 'f', 7);

                  //ui->textBrowser->append(lat+" "+lon);
                  continue;
              }

              // Время
              // <time>2016-01-29T09:21:34.000Z</time>
              //
              if(line.contains("time"))
              {
                  time = line.split("T")[1];
                  time = time.remove(":");
                  time = time.remove("Z</time>");

                  //ui->textBrowser->append(time);
                  continue;
              }

              // Высота
              // <ele>173.607</ele>
              //
              if(line.contains("ele"))
              {
                  ele = line;
                  ele = ele.remove(" ");
                  ele = ele.remove("<ele>");
                  ele = ele.remove("</ele>");

                  //ui->textBrowser->append(ele);
                  continue;
              }


              // Конец. Формируем GGA и записываем в файл
              // $GPGGA,094239.00,5621.1095,N,03714.7943,E,1,15,00.6,221.7,M,14.8,M,,*55
              //
              if(line.contains("</wpt>") or line.contains("</trkpt>"))
              {

                  QString GGA = "$GPGGA,"+time+","+lat+",N,"+lon+",E,1,15,00.1,"+ele+",M,,M,,*";
                  GGA += func::CRC(GGA);

                  // Записываем строчку в файл
                  stream_GGA
                          << GGA     // GGA
                          << "\r\n";   // конец строки

                  // Сбрасываем параметры
                  time = ""; // Время
                  lat = ""; // Широта
                  lon = ""; // Долгота
                  ele = ""; // Высота
                  GGA = ""; // Строка GGA
                  //ui->textBrowser->append(GGA);

              }
           }
           file_GGA.close();
        }
}
//---------------------------------------------------------------------------------------------------------
// Статистика по файлу
void MainWindow::on_actionStatistics_triggered()
{
    // Если fileName пустой - открыть Диалог. Файл откуда читаем
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

     ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat

     func::Statistics(fileName, ui->textBrowser);

}
//---------------------------------------------------------------------------------------------------------
// Оценка СКО
void MainWindow::on_actionRZD_RMS_Error_triggered()
{
    graphName = "RZD_RMS";

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

           // Массив СКО и время. VRMS - оценка СКО скорости
           QVector<double> X1, Y1, X4, Y4, X5, Y5, timeVRMS, VRMS;

           // Счетчик. Используется для Тестов.
           int i=0;           // Счетчик
           int day=0;
           double time=-1, curr_time=0, pred_time=-1;    // Время
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

                  curr_time = func::TimeToSeconds(nmea[1]);
                  if(curr_time<pred_time) // Следующий день
                    day++;

                  pred_time = curr_time;

                  time = func::TimeToQDateTime(nmea[1], day);

                  continue;
              }


              if(type=="" or type=="0" or time<0)
                  continue;

              // Если строка содежит RZD
              // $PNVGRZD,A,0.010*2D or (Z410 - РЖД) $PORZD,A,1.1,0.03*0D
              //    0     1   2   3                     0   1   2   3
              if(line.contains("RZD")) // Ищем строку RZD
              if(line.split('*').count()==2) //  и проверяем, что есть контрольная сумма
              if(line.split(',').count()==3 or line.split(',').count()==4)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
              {
                  // Отбрасываем контрольную сумму
                  line = line.split('*')[0];

                  // Разбиваем строку по запятой
                  QStringList nmea = line.split(',');
                  double RMS = QString(nmea[2]).toDouble();

                  if(nmea.count()==4)
                  {
                      VRMS.append(QString(nmea[3]).toDouble());
                      timeVRMS.append(time);
                  }

                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(type=="4")
                      {
                          X4.append(time);
                          Y4.append(RMS);
                      }
                  // RTK Float
                  else if (type=="5")
                      {
                          X5.append(time);
                          Y5.append(RMS);
                      }
                  else
                  // Остальные - 3D/3D Diff/DR ...
                      {
                          X1.append(time);
                          Y1.append(RMS);
                      }


                 i++;  // Счетчик
                 type="";
                 time=-1;

              }

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
               func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

               if(VRMS.count()>0)
                    func::drawGraph(ui->customPlot,timeVRMS,VRMS,"Time","Positioning/Velocity RMS","Velocity RMS");

               // Mid
               if(X4.count()>1)
                    func::drawMidGraph(ui->customPlot,midDistance,X4[0],X4[X4.count()-1]);
           }
           // Если массивы векторов пустые то сообщаем, что нет RZD данных
           else
           {
               ui->textBrowser->append("No RZD Data");
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
//------------------------------------------------------------------------------------------------
// График - Разница между опорной точкой и точками лога
void MainWindow::on_actionGGA_Ref_Point_Diff_triggered()
{
    graphName = "GGA_REF";

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

           // Массивы X- время, Y- значение отклонения от эталона
           QVector<double> X1, Y1, X4, Y4, X5, Y5;

           double maxDiff=0.; // Максимальное отклонение от эталона. Только в Fix
           int fakeFix = 0.; // Счетчик ложных фиксаций
           double maxDiffReal = 0.05; // Допустимое максимальное отклонение 2 см+1ppm

           // Счетчик. Используется для Тестов.
           int i=0, day=0;           // Счетчик, День
           double time=-1, curr_time=0, pred_time=-1, timeMin=0;    // Время
           QString type = ""; // Тип решения

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd())
           {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит GGA - Запоминаем тип решения и время
              // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
              //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
              if(func::GGA_Check(line))
              {
                  QStringList nmea = line.split(',');
                  type = nmea[6];

                  // Пропускаем решение с признаком "невалидно" или "счисление"
                  if(type=="0" or type=="6")
                      continue;

                  curr_time = func::TimeToSeconds(nmea[1]);

                  if(curr_time<pred_time) // Следующий день
                    day++;
                  pred_time = curr_time;

                  time = func::TimeToQDateTime(nmea[1],day);

                  if(timeMin==0)
                      timeMin = time;

                  double diffMOD = func::GGA_2Point_Diff(referencePointGGA,line);

                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(type=="4")
                      {
                          X4.append(time);
                          Y4.append(diffMOD);

                          // Максимальное отклонение от эталона
                          if(diffMOD > maxDiff)
                              maxDiff = diffMOD;

                          if(diffMOD > maxDiffReal) // Ложная фиксация при отклонении более maxDiffReal от эталона
                          {
                              fakeFix++;
                          }

                      }
                  // RTK Float
                  else if (type=="5")
                      {
                          X5.append(time);
                          Y5.append(diffMOD);
                      }
                  else
                  // Остальные - 3D/3D Diff/DR ...
                      {
                          X1.append(time);
                          Y1.append(diffMOD);
                      }

                 i++;  // Счетчик
             }

           }

           inputFile.close();

           // Если хотябы в одном массиве есть данные
           if(!X1.isEmpty() || !X4.isEmpty() || !X5.isEmpty())
           {
               // Вычисляем статистику
               double cep; // СКО
               double midDistance; // Среднее
               func::statMID_CEP(Y4,cep,midDistance); // Вычисляем Среднее арифметическое и СКО только по RTK Fix решениям

               ui->textBrowser->append("Mid: "+QString::number(midDistance));
               ui->textBrowser->append("CEP: "+QString::number(cep));
               ui->textBrowser->append("Fake fix (RTK Int): "+QString::number((double)fakeFix/(double)X4.count()*100)+ "% ("+QString::number(fakeFix)+")");
               ui->textBrowser->append("Max Diff: "+QString::number(maxDiff));

               // Выводим статистику
               func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

               // Рисуем графики
               ui->customPlot->clearGraphs();
               func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

               // Mid
               func::drawMidGraph(ui->customPlot,midDistance,timeMin,time);
           }
           // Если массивы векторов пустые то сообщаем, что нет GGA данных
           else
           {
               ui->textBrowser->append("No GGA Data");
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
//-------------------------------------------------------------------------------------------------
// Усреднение координат
void MainWindow::on_actionGGA_Coord_Average_triggered()
{
    graphName = "GGA_AVR";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat

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

            double summLat=0., summLon=0., summAlt=0.; // Сумма значений
            int i=0;           // Счетчик
            QString type = ""; // Тип решения

            // Пока не достигнем конца файла читаем строчки
            while (!in.atEnd())
            {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит GGA - Запоминаем тип решения
              // $GPGGA,113448.601,5452.3307572,N,08258.7870772,E,1,17,0.8,160.927,M,    ,M  ,0.6,  *6F
              //    0        1           2      3      4        5 6  7  8    9     10 11  12   13  14
              if(func::GGA_Check(line))
              {
                  QStringList nmea = line.split(',');
                  type = nmea[6];

                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(type=="4")
                  {
                      // Добавить фильтр отскоков и ложных решений
                      double lat = nmea[2].toDouble(); // Широта
                      double lon = nmea[4].toDouble(); // Долгота
                      double alt = nmea[9].toDouble() + nmea[11].toDouble(); // Высота над элипсоидом

                      summLat += lat;
                      summLon += lon;
                      summAlt += alt;
                      i++;
                  }
                  // RTK Float
                  else if (type=="5")
                      {
                      }
                  else
                  // Остальные - 3D/3D Diff/DR ...
                      {

                      }

             }

           }

           inputFile.close();

           // Если хотябы в одном массиве есть данные
           if(i>0)
           {
               // Вычисляем статистику
               double midLat = summLat/(double)i;
               double midLon = summLon/(double)i;
               double midAlt = summAlt/(double)i;

               ui->textBrowser->append("RTK Int Points: "+QString::number(i));
               ui->textBrowser->append("Lat: "+QString::number(midLat,'f',7));
               ui->textBrowser->append("Lon: "+QString::number(midLon,'f',7));
               ui->textBrowser->append("Alt (Ellipsoid): "+QString::number(midAlt,'f',3));

           }
           // Если массивы векторов пустые то сообщаем, что нет GGA данных
           else
           {
               ui->textBrowser->append("No GGA Data");
           }

        }// if inputFile
        else
        {
            ui->textBrowser->append("Please select a file");
        }
    }
}
//-------------------------------------------------------------------------------------------------
// RMC Проверка дат - ищем все варианты дат в файле
void MainWindow::on_actionRMC_Date_Check_triggered()
{
    {
        graphName = "RMC_DATE";

        // Если fileName пустой - открыть Диалог.
        if(fileName.isEmpty())
            MainWindow::on_actionOpen_File_triggered();

        ui->tabWidget->setCurrentIndex(1); // Переходим на вкладку Stat

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

                QVector<int> counterRMC;
                QStringList dateList; // Список дат

                //int i=0;           // Счетчик
                QString type = ""; // Тип решения

                // Пока не достигнем конца файла читаем строчки
                while (!in.atEnd())
                {
                  // Считываем строку
                  QString line = in.readLine();

                  // Если строка содежит RMC
                  // $GPRMC,185836.40,A,5544.5523157,N,03731.3598760,E,0.01,169.57,010516,0.0, E , R*2F
                  //    0      1      2      3       4     5         6   7    8      9     10  11  12
                  if(line.contains("RMC"))
                  {
                      QStringList nmea = line.split(',');

                      if(nmea.count()!=13)
                          continue;

                      type = nmea[12];
                      if(!type.contains('*'))
                          continue;

                      type = type.split('*')[0];

                      QString date = nmea[9];

                      bool dateInList=false; // Флаг - дата уже есть в списке

                      if(dateList.count()>0)
                          for(int j=0; j<dateList.count();j++)
                          {
                                if(dateList[j]==date)
                                {
                                        dateInList = true;
                                        break;
                                }
                          }

                      if(!dateInList)
                          dateList.append(date);
                    }

               }

               inputFile.close();

               // Если хотябы в одном массиве есть данные
               if(dateList.count()>0)
               {
                   for(int j=0; j<dateList.count();j++)
                    {
                        ui->textBrowser->append(dateList[j]);
                    }

               }
               // Если массивы векторов пустые то сообщаем, что нет RMC данных
               else
               {
                   ui->textBrowser->append("No RMC Data");
               }

            }// if inputFile
            else
            {
                ui->textBrowser->append("Please select a file");
            }
        }
    }
}
//------------------------------------------------------------------------------
// SDP Оценка СКО
void MainWindow::on_actionSDP_Standard_Deviation_triggered()
{
    graphName = "SDP_SDP";

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

           // Массив СКО и время
           QVector<double> X1, Y1, X4, Y4, X5, Y5;

           // Счетчик. Используется для Тестов.
           int i=0;           // Счетчик
           int day=0;
           double time=-1, curr_time=0, pred_time=-1;    // Время
           QString type = ""; // Тип решения

           // Пока не достигнем конца файла читаем строчки
           while (!in.atEnd()) {
              // Считываем строку
              QString line = in.readLine();

              // Если строка содежит SDP
              // $PNVGSDP,070300.60,0.455,0.393,1.057,A*29
              //    0         1       2     3     4   5
              if(line.contains("SDP")) // Ищем строку SDP
              if(line.split('*').count()==2) //  и проверяем, что есть контрольная сумма
              if(line.split(',').count()==6)// and (line.split('*')[1]==func::CRC(line))) // Проверка на количество полей, Проверка контрольной суммы
              {
                  // Отбрасываем контрольную сумму
                  line = line.split('*')[0];

                  // Разбиваем строку по запятой
                  QStringList nmea = line.split(',');
                  type = nmea[5];

                  if(type=="N")
                      continue;

                  curr_time = func::TimeToSeconds(nmea[1]);
                  if(curr_time<pred_time) // Следующий день
                    day++;

                  pred_time = curr_time;

                  time = func::TimeToQDateTime(nmea[1], day);

                  double sdH = QString(nmea[4]).toDouble();

                  // Разбиваем данные по типу решения
                  // Если тип решения RTK Fix
                  if(type=="R")
                      {
                          X4.append(time);
                          Y4.append(sdH);
                      }
                  // RTK Float
                  else if (type=="F")
                      {
                          X5.append(time);
                          Y5.append(sdH);
                      }
                  else
                  // Остальные - 3D/3D Diff/DR ...
                      {
                          X1.append(time);
                          Y1.append(sdH);
                      }


                 i++;  // Счетчик
                 type="";
                 time=-1;

              }

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
               func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);

               // Mid
               if(X4.count()>1)
                    func::drawMidGraph(ui->customPlot,midDistance,X4[0],X4[X4.count()-1]);
           }
           // Если массивы векторов пустые то сообщаем, что нет SDP данных
           else
           {
               ui->textBrowser->append("No SDP Data");
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
//----------------------------------------------------------------------------------------
// Конвертер Время в секунды и наоборот
void MainWindow::on_Sec2Time_clicked()
{
    if(ui->Seconds->text()!="")
        ui->Time->setText(func::SecondsToTime(ui->Seconds->text().toDouble()));

    else if(ui->Time->text()!="")
        ui->Seconds->setText(QString::number(func::TimeToSeconds(ui->Time->text()),'f',2));
}
//----------------------------------------------------------------------------------------
// RMC Трек
void MainWindow::on_actionRMC_Position_triggered()
{
    graphName = "RMC_Position";

    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1, X4, Y4, X5, Y5, bf_lf_Rb_Rl;

    // Первый файл
    func::RMC_XY_Vectors(fileName,&X1,&Y1,&X4,&Y4,&X5,&Y5,&bf_lf_Rb_Rl);

    // Выводим статистику
    ui->textBrowser->append("File 1 Stat");
    func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

    // Очищаем и рисуем график файла 1
    ui->customPlot->clearGraphs();
    func::drawGraph3D_Fix_Float(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
}
//----------------------------------------------------------------------------------------
// График разницы по времени
void MainWindow::on_actionGGA_Diff_Time_Check_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1, X4, Y4, X5, Y5;

    // Собираем векторы
    func::GGA_DiffTime_Vectors(fileName,&X1,&Y1,&X4,&Y4,&X5,&Y5);

    // Выводим статистику
    ui->textBrowser->append("File 1 Stat");
    func::Stat3D_Fix_Float(ui->textBrowser,&X1,&X4,&X5);

    // Очищаем и рисуем графики
    ui->customPlot->clearGraphs();
    func::drawGraph3D_Fix_Float_Time(ui->customPlot,X1,Y1,X4,Y4,X5,Y5);
}
//----------------------------------------------------------------------------------------
// Добавляем CRC в строку
void MainWindow::on_pushButton_clicked()
{

    if(ui->NMEA_String->text().count()<3)
        return;

    // Удалаяем пробелы и переводим в верхний регистр
    ui->NMEA_String->setText(func::removeSpaces(ui->NMEA_String->text().toUpper()));

    //Если первый символ не $ то добавляем
    if(ui->NMEA_String->text()[0]!='$')
        ui->NMEA_String->setText("$"+ui->NMEA_String->text());

    if(!ui->NMEA_String->text().contains("*"))
        ui->NMEA_String->setText(ui->NMEA_String->text()+"*");

    ui->NMEA_String->setText(ui->NMEA_String->text().split('*')[0]+"*"+func::CRC(ui->NMEA_String->text()));

}

void MainWindow::on_NMEA_String_returnPressed()
{
    MainWindow::on_pushButton_clicked();
}
//----------------------------------------------------------------------------------------
// Проверка времени PERC
void MainWindow::on_actionPERC_Time_Check_triggered()
{
    // Если fileName пустой - открыть Диалог.
    if(fileName.isEmpty())
        MainWindow::on_actionOpen_File_triggered();

    // Массивы координат 3D/Diff - X1,Y1; Fix - X4,Y4; Float - X5,Y5;
    QVector<double> X1, Y1;

    // Собираем векторы
    func::PERC_Time_Vectors(fileName,&X1,&Y1);

    // Выводим статистику
    ui->textBrowser->append("File 1 Stat");

    // Очищаем и рисуем графики
    ui->customPlot->clearGraphs();
    func::drawGraph(ui->customPlot,X1,Y1,"TOW","Counter","PERC Time Check");
}
