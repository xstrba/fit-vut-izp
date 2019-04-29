/*
 * Author: Boris Štrbák, xstrba05
 * Date: 12 nov 2018
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IC 100000000			//maximalny pocet iteracii
#define LOGEPS 100000000

//chybne zadanie cisla
#define numErr(x) fprintf(stderr, "Zadal si zle %d. cislo.\n", x)
//param count erro --> zly pocet parametrov
#define paramcErr(opt, n) fprintf(stderr, "Zadal si zly pocet parametrov.\nZadaj %d argumenty pre %s.\n", n, opt)
//zadanie zlej volby
#define optionErr() fprintf(stderr, "Zadal si zlu volbu.\n")	
				
short mode = 1;		//1 => log ... 2 => pow

void print_help(){
	//vytiskne nejaku napovedu
	printf(	"\nProgram pocita prirodzeny logaritmus a mocninu z cisla\n"
		"\n------------------------------------------------------\n\n"
		"./proj2 [option] [X] [Y] [N]\n\n"
		"[option] -> --log  - vypocet logaritmu\n"
		"            --pow  - vypocet mocniny\n"
		"            --help - zobrazi napovedu\n"
		"             -h    - zobrazi napovedu\n"
		"   [X]   -> cislo, ktoreho logaritmus/mocnina sa pocita\n"
		"   [Y]   -> v pripade volby '--pow' je to mocninovy zaklad\n"
		"         -> v pripade volby '--log' sa nezadava\n"
		"   [N]   -> pocet iteracii\n\n"
		"Maximalny pocet iteracii je nastaveny na %d.\n", MAX_IC);
	return;
}
double mad_log(double x){
	//navratoveho hodnoty pre sialene logaritmy
	if(x < 0)
		return NAN;
	if(x == 0)
		return -INFINITY;
	if(isinf(x))
		return INFINITY;
	return NAN;	
}

double taylor_log(double x, unsigned int n){
	/*
	 *funkcia vyrata prirodzeny logaritmus
	 *pre x > 1 podla druheho vzorca
	 *pre x < 1 podla prveho vzorca
	 *oba vzorce sa lisia znamienkom +/- a citatelom 
	 */
	if(x <= 0 || isinf(x))
		return mad_log(x);
	double realx, sum, t;	
	unsigned int i = 2;
	if(x < 1){
		//pocita logaritmus pre cislo 1 - x nie pre x
		//napr ked chceme vypocitat log(0.25) musime pocitat log(0.75)
		realx = 1 - x;
		t = -realx;
	} else{
		//v tomto pripade je realx iba hodnota citatela vo vzorci
		realx = (x-1)/x;
		t = realx;
	}
	sum = t;	//nastavenie suctu na zaciatku
	while(i <= n){
		t = t*(i-1)*realx/(double) i;	//vypocet noveho clena postupnosti
		sum += t;										
		i += 1;
	}	
	
	return sum;
}

double cfrac_log(double x, unsigned int n){
	/*
	 *funkcia pocita logaritmus cisla pomocou zretazeneho zlomku
	 */
	//pocita log pre (1+y)/(1-y)
	//realx je hodnota cisla,	ktore ked dosadime do rovnice za y dostaneme povodne cislo
	//ked chceme pocitat log pre 2 musime pocitat pre (2-1)/(2+1) = 1/3
	//potom (1+1/3)/(1-1/3) = 2, teda pocitame logaritmus pre dvojku 
	if(x <= 0 || isinf(x))
		return mad_log(x);
	double realx = (x-1)/(1+x);    
	double cf = 0.0;
	int k = n-1;			 
	double y = realx*realx;	
	
	while(k >= 1){
		cf = (k*k*y)/((2*k+1) - cf);  //vypocet celkoveho vysledku
		k--;
	}
	return 2*realx/(1-cf);		//posledna iteracia
}

