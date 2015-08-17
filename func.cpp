#include "func.h"

func::func()
{

}

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
    s = QString::number(sum,16); //перевод в шестнадцатиричный формат с 2 - мя символами

    // Перевод в Uppercase

    return(s.toUpper());
    }
    else
        return("NO *");
}
