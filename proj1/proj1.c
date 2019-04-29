/*
 * Author: Boris Štrbák, xstrba05
 * Date: 19 oct 2018
 */

#include <stdio.h>
#include <string.h>
#define MAX 1003

FILE *insf, *inserts, *f;
char buf[MAX];
	
void print_help(){
	//funkcia tiskne napovedu
	printf("\nProgram cita riadky zo stdin a postupne ich upravuje podla prikazov v textovom subore\n"
		"-------------------------------------------------------------------------------------\n"
		"./proj1 [subor.txt]\n\n"
		"[subor.txt] => meno suboru s prikazmi\n\n");
	return;
}

int citaj_riadky(const char mode){
	/*
	 *precita n riadkov zo suboru 'f' do bufferu
	 *ma 2 mody:
	 * n -> mod citania kedy precitane riadky vypisuje
	 * d -> mod 'delete' kedy precitane riadky zahazduje
	 */
	int n;	
	if((fscanf(f, "%d", &n)) != 1) 		//kontrola ci precita cislo
		n = 1;				//ak nie tak N nastavi na 1
	for(int i = 1; i <= n; i++){
		if(mode == 'n')	
			printf("%.1002s", buf);		//vypise aktualny riadok ak je v mode citani
		if((fgets(buf, MAX, stdin)) == NULL)
			return 0;			//precita do bufferu dalsi riadok	
	}						//vracia 0 ak uz v bufferi nic neni
	return n;					//vriacia pocet spracovanych riadkov
}

int zmaz_koniec_riadku(char *str){
	/*
	 *funkcia maze \n zo stringu a pripaja k nemu dalsi riadok zo stdin
	 */
	int i = 0;
	while(str[i] != '\n' && i < MAX)
		i++;				//najde index na ktorom je znak konca riadku	
	str[i] = '\0';				//nahradi \n nulovym znakom
	return 0;	
}

char *daj_content_na_koniec_buffu(){
	/*
	 *funkcia pridava retazec nacitany zo suboru 'f' na koniec retazca dest
	 */
	char str[MAX] = {"\0"};
	//int i = 0;
	//najde na ktorom indexe v 'dest' je znak konca riadku
	zmaz_koniec_riadku(buf);
	fgets(str, MAX, f);	//preciat riadok zo suboru
	strcat(buf, str);	//spoji retazce
	return buf;
}

char *daj_content_na_zaciatok_buffu(){
	/*
	 *funnkcia pridava retazec nacitany zo subor 'f' na zaciatok retazca dest
	 */
	char str[MAX] = {"\0"};		//na zaciatku incicializuje pradny retazec, do ktoreho sa cita zo suboru
	fgets(str, MAX, f);		//precita riadok zo suboru do retazca
	zmaz_koniec_riadku(str);	//zmaze '\n' z retazca
	strcat(str, buf);		//precitany retazec spoji s retazcom 'dest'
	if(str[MAX]){
		str[MAX] = '\0';
		str[MAX-1] = '\n';
	}
	strcpy(buf, str);		//vysledok skopiruje do retazca 'dest'
	return buf;			
}

int chod_na_prikaz(){
	/*
	 *funkcia skoci na urcity riadok v subore 'f'
	*/
	int n;
	int c;
	if((fscanf(f, "%d", &n)) != 1)		//skontorluje ci za prikazom ide nejake cislo
		n = 1;				//ak nie bere jednotku
	if((fseek(f, 0, SEEK_SET)) != 0)	//nastavi kurzor na zaciatok suboru a skontroluje ci nenastala chyba
		return -1;			//ak hej vrati -1
	for(int i = 1; i < n; i++)			
		do{					//cita riadky
			c = getc(f);
		} while(c != '\n' && c != EOF);
	if(c == EOF)		
		return -1;
	return n;				//vracia pocet precitanych riadkov
}

