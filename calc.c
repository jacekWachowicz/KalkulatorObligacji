#ifndef CALC_C
#define CALC_C
#include "calc.h"
#include "data_types.h"
#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void kupObligacje(Portfel* portfel, int ileObligacji, typObligacji typ, int startMonth)
{
    if (portfel->konto < ileObligacji * BAZA)
    {
        printf("Brakuje funduszy w portfelu!\n");
        exit(1);
    }
    Pozycja pozycja = {.amount = ileObligacji,
                       .startMonth = startMonth,
                       .typ = typ,
                       .wartoscBrutto = ileObligacji * BAZA,
                       .miesiecznyPrzychodBrutto = (ileObligacji * BAZA * typ.prc) / (100.0 * 12)};

    portfel->pozycje[portfel->liczbaPozycji] = pozycja;
    portfel->liczbaPozycji++;
    portfel->konto -= ileObligacji * BAZA;
}

int isTimeToBuy(const int currentMonth, const Parameters parameters)
{
    // dokupowanie co 1 miesiąc:
    // 1, 2-kup, 3-kup, 4-kup
    //
    // dokupowanie co 2:
    // 1,2,3-kup,4,5-kup,6,7-kup
    //
    // co 3:
    // 1,2,3,4-kup,5,6,7-kup

    if (parameters.buyingFreq > 0 && ((currentMonth - 1) % parameters.buyingFreq == 0) && (currentMonth != 1))
        return 1;
    else
        return 0;
}

int isTimeToCapitalise(const Pozycja* pozycja, const int currentMonth)
{
    // przyklad: start month:1 kapitalizacja co rok
    // 13msc na początku musi być kapitalizacja
    int elapsedMonths = currentMonth - pozycja->startMonth;
    if (pozycja->typ.okresWyplaty == 0 && elapsedMonths % 12 == 0 && elapsedMonths > 0)
        return 1;
    return 0;
}

int isTimeToPay(const Pozycja* pozycja, const int currentMonth)
{
    // przyklady:
    // startMonth 1 payInterv. 1
    // 1-t,2-t,3-t
    //
    //
    // startMonth 1 payInt. 12
    // 1,2,3,...,10,11,12-t,13,14,...,23,24-t
    // elpTime dla 12m: 12-1=11
    //
    int elapsedMonths = currentMonth - pozycja->startMonth;
    int payInterval = pozycja->typ.okresWyplaty;
    if (payInterval > 0 && (elapsedMonths + 1) % payInterval == 0)
        return 1;
    return 0;
}

double obliczZyskNetto(double zyskBruttoTotal, int iloscObligacji, float karaTotal)
{
    double zyskBruttoSztuka = zyskBruttoTotal / iloscObligacji;
    double karaSztuka = karaTotal / iloscObligacji;
    double podstawaSztuka = zyskBruttoSztuka - karaSztuka;
    double podatekSztuka = 0.0;

    if (podstawaSztuka > 0)
        podatekSztuka = round(podstawaSztuka * 0.19 * 100.0) / 100.0;

    double zyskNettoSztuka = podstawaSztuka - podatekSztuka;
    return zyskNettoSztuka * iloscObligacji;
}

int isEndOfBondLifecycle(Pozycja* pozycja, int currentMonth)
{
    // przyklady
    //  dla 3 miesiecznego
    //  startM=1 current month 3 return 0
    //  current month 4 return 1.
    int elapsedMonths = currentMonth - pozycja->startMonth;
    if (elapsedMonths == pozycja->typ.czas)
        return 1;
    return 0;
}

void reinvest(Portfel* portfel, int numerPozycji, int currentMonth, const Parameters parameters)
{
    int v = parameters.verbose;
    if (v)
        printf("\n  Rozpoczęta reinwestycja obligacji nr %d.\nStan portfela przed: %.2fzł\n", numerPozycji,
               portfel->konto);

    Pozycja* pozycja = &(portfel->pozycje[numerPozycji]);
    wykupStandardowo(portfel, numerPozycji, parameters);

    if (v)
        printf("wykupiono %d obligacji, stan portfela: %.2fzł\n", pozycja->amount, portfel->konto);

    portfel->konto -= pozycja->amount * pozycja->typ.kosztZamiany;

    if (v)
        printf("Zakupiono od nowa %d obligacji w cenie %.2fzł. Stan portfela: %.2fzł\n", pozycja->amount,
               pozycja->typ.kosztZamiany, portfel->konto);

    int ileMoznaDokupic = (int)portfel->konto / BAZA;
    portfel->konto -= ileMoznaDokupic * BAZA;
    pozycja->amount += ileMoznaDokupic;
    pozycja->startMonth = currentMonth;
    pozycja->wartoscBrutto = pozycja->amount * BAZA;
    pozycja->miesiecznyPrzychodBrutto = (pozycja->amount * BAZA * pozycja->typ.prc) / (100.0 * 12);

    if (v)
        printf("Za pozostałe pieniądze w portfelu można dokupić %d obligacji\nWartość brutto zrolowanej obligacji: "
               "%.2fzł\nKoniec reinwestycji\n\n",
               ileMoznaDokupic, pozycja->wartoscBrutto);
}

