#include <ctype.h>
#include <stdio.h>
#include "data_types.h"
#include "helpers.h"
#include "calc.h"
#include <stdlib.h>


double analiza(Parameters parameters){
    printf("\n===Obligacje %s===\n\nOprocentowanie: %.2f%%\nCzas oszczędzania: %d miesięcy\nIlość obligacji: %d\n"
            ,parameters.typ.nazwa, parameters.typ.prc, parameters.msc, parameters.ileObligacji);

    Results result=obliczZysk(parameters.msc, parameters.typ, parameters.verbose);

    printf("\n\n========Analiza pojedynczej obligacji=========\n");
    printf("Wszystkie wytworzone odsetki: %.3fzł\nKara za wcześniejszy wykup: %.3fzł\nZysk brutto: %.3fzł\nZapłacony podatek: %.3fzł\nZysk netto: %.3fzł\n",
            result.wszystkieOdsetki, result.kara, result.brutto, result.brutto-result.netto, result.netto);

    if(parameters.ileObligacji!=1){
    printf("\n========Analiza całości=========\n");
    printf("Wszystkie wytworzone odsetki: %.2fzł\nKara za wcześniejszy wykup: %.2fzł\nZysk brutto: %.2fzł\nZapłacony podatek: %.2fzł\nZysk netto: %.2fzł\n",
            result.wszystkieOdsetki*parameters.ileObligacji, result.kara*parameters.ileObligacji, result.brutto*parameters.ileObligacji,parameters.ileObligacji*(result.brutto-result.netto), result.netto*parameters.ileObligacji);
    }
    return result.netto*parameters.ileObligacji;
}

int main(){
    char choice='n';
    char buff[10];
    int typ=1, kapital1=0;
    double zysk;
    const typObligacji types[] = {
        { .nazwa = "OTS", .kara = 99.0, .czas = 3,     .prc = 2.0,  .okresWyplaty = 0 },
        { .nazwa = "ROR", .kara = 0.5,  .czas = 12,    .prc = 4.0,  .okresWyplaty = 1 },
        { .nazwa = "DOR", .kara = 0.7,  .czas = 24,    .prc = 4.15, .okresWyplaty = 1 },
        { .nazwa = "TOS", .kara = 1.0,  .czas = 36,    .prc = 4.4,  .okresWyplaty = 0 },
        { .nazwa = "COI", .kara = 2.0,  .czas = 48,    .prc = 4.75, .okresWyplaty = 12 },
        { .nazwa = "EDO", .kara = 2.0,  .czas = 120,   .prc = 5.35, .okresWyplaty = 0 }
    };

    const int n=sizeof(types)/sizeof(types[0]);

    Parameters parameters;

    printf("Wyświetlać szczegóły?(y/n): ");
    scanf("%c",&choice);
    if(choice=='y')parameters.verbose=1;
    else parameters.verbose=0; 

    printf("Jaki typ obligacji?\n");
    for(int i=0; i<n; i++)
    {
        printf("\n%d. %s - ",i+1,types[i].nazwa);
        if(types[i].czas<12) printf("%d miesięce\n",types[i].czas);
        else printf("%d lata\n",types[i].czas/12);
        printf("    Oprocentowanie: %.2f\n",types[i].prc);

    }
    scanf("%d",&typ);
    if(typ<1 || typ >n){
        printf("Niepoprawny typ\n");
        return 1;
    }

    parameters.typ=types[typ-1];
    printf("Po jakim czasie chcesz wyjąć pieniądze?\n(np. 12m - 12 miesięcy, 10l - 10 lat)\n");
    scanf("%s",buff);
    parameters.msc=parseTime(buff);

    printf("Ile chcesz zainwestować? (zł)\n");
    scanf("%d",&kapital1);
    parameters.ileObligacji=kapital1/100;

    zysk=analiza(parameters);

    printf("\nSuma: %.2fzł\n",zysk+(double)kapital1);
    return 0;
}

