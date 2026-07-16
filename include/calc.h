#ifndef CALC_H
#define CALC_H
#include "data_types.h"

Results obliczZysk(int msc, typObligacji obligacja, int verbose);
void kupObligacje(Portfel* portfel, int ileObligacji, typObligacji typ, int startMonth);
int isTimeToBuy(const int currentMonth, const Parameters parameters);
int isTimeToCapitalise(const Pozycja* pozycja, const int currentMonth);
int isTimeToPay(const Pozycja* pozycja, const int currentMonth);
void aktualizujOdsetki(Portfel* portfel, const int numerPozycji, const int currentMonth, const Parameters parameteres);
void wykupStandardowo(Portfel* portfel, const int numerPozycji, const Parameters parameters);
void wykupPrzedterminowo(Portfel* portfel, const int numerPozycji, const Parameters parameters);
void wykupPozycje(Portfel* portfel, const int numerPozycji, const Parameters parameters);
void calculate(Portfel* portfel, const Parameters parameters);
#endif
