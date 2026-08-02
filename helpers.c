#ifndef HELPERS_C
#define HELPERS_C
#include "helpers.h"

int min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

double mind(double a, double b)
{
    if (a < b)
        return a;
    return b;
}

double maxd(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

int isNumber(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

int parseTime(char *buff)
{
    int i = 0, number = 0;
    if (!isNumber(buff[0]))
        return 0;
    while (buff[i] != '\0')
    {
        if (!isNumber(buff[i]))
        {
            if (buff[i] == 'l')
                return number * 12;
            if (buff[i] == 'm')
                return number;
            break;
        }
        number *= 10;
        number += buff[i] - '0';
        i++;
    }
    return number;
}

#endif
