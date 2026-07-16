#ifndef CALC_C
#define CALC_C
#include "calc.h"
#include "data_types.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

// version 2

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

    if (((currentMonth - 1) % parameters.buyingFreq == 0) && (currentMonth != 1) && parameters.buyingFreq > 0)
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
        double zyskNetto = zyskBrutto * BELKA;
        aktualizowanaPozycja->wartoscBrutto -= zyskBrutto;
        portfel->konto += zyskNetto;
        if (parameteres.verbose)
            printf("\nObligacja wypłaca ci: %.2fzł na konto\n\n", zyskNetto);
    }
}

void wykupStandardowo(Portfel* portfel, const int numerPozycji, const Parameters parameters)
{
    Pozycja* pozycja = &(portfel->pozycje[numerPozycji]);
    double zyskBrutto = pozycja->wartoscBrutto - (pozycja->amount * BAZA);
    double zyskNetto = zyskBrutto * BELKA;
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
        printf("\n\nPrzedterminowy wykup obligacji %d: \nzysk brutto przed wykupem: %.2fzł\n",numerPozycji, zyskBrutto);
    if (pozycja->typ.okresWyplaty != 0 && elapsedMonths >= pozycja->typ.okresWyplaty) // obligacje wypłacające odsetki
    {
        kara = pozycja->typ.kara * (double)pozycja->amount;
        if (zyskBrutto - kara <= 0)
        {
            zyskNetto = zyskBrutto - kara;
        }
        else
        {
            zyskNetto = (zyskBrutto - kara) * BELKA;
        }
    }
    else // obligacje kapitalizujące odsetki
    {
        kara = mind(zyskBrutto, pozycja->typ.kara * (double)pozycja->amount);
        zyskBrutto -= kara;
        zyskNetto = zyskBrutto * BELKA;
    }
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

// version 1
Results obliczZysk(int msc, typObligacji obligacja, int verbose)
{
    double mnoznikMiesieczny = (obligacja.prc / 12) / 100;
    double cosieDodaje = BAZA * mnoznikMiesieczny;
    double pieniadze = BAZA;
    int gainFlag = 0;
    Results result;
    result.brutto = 0.0;
    result.netto = 0.0;
    result.kara = 0.0;
    result.wszystkieOdsetki = 0.0;

    for (int i = 0; i < min(msc, obligacja.czas); i++)
    {

        if (i % 12 == 0 && !obligacja.okresWyplaty && i != 0)
            cosieDodaje = pieniadze * mnoznikMiesieczny;

        if (i % 12 == 0 && verbose)
        {
            printf("\nBaza oprocentowania: %.2fzł\nZysk miesieczny: %.2fzł\n\n", pieniadze, cosieDodaje);
        }

        pieniadze = pieniadze + cosieDodaje; // dodanie comiesiecznego przychodu z odsetek

        if (verbose)
            printf("Miesiąc %d: %.2fzł\n", i + 1, pieniadze);

        if (pieniadze - BAZA >= obligacja.kara && !gainFlag && verbose)
        {
            printf("\nZysk jest teraz większy od kary %.2fzł\n\n", obligacja.kara);
            gainFlag = 1;
        }

        if (obligacja.okresWyplaty > 0 && (i + 1) % obligacja.okresWyplaty == 0)
        {
            result.brutto += pieniadze - BAZA;
            result.netto += (pieniadze - BAZA) * BELKA;
            if (verbose)
                printf("\nWypłata odsetek:\n    %.2fzł brutto\n    %.2fzł netto (delta %.2fzł)\nRAZEM: %.2fzł\n\n",
                       pieniadze - BAZA, (pieniadze - BAZA) * BELKA, (pieniadze - BAZA) * (1 - BELKA), result.netto);
            pieniadze = BAZA;
        }
    }

    //-----------obligacje, które wypłacają odsetki na konto
    if (obligacja.okresWyplaty > 0)
    {
        if (msc >= obligacja.czas)
        {
            result.wszystkieOdsetki = result.brutto;
            return result;
        }
        if (msc < obligacja.okresWyplaty)
        {
            result.kara = mind(pieniadze - BAZA, obligacja.kara);
            result.wszystkieOdsetki = pieniadze - BAZA;
            result.brutto = result.wszystkieOdsetki - result.kara;
            result.netto = result.brutto * BELKA;
            printf("\nPieniadze wyciągnięte przed okresem %d lat (%d miesięcy)", obligacja.czas / 12, obligacja.czas);
            return result;
        }
        result.kara = obligacja.kara;
        result.wszystkieOdsetki = result.brutto + pieniadze - BAZA;
        result.brutto = result.wszystkieOdsetki - result.kara;

        double reszta;
        if (pieniadze - BAZA - result.kara <= 0)
            reszta = (pieniadze - BAZA - result.kara);
        else
            reszta = (pieniadze - BAZA - result.kara) * BELKA;
        result.netto = result.netto + reszta;
        printf("\nPieniadze wyciągnięte przed okresem %d lat (%d miesięcy)", obligacja.czas / 12, obligacja.czas);
        return result;
    }

    //----------obligacje z kapitalizacją odsetek
    result.wszystkieOdsetki = pieniadze - BAZA;
    // teraz mamy same odsetki w pieniadzach
    if (msc >= obligacja.czas)
    {
        result.brutto = result.wszystkieOdsetki;
        result.netto = result.brutto * BELKA;
        return result;
    }
    result.kara = mind(result.wszystkieOdsetki, obligacja.kara);
    result.brutto = result.wszystkieOdsetki - result.kara;
    result.netto = result.brutto * BELKA;
    printf("\nPieniadze wyciągnięte przed okresem %d lat (%d miesięcy)", obligacja.czas / 12, obligacja.czas);
    return result;
}

#endif
