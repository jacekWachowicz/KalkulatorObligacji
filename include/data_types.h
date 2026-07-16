#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#define BAZA 100.0
#define BELKA 0.81

typedef struct typObligacji
{
    int czas, okresWyplaty;
    double prc, kara, kosztZamiany;
    char *nazwa;
} typObligacji;

typedef struct Parameters
{
    int msc, ileObligacji, verbose, buyingFreq, buyingAmt;
    typObligacji typ;
} Parameters;

typedef struct Results
{
    double brutto, netto, kara, wszystkieOdsetki;
} Results;

typedef struct Pozycja
{
    int amount, startMonth;
    double wartoscBrutto,miesiecznyPrzychodBrutto;
    typObligacji typ;
} Pozycja;

typedef struct Portfel
{
    Pozycja *pozycje;
    int liczbaPozycji;
    double konto,ileZainwestowane;
} Portfel;
#endif