int jadro_np(char *pattern, char *subs){
	char *str1, str2[MAX], nsubs[MAX];
	int index, removel, bufl, i , k;
	if((str1 = strstr(buf, pattern)) == NULL)  	//z retazca dest odreze cast zacinajucu patternom a ulozi do str1	
		return 1;
	strcpy(nsubs, subs);
	index = strlen(pattern);			
	removel = strlen(str1);			
	bufl = strlen(buf);
	for(i = index, k = 0; str1[i] != '\0'; i++, k++) //do str2 skopiruje cast str1 neobsahujucu pattern
		str2[k] = str1[i];
	str2[k] = '\0';
	strcat(nsubs, str2);				//spoji subs so str2 cim nahradi pattern za subs
	index = bufl - removel;			//zisti, na ktorom indexe zacinal pattern v dest
	buf[index] = '\0';	
	strcat(buf, nsubs);				//spoji dest a subs	
	return 0;
}

int nahrad_pattern(const char mode){
	/*
	 * funkcia precita zo suboru pattern a 'subs'titute retazce oddelen ' ' alebo inym znakom 
	 * v retazci dest nahradi vyskyt retazcu pattern retazcom 'subs'titute
	**/
	int contr = 0, i, c, fc;
	char pattern[MAX], subs[MAX];
	fc = getc(f);					//prvy znak za prikazom bere ako oddelovac
	if(fc == '\n' || fc == EOF){			//kontoroluje ci uzivatel nezadal prikaz bez retazca
		printf("chyba v prikaze 's'\n");
		return 1;
	}
	
	for(i = 0; (c = getc(f)) != fc; i++)		//cita znaky do retazca pattern 	
		pattern[i] = c;				//az kym nenarazi na dalsi oddelovac
	pattern[i] = '\0';
	
	fgets(subs, MAX, f);				//zvysok riadky precita do retazca subs
	zmaz_koniec_riadku(subs);			//zmaze koniec riadku z retazca subs[kvolu funkcii fgets]

	if(mode == 'S'){
		while(contr != 1){
			contr = jadro_np(pattern, subs);
		}
		return 0;
	} else
		return jadro_np(pattern, subs);
}

int vloz_novy_riadok(){
	char str[MAX];
	fgets(str, MAX, f);
	insf = fopen("instmp.tmp", "w+");
	if(inserts == NULL)
		inserts = fopen("inserts.tmp", "w+");
	strcpy(buf, str);
	fprintf(insf, "%s", buf);
	
	
	return 0;
}

void zavri_subory(){
	/*
	 *funkcia skontroluje ci su otvorene subory. Ak hej tak ich zavre
	 */
	if(insf)
		fclose(insf);
	if(inserts)
		fclose(inserts);
	if(f)
		fclose(f);
	return;	
}

int main(int argc, char *argv[]){
	int c, lncon;		
	
	//kontrola ci uzivatel nezadal viac alebo menej ako 1 argument(okrem nazvu programu)
	if (argc != 2){
		print_help();
		return 0;
	}
	
	//pokus o otvorenie a kontrola otvorenia suboru(nazov je prvy argument pri spusteni)
	if ((f = fopen(argv[1], "r")) == NULL){
		printf("wrong file with rules\n");
		return 1;
	}
	
	if(fgets(buf, MAX, stdin) == NULL){
		fclose(f);
		return 1;
	}
	if(buf[MAX-1] != '\0'){
		buf[MAX-1] = '\0'; 
	}	

	//postupne prechadza subor s prikazmy po znaku
	while((c = getc(f)) != EOF){
		switch(c){
			case 'i':
				vloz_novy_riadok(buf);
				break;
			case 'b':
				daj_content_na_zaciatok_buffu();
				break;	
			case 'a':
				daj_content_na_koniec_buffu();
				break;
			case 'r':
				zmaz_koniec_riadku(buf);
				break;	
			case 's':
			case 'S':
				nahrad_pattern(c);
				break;
			case 'd':
			case 'n':
				if((lncon = citaj_riadky(c)) < 0){
					printf("Skontroluj zadane cislo!!!\n");
					zavri_subory();
					return 1;
				}
				if(lncon == 0){
					zavri_subory();
					return 0;
				}
				break;
			case 'q':
				return 0;
			case 'g':
				if((chod_na_prikaz()) < 0){
					printf("Skontroluj zadane cislo!!\n");
					zavri_subory();
					return 1;
				}
				break;	
		}
	}
	
	do {
		printf("%.1003s", buf);
	} while(fgets(buf, MAX, stdin) != NULL);
	zavri_subory();
	return 0;
}
