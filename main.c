#include <ctype.h>
#include <stdio.h>

#define BAZA 100.0
#define BELKA 0.81

typedef struct typObligacji{ 
    int czas,okresWyplaty;
    double prc,kara;
    char* nazwa;
}typObligacji;

typedef struct Parameters{
    int msc, ileObligacji,verbose;
    typObligacji typ;
}Parameters;

typedef struct Results{
    double brutto,netto,kara,wszystkieOdsetki;
}Results;


int min(int a, int b){
    if (a<b) return a;
    return b;
}
double minf(double a, double b){
    if (a<b) return a;
    return b;
}

int isNumber(char c){
    if(c>='0' && c<='9') return 1;
    return 0;
}

int parseTime(char* buff){
    int i=0,number=0;
    if(!isNumber(buff[0])) return 0;
    while(buff[i]!='\0')
    {
        if(!isNumber(buff[i]))
        {
            if(buff[i]=='l') return number*12;
            if(buff[i]=='m') return number;
            break;
        }
        number*=10;
        number+=buff[i]-'0';
        i++;
    }
    return number;
}

Results obliczZysk(int msc, typObligacji obligacja, int verbose){
    double mnoznikMiesieczny= (obligacja.prc/12)/100 ;
    double cosieDodaje= BAZA * mnoznikMiesieczny;
    double pieniadze=BAZA; 
    int gainFlag=0;
    Results result;
    result.brutto=0.0; result.netto=0.0; result.kara=0.0; result.wszystkieOdsetki=0.0;

    for(int i=0; i< min(msc,obligacja.czas) ;i++){

        if(i%12 ==0 && !obligacja.okresWyplaty && i!=0) cosieDodaje=pieniadze * mnoznikMiesieczny;

        if(i%12 == 0 && verbose) printf("\nBaza oprocentowania: %.2fzł\nZysk miesieczny: %.2fzł\n\n",pieniadze, cosieDodaje);

        pieniadze=pieniadze+cosieDodaje;// dodanie comiesiecznego przychodu z odsetek

        if(verbose)printf("Miesiąc %d: %.2fzł\n",i+1,pieniadze);

        if( 
                pieniadze-BAZA >= obligacja.kara 
                && !gainFlag && verbose){
            printf("\nZysk jest teraz większy od kary %.2fzł\n\n",obligacja.kara);
            gainFlag=1;
        }

        if(obligacja.okresWyplaty>0 && i>0 && (i+1)%obligacja.okresWyplaty==0){
            result.brutto+=pieniadze-BAZA;
            result.netto+=(pieniadze-BAZA)*BELKA;
            if(verbose)printf("\nWypłata odsetek:\n    %.2fzł brutto\n    %.2fzł netto (delta %.2fzł)\nRAZEM: %.2fzł\n\n"
                    , pieniadze-BAZA, (pieniadze-BAZA)*BELKA, (pieniadze-BAZA)*(1-BELKA), result.netto);
            pieniadze=BAZA;
        }
    }
    
    //-----------obligacje, które wypłacają odsetki na konto
    if(obligacja.okresWyplaty>0)
    {
        if(msc>=obligacja.czas)
        {
            result.wszystkieOdsetki=result.brutto;
            return result;
        }
        if(msc<obligacja.okresWyplaty)
        {
            result.kara=minf(pieniadze-BAZA, obligacja.kara);
            result.wszystkieOdsetki=pieniadze-BAZA;
            result.brutto= result.wszystkieOdsetki-result.kara;
            result.netto=result.brutto*BELKA;
            printf("\nPieniadze wyciągnięte przed okresem %d lat (%d miesięcy)",obligacja.czas/12,obligacja.czas);
            return result;
        }
        result.kara=obligacja.kara;
        result.wszystkieOdsetki= result.brutto+ pieniadze-BAZA;
        result.brutto= result.wszystkieOdsetki-result.kara;

        double reszta;
        if(pieniadze-BAZA-result.kara <= 0) reszta=(pieniadze-BAZA-result.kara);
        else reszta=(pieniadze-BAZA-result.kara)*BELKA;
        result.netto= result.netto + reszta;
        printf("\nPieniadze wyciągnięte przed okresem %d lat (%d miesięcy)",obligacja.czas/12,obligacja.czas);
        return result;
    }

    //----------obligacje z kapitalizacją odsetek
    result.wszystkieOdsetki=pieniadze-BAZA;
    // teraz mamy same odsetki w pieniadzach
    if (msc>= obligacja.czas){
        result.brutto=result.wszystkieOdsetki;
        result.netto=result.brutto*BELKA;
        return result;
    }
    result.kara=minf(result.wszystkieOdsetki, obligacja.kara);
    result.brutto=result.wszystkieOdsetki-result.kara;
    result.netto=result.brutto*BELKA;
    printf("\nPieniadze wyciągnięte przed okresem %d lat (%d miesięcy)",obligacja.czas/12,obligacja.czas);
    return result;
}

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

    typObligacji TOS;
    TOS.kara=1.0; TOS.czas=3*12; TOS.prc = 4.4; TOS.okresWyplaty=0; TOS.nazwa="TOS\0";
    typObligacji EDO;
    EDO.kara=2.0; EDO.czas=10*12; EDO.prc = 5.35; EDO.okresWyplaty=0; EDO.nazwa="EDO\0";
    typObligacji COI;
    COI.kara=2.0; COI.czas=4*12; COI.prc = 4.75; COI.okresWyplaty=12; COI.nazwa="COI\0";
    typObligacji ROR;
    ROR.kara=0.5; ROR.czas=12; ROR.prc = 4.0; ROR.okresWyplaty=1; ROR.nazwa="ROR\0";
    typObligacji DOR;
    DOR.kara=0.7; DOR.czas=2*12; DOR.prc = 4.15; DOR.okresWyplaty=1; DOR.nazwa="DOR\0";
    typObligacji OTS;
    OTS.kara=99.0; OTS.czas=3 ; OTS.prc = 2.0; OTS.okresWyplaty=0; OTS.nazwa="OTS\0";
    Parameters parameters;

    printf("Wyświetlać szczegóły?(y/n): ");
    scanf("%c",&choice);
    if(choice=='y')parameters.verbose=1;
    else parameters.verbose=0; 

    printf("Jaki typ obligacji?\n");
    printf("1. 3 miesięczne OTS (%.2f%%)\n2. 1 roczne ROR (%.2f%%)\n3. 2 letnie DOR (%.2f%%)\n4. 3 letnie TOS (%.2f%%)\n5. 4 letnie COI (%.2f%%)\n6. 10 letnie EDO (%.2f%%)\n",
            OTS.prc,ROR.prc,DOR.prc,TOS.prc,COI.prc,EDO.prc);
    scanf("%d",&typ);
    switch (typ){
        case 1:
            parameters.typ=OTS;
            break;
        case 2:
            parameters.typ=ROR;
            break;
        case 3:
            parameters.typ=DOR;
            break;
        case 4:
            parameters.typ=TOS;
            break;
        case 5:
            parameters.typ=COI;
            break;
        case 6:
            parameters.typ=EDO;
            break;
        default:
            printf("Niepoprawny typ obligacji\n");
            return 1;
    }

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

