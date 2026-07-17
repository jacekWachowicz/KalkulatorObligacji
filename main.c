#include "data_types.h"
#include "calc.h"
#include "helpers.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


void getParameters(Parameters* parameters, Portfel* portfel, const typObligacji* types, int n)
{

    char choice = 'n';
    char buff[11];
    int typ = 1;
    int kapital1;

    printf("Jaki typ obligacji?\n");
    for (int i = 0; i < n; i++)
    {
        printf("\n%d. %s - ", i + 1, types[i].nazwa);
        if (types[i].czas < 12)
            printf("%d miesięce\n", types[i].czas);
        else
            printf("%d lata\n", types[i].czas / 12);
        printf("    Oprocentowanie: %.2f%%\n", types[i].prc);
        printf("    Kara za wcześniejszy wykup: %.2fzł\n", types[i].kara);
    }
    scanf("%d", &typ);
    if (typ < 1 || typ > n)
    {
        printf("Niepoprawny typ\n");
        exit(1);
    }

    parameters->typ = types[typ - 1];
    printf("Po jakim czasie chcesz wyjąć pieniądze?\n(np. 12m - 12 miesięcy, 10l - 10 lat)\n");
    scanf("%10s", buff);
    parameters->msc = parseTime(buff);

    printf("Ile chcesz zainwestować? (zł)\n");
    scanf("%d", &kapital1);
    portfel->konto = (double)kapital1;
    portfel->ileZainwestowane=kapital1;
    parameters->ileObligacji = kapital1 / 100;

    printf("Czy chcesz regularnie dokupować obligacje? (y/n)\n");
    scanf(" %c", &choice);
    if (choice == 'y')
    {
        printf("Jak dużo chcesz wpłacać?\n");
        scanf("%d", &parameters->buyingAmt);
        printf("co ile miesięcy?\n");
        scanf("%d", &parameters->buyingFreq);
    }
    if ((parameters->msc <= 36 && (1 + (parameters->msc / parameters->buyingFreq)) <= 5) || (parameters->buyingFreq==0 && parameters->msc<=10*12))
        parameters->verbose = 1;
    else
        parameters->verbose = 0;
}

int main(){
    Portfel portfel={.konto=0.0,.liczbaPozycji=0};

    const typObligacji types[] = {{.nazwa = "OTS", .kara = 99.0, .czas = 3,   .prc = 2.0,  .okresWyplaty = 0, .kosztZamiany=100.0, .obliczKare = obliczKareKapitalizujaco},
                                  {.nazwa = "ROR", .kara = 0.5,  .czas = 12,  .prc = 4.0,  .okresWyplaty = 1, .kosztZamiany=99.9,  .obliczKare = obliczKareWyplacajaco},
                                  {.nazwa = "DOR", .kara = 0.7,  .czas = 24,  .prc = 4.15, .okresWyplaty = 1, .kosztZamiany=99.9,  .obliczKare = obliczKareWyplacajaco},
                                  {.nazwa = "TOS", .kara = 1.0,  .czas = 36,  .prc = 4.4,  .okresWyplaty = 0, .kosztZamiany=99.9,  .obliczKare = obliczKareKapitalizujaco},
                                  {.nazwa = "COI", .kara = 2.0,  .czas = 48,  .prc = 4.75, .okresWyplaty = 12,.kosztZamiany=99.9,  .obliczKare = obliczKareWyplacajaco},
                                  {.nazwa = "EDO", .kara = 2.0,  .czas = 120, .prc = 5.35, .okresWyplaty = 0, .kosztZamiany=99.9,  .obliczKare = obliczKareKapitalizujaco}};

    const int n = sizeof(types) / sizeof(types[0]);

    Parameters parameters = {.buyingAmt = 0.0, .buyingFreq = 0};

    getParameters(&parameters,&portfel,types,  n);

    int liczbaPozycji = 1;
    if (parameters.buyingFreq > 0)
    {
        liczbaPozycji = 1 + (parameters.msc / parameters.buyingFreq);
    }
    portfel.pozycje = malloc(liczbaPozycji * sizeof(Pozycja));

    kupObligacje(&portfel, parameters.ileObligacji, parameters.typ, 1);

    calculate(&portfel, parameters);

    printf("\nIle zainwestowano: %.2fzł\nZysk: %.2fzł\nCałość: %.2fzł\n",portfel.ileZainwestowane,portfel.konto-portfel.ileZainwestowane, portfel.konto);
    free(portfel.pozycje);
    return 0;
}