double taylor_pow(double x, double y, unsigned int n){
	/*
	 *funkcia pocita y-tu mocninu z cisla x pomocou taylorovho logaritmu
	 */
	int inty; //pre zaporne x sa da pocitat iba cela mocnina
	short minus = 0;
	if(x == 0){	//sialene hodnoty
		if(y < 0)
			return INFINITY;
		if(isnan(y))
			return NAN;
		else 
			return 0;
	}	
	if(y == 0)
		return 1;
	if(x < 0){	//pocitanie pre zaporne x
		x = -x;
		inty = y;
		if(y - inty != 0.0 && !isnan(y) && !isinf(y))
			return -NAN;
		minus = 1;	//kvoli kontrole na konci (akoby mod)
	}
	if(x == 1 && isinf(y))
		return 1;
	double t = 1;		//vypocet nulteho clena
	double sum = t;
	unsigned int i = 1;
	double log = taylor_log(x, n);	
	while(i < n){
		t = t*(y*log/i);	//vypocet dalsieho clenai
		sum += t;			
		i++;
		//nekonecne hodnoty
		if(isinf(sum)){	
			//specialny pripad pre x < 1 na +y a pre x > 1 na -y
			if((x < 1 && y > 0) || (x > 1 && y < 0))
				sum = 0;
			else if(x == 1)
				sum = 1;
			else 
				sum =  INFINITY;
			break;
		}
	}
	if(minus && inty % 2 != 0)	//ak je inty zaporne vracia prevratenu hodnot	u
		return -sum;
	return sum;
}

double taylorcf_pow(double x, double y, unsigned int n){
	/*
	 *funkcia pocita y-tu mocninu cisla x pomocou vypoctu logaritmu cez zretazeny zlomok
	 */
	int inty; //pre zaporne x sa da pocitat iba cela mocnina
	short minus = 0;	
	if(x == 0){	//sialene hodnoty
		if(y < 0)
			return INFINITY;
		if(isnan(y))
			return NAN;
		else 
			return 0;
	}
	if(y == 0)
		return 1;	
	if(x < 0){	//pre zaporne x
		x = -x;
		inty = y;
		if(y - inty != 0.0 && !isnan(y) && !isinf(y))
			return -NAN;
		minus = 1;	//kvoli kontrole na konci (akoby mod)
	}
	if(x == 1 && isinf(y))
		return 1;
	double t = 1;		//vypocet nulteho clena
	double sum = t;
	unsigned int i = 1;
	double log = cfrac_log(x, n);
	while(i < n){
		t = t*(y*log/i);		//vypocet dalsieho clena
		sum += t;	
		i++;
		//nekonecne hodnoty
		if(isinf(sum)){	
			//specialny pripad pre x < 1 na +y a pre x > 1 na -y
			if((x < 1 && y > 0) || (x > 1 && y < 0))
				sum = 0;
			else if(x == 1)
				sum = 1;
			else 
				sum =  INFINITY;
			break;
		}
	}
	if(minus && inty % 2 != 0)	//ak je inty zaporne vracia prevratenu hodnot	u
		return -sum;
	return sum;
}

double mylog(double x){
	/*
	 *funkcia pocita logaritmus pomocou funkcie cfrac_log
	 *porovnanava dva predchadzajuce vysleky pri com zakazdym zvysuje pocet oteracii
	 *ak su zhodne na urcity pocet desatinnych miest tak vracia vysledok
	 */
	if(x == 1.00000000000000)
		return 0.0;
	if(x <= 0 || isinf(x))
		return mad_log(x);
	double res, prevres, eps;
	int k = 5;	//preskocenie prvych par iteracii
	if(x > 1)							//vypocet epsilonu
		eps = LOGEPS*x;			
	else
		eps = LOGEPS*10; 
	do{
		if(isnan(res))	//pri prvom kroku este nevieme res
			prevres = 0;  
		else    
			prevres = eps*res;		//predchadzajuci vysledok		
		res = cfrac_log(x, k);
		k++;
		if(isinf(res))
			return INFINITY;
	}while(fabs(eps*res-prevres) >= 1);
	return res;
}

