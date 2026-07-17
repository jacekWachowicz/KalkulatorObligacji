#ifndef CALC_H
#define CALC_H
#include "data_types.h"

void kupObligacje(Portfel* portfel, int ileObligacji, typObligacji typ, int startMonth);
void wykupPozycje(Portfel* portfel, const int numerPozycji, const Parameters parameters);
void calculate(Portfel* portfel, const Parameters parameters);
double obliczKareWyplacajaco(typObligacji* this, double zyskBrutto, int iloscObligacji, int elapsedMonths);
double obliczKareKapitalizujaco(typObligacji* this, double zyskBrutto, int iloscObligacji, int elapsedMonths);
#endif