void aktualizujOdsetki(Portfel* portfel, const int numerPozycji, const int currentMonth, const Parameters parameteres)
{

    Pozycja* aktualizowanaPozycja = &(portfel->pozycje[numerPozycji]);

    if (isEndOfBondLifecycle(aktualizowanaPozycja, currentMonth))
    {
        reinvest(portfel, numerPozycji, currentMonth, parameteres);
    }

    if (isTimeToCapitalise(aktualizowanaPozycja, currentMonth))
    {
        aktualizowanaPozycja->miesiecznyPrzychodBrutto =
            (aktualizowanaPozycja->wartoscBrutto * aktualizowanaPozycja->typ.prc) / (100.0 * 12);
        if (parameteres.verbose)
            printf("\nobligacja %d: kapitalizacja: mscPrzychodBrutto=%.2fzl\n", numerPozycji,
                   aktualizowanaPozycja->miesiecznyPrzychodBrutto);
    }

    aktualizowanaPozycja->wartoscBrutto += aktualizowanaPozycja->miesiecznyPrzychodBrutto;

    if (parameteres.verbose)
        printf("obligacja %d: %.2fzł\n", numerPozycji, aktualizowanaPozycja->wartoscBrutto);

    if (isTimeToPay(aktualizowanaPozycja, currentMonth))
    { // dla obligacji ROR,COI itp, które wypłacają odsetki
        double zyskBrutto = aktualizowanaPozycja->wartoscBrutto - (aktualizowanaPozycja->amount * BAZA);
        double zyskNetto = obliczZyskNetto(zyskBrutto, aktualizowanaPozycja->amount, 0.0);
        aktualizowanaPozycja->wartoscBrutto -= zyskBrutto;
        portfel->konto += zyskNetto;
        if (parameteres.verbose)
            printf("\nObligacja wypłaca ci: %.2fzł na konto\n\n", zyskNetto);
    }
}

void wykupStandardowo(Portfel* portfel, const int numerPozycji, const Parameters parameters)
{
    Pozycja* pozycja = &(portfel->pozycje[numerPozycji]);

    double zyskBruttoTotal = pozycja->wartoscBrutto - (pozycja->amount * BAZA);

    double zyskNetto = obliczZyskNetto(zyskBruttoTotal, pozycja->amount, 0.0);

    portfel->konto += zyskNetto + pozycja->amount * BAZA;
}

void wykupPrzedterminowo(Portfel* portfel, const int numerPozycji, const Parameters parameters)
{
    Pozycja* pozycja = &(portfel->pozycje[numerPozycji]);
    int currentMonth = parameters.msc + 1;
    int elapsedMonths = currentMonth - pozycja->startMonth;

    double zyskBrutto = pozycja->wartoscBrutto - (pozycja->amount * BAZA);
    double zyskNetto = 0.0;
    double kara = 0.0;
    if (parameters.verbose)
        printf("\n\nPrzedterminowy wykup obligacji %d: \nzysk brutto przed wykupem: %.2fzł\n", numerPozycji,
               zyskBrutto);
    if (pozycja->typ.okresWyplaty != 0 && elapsedMonths >= pozycja->typ.okresWyplaty) // obligacje wypłacające odsetki
    {
        kara = pozycja->typ.kara * pozycja->amount;
    }
    else // obligacje kapitalizujące odsetki
    {
        kara = mind(zyskBrutto, pozycja->typ.kara * (double)pozycja->amount);
    }
    zyskNetto = obliczZyskNetto(zyskBrutto, pozycja->amount, kara);

    if (parameters.verbose)
        printf("Kara za wykup: %.2fzł\ncałkowity zysk netto: %.2fzł\n\n", kara, zyskNetto);
    portfel->konto += zyskNetto + pozycja->amount * BAZA;
}

void wykupPozycje(Portfel* portfel, const int numerPozycji, const Parameters parameters)
{
    // przyklad
    // czas 4lata - 48msc
    // tos wykupione 13 msc.
    // 49-13=36 git
    const int currentMonth = parameters.msc + 1;
    Pozycja* pozycja = &(portfel->pozycje[numerPozycji]);
    int elapsedMonths = currentMonth - pozycja->startMonth;
    if (elapsedMonths >= pozycja->typ.czas)
        wykupStandardowo(portfel, numerPozycji, parameters);
    else
        wykupPrzedterminowo(portfel, numerPozycji, parameters);
}

void calculate(Portfel* portfel, const Parameters parameters)
{
    for (int currentMonth = 1; currentMonth <= parameters.msc; currentMonth++)
    {
        if (parameters.verbose)
            printf("\n   Zaczyna się miesiąc %d:\n", currentMonth);
        if (isTimeToBuy(currentMonth, parameters))
        {
            portfel->konto += parameters.buyingAmt;
            portfel->ileZainwestowane += parameters.buyingAmt;
            kupObligacje(portfel, parameters.buyingAmt / BAZA, parameters.typ, currentMonth);
        } // obligacje kupowane na początku miesiąca

        // mija miesiąc i wytwarzają odsetki
        for (int i = 0; i < portfel->liczbaPozycji; i++)
        {
            aktualizujOdsetki(portfel, i, currentMonth, parameters);
        }
        // if(parameters.verbose) printf("\n");
    }
    for (int i = 0; i < portfel->liczbaPozycji; i++)
    {
        wykupPozycje(portfel, i, parameters);
    }
}

#endif