double mypow(double x, double y){
	/*
	 *funkcia pocita y-tu mocninu z cisla x pomocou taylorovho logaritmu
	 */
	int inty; //cela cast zaporneho y
	short minus = 0;
	//pre hodnoty mensie ako x:
	//pocita mocninu kladneho cisla
	if(x == 0){	//sialene hodnoty
		if(y < 0)
			return INFINITY;
		if(isnan(y))
			return NAN;
		else 
			return 0;
	}	
	if(y == 0)
		return 1;
	if(x < 0){	//pre zaporne x
		x = -x;
		inty = y;
		if(y - inty != 0.0 && !isnan(y) && !isinf(y))
			return -NAN;
		minus = 1;	//kvoli kontrole na konci (akoby mod)
	}
	if(x == 1 && isinf(y))
		return 1;
	double eps;
	double t = 1;			//vypocet nulteho clena
	double sum = t;
	unsigned int i = 1;
	double log = mylog(x);		//pocita logaritmus podla mylog
	if((x > 1 && y  > 0) || (x < 1 && y < 0))
		eps = LOGEPS*x*fabs(y);
	else
		eps = LOGEPS*10*fabs(y);
	do{
		t = t*(y*log/i);		//vypocet dalsieho clena
		sum += t;			
		i++;
		//nekonecne hodnoty
		if(isinf(sum)){	
			//specialny pripad pre x < 1 na +y a pre x > 1 na -y
			if((x < 1 && y > 0) || (x > 1 && y < 0))
				sum = 0;
			else if(x == 1)
				sum = 1;
			else 
				sum =  INFINITY;
			break;
		}
	}while(fabs(eps*t) >= 1);			
	if(minus && inty % 2 != 0)	//ak je inty zaporne vracia prevratenu hodnot	u
		return -sum;
	return sum;
}


int check_opt(char *str, int cnt){
	//funkcia kontroluje zadane moznosti(druhy argument na vstupe)
	//zaroven nadstavuje mod programu
	//vracia 1 ak nebol zadany ziadny druhy argument
	//vracia 1 a viac ak bol zadany chybny argument
	//vracia -1 ak bol zadany argumnt "help"
	//vracia 0 ak bol zadany argument pre pow alebo log
	int errFlag = 0;
	if(cnt < 2){
		print_help();
		return 1;
	}
	if(strcmp(str, "--help") == 0 || strcmp(str, "-h") == 0){
			print_help();
			return -1;
	}
	
	if(strcmp(str, "--pow") == 0){
		if(cnt < 5){		//pre mocninu treba 3 dalsie argumenty
			paramcErr("--pow", 3);
			errFlag++;
		}
		else
			mode = 2;
	} else if(strcmp(str, "--log") == 0){
		if(cnt < 4){		//pre logaritmus treba 2 dalsie argumenty
			paramcErr("--log", 2);
			errFlag++;
		}
		else
			mode = 1;
	} else{
		optionErr();
		errFlag++;
	}
	return errFlag;
}

int scan_nums(double *x, double *y, int *n, int npos, char *args[]){
	//funkcia konvertuje argumenty na cisla a zaroven robi kontrolu
	//vracia 1 v pripade zleho zadania cisla a 0 v pripade spravneho
	//zaroven vypisuje chybove hlasenia
	char *strx, *strn, *stry;
	*x = strtod(args[2], &strx);
	if(*strx != 0){
		numErr(1);
		return 1;
	}
	*n = strtol(args[2+npos], &strn, 10);
	if(*strn != 0 || *n < 1){
		numErr(npos+1);
		return 1;
	}
	if(mode == 2){
		*y = strtod(args[3], &stry);
		if(*stry != 0){
			numErr(2);
			return 1;
		}
		//if(*x < 0){
			//numErr(1);
			//return 1;
		//}
	}
	return 0;
}

void print_log(double x, int n){
	//Funkcia tiskne logaritmy
	printf("       log(%g) = %.12g\n", x, log(x));
	printf(" cfrac_log(%g) = %.12g\n", x, cfrac_log(x, n));
	printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
	printf("     mylog(%g) = %.7e\n", x, mylog(x));
}

void print_pow(double x, double y, int n){
	//funkcia tiskne mocniny
	printf("         pow(%g, %g) = %.12g\n", x, y, pow(x, y));
	printf("  taylor_pow(%g, %g) = %.12g\n", x, y, taylor_pow(x, y, n));
	printf("taylorcf_pow(%g, %g) = %.12g\n", x, y, taylorcf_pow(x, y, n));
	printf("       mypow(%g, %g) = %.7e\n", x, y, mypow(x, y));
}

int main(int argc, char *argv[]){
	int check = check_opt(argv[1], argc); 	//kontrola druheho argumentu
	if(check != 0)
		return check > 0 ? 1 : 0;
	
	short npos = 2;	//pozicia N
	double x, y;
	int n;

	if(mode == 1)
		npos = 1;
	if(scan_nums(&x, &y, &n, npos, argv))	//scanovanie a kontrola cisel
		return 1;
	if(mode == 1)				//tisk logaritmov/mocnin
		print_log(x, n);	
	else if(mode == 2)
		print_pow(x, y, n);
	else 
		return 1;
	return 0;
}
