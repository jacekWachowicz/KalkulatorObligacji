#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#define BAZA 100.0
#define BELKA 0.81

typedef struct typObligacji
{
    int czas, okresWyplaty;
    double prc, kara, kosztZamiany;
    char *nazwa;
    double (*obliczKare)(struct typObligacji* this, double zyskBrutto,int iloscObligacji, int elapsedMonths);
} typObligacji;

typedef struct Parameters
{
    int msc, ileObligacji, verbose, buyingFreq, buyingAmt;
    typObligacji typ;
} Parameters;

typedef struct Statistics
{
    double totalOdsetki,totalKara,totalPodatek,ileZainwestowane;
    int ileNieDaloZysku,ileZakonczyloCykl;
} Statistics;

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
    double konto;
    Statistics statystyki;
} Portfel;
#endif
