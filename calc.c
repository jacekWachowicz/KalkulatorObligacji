#ifndef CALC_C
#define CALC_C
#include "calc.h"
#include "data_types.h"
#include "helpers.h"
#include <stdio.h>



void calculate(Parameters parameters)
{
    for (int currentMonth = 1; currentMonth <= parameters.msc; currentMonth++)
    {
    }
}

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
