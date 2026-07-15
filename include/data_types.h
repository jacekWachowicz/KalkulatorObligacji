#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#define BAZA 100.0
#define BELKA 0.81

typedef struct typObligacji
{
    int czas, okresWyplaty;
    double prc, kara;
    char *nazwa;
} typObligacji;

typedef struct Parameters
{
    int msc, ileObligacji, verbose;
    typObligacji typ;
} Parameters;

typedef struct Results
{
    double brutto, netto, kara, wszystkieOdsetki;
} Results;

typedef struct Pozycja
{
    int amount, startMonth;
    double zyskBrutto;
} Pozycja;

typedef struct Portfel
{
    Pozycja *pozycje;
    double konto;
} Portfel;
#endif
