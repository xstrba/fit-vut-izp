/**
 * Program k projektu c. 3, predmet IZP 2018/2019
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 *
 * Author: Boris Strbak, xstrba05
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> // strlen, strcmp
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

#define std_err(s) fprintf(stderr,"ERROR: " s "\n")
#define std_ferr(s, ...) fprintf(stderr,"ERROR: " s "\n", __VA_ARGS__)

#define print_help printf("\n--------------------------------------------------------------------\n\n""           Clusterova analyza. Metoda najblizsie suseda.\n\n""--------------------------------------------------------------------\n\n""./proj3 [subor.tx] [N]\n\n""[subor.txt] -> subor s objektmi.\n""          Format suboru:\n""            count=X [kde X je pocet objektov v subore]\n""            ID X Y [ID je identifikator objektu, X a Y su suradnice]\n\n""     [N]    -> volitelny parameter oznacujuci pocet clusterov. N > 0\n\n")
#define DEFAULT_SIZE 1
#define MAX_LINE 1000
/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
	int id;
    	float x;
    	float y;
};

struct cluster_t {
    	int size;
    	int capacity;
    	struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
	assert(c != NULL);
    	assert(cap >= 0);
	c->size = 0;
	if(cap > 0 && (c->obj = malloc(cap * sizeof(struct obj_t))) != NULL)
	{
		c->capacity = cap;
		return;
	}
	c->capacity = 0;
	c->obj = NULL;
	return;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
	assert(c != NULL);
	assert(c->obj != NULL);

	free(c->obj);
	init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	assert(c);
	assert(c->size >= 0);
	assert(obj);

    	if(c->size >= c->capacity || c->capacity == 0)
	{
		if(resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL)
		{
			c->capacity = 0;
			return;
		}
	}
	assert(c->obj);
	c->obj[c->size++] = obj;
	return;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Vycisti shluky a uvolni pamat
*/
void clear_clusters(struct cluster_t *c, int narr);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
   	assert(c2 != NULL);

    	for(int i = 0; i < c2->size; i++)
		append_cluster(c1, c2->obj[i]);
	sort_cluster(c1);
	return;
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
	assert(idx < narr);
    	assert(narr > 0);

    	clear_cluster(&carr[idx]);
	for(int i = idx; i < narr-1; i++)
		carr[i] = carr[i+1];
	return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
 	assert(o1 != NULL);
  	assert(o2 != NULL);

	int x = o1->x - o2->x;
	int y = o1->y - o2->y;
	return sqrt((x*x) + (y*y));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    	assert(c1 != NULL);
    	assert(c1->size > 0);
    	assert(c2 != NULL);
    	assert(c2->size > 0);

	float dist, min_dist = -1;
    	for(int i = 0; i < c1->size; i++)
	{
		for(int k = 0; k < c2->size; k++)
		{
			dist = obj_distance(&c1->obj[i], &c2->obj[k]);
			if(min_dist == -1 || dist < min_dist)
				min_dist = dist;
		}
	}
	return min_dist;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    	assert(narr > 0);

	float dist, min_dist = -1;
    	for(int i = 0; i < narr-1; i++)
	{
		for(int k = i+1; k < narr; k++)
		{
			dist = cluster_distance(&carr[i], &carr[k]);
			if(min_dist == -1 || dist < min_dist)
			{
				*c1 = i;
				*c2 = k;
				min_dist = dist;
			}
		}
	}
	return;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
	Funkcia hlada v poli clusterov carr o velkosti size
	objekty podla id
	vracia 1 ak nastala zhoda a 0 ak nie
*/
int find_obj(struct cluster_t *carr, int id, int size)
{
	for(int i = 0; i < size-1; i++)
	{
		for(int k = 0; k < carr[i].size; k++)
		{
			if(carr[i].obj[k].id == id)
			{
				std_ferr("Id objektu cislo %d neni unikatne.", size);
				return 1;
			}
		}
	}
	return 0;
}

/*
 pomocna funkcia na ziskanie objektov z retazca
 kontroluje spravnost zadania cisel
*/
int get_coords(int *id, float *x, float *y, char *str)
{
    char c;
    //kontroluje aj ci za tretim cislom nasleduje '\n'
    if(sscanf(str, "%d %f %f%c", id, x, y, &c) != 4 || c != '\n')
        return 0;
	//kontrola velkosti cisel
    if(*id < 0 || *x > 1000 || *x < 0 || *y > 1000 || *y < 0)
		return 0;
    if(isnan(*x) || isnan(*y))
        return 0;
    return 1;
}

/*
	funkcia okrem fgets aj kontroluje dlzku riadka
*/
char *my_fgets(char *str, int c, FILE *f){
	if(fgets(str, MAX_LINE, f) == NULL)
		return NULL;
    //kontrola dlzky riadka
	unsigned int len = strlen(str);
	if(len > MAX_LINE && str[MAX_LINE-2] != '\n')
	{
		std_ferr("Prilis dlhy riadok cislo %d", c);
		return NULL;
	}
	return str;
}



/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    	assert(arr != NULL);

    	FILE *f;
	char line[MAX_LINE], c;
	int count, i = 0, rep_flag = 0;
    	struct cluster_t *clusters;
	struct obj_t obj;

    	*arr = NULL;
	//otvorenie suboru + kontrola
	if((f = fopen(filename, "r")) == NULL)
	{
		std_err("Soubor se nepodarilo otevrit.");
		return -1;
	}
	//scan riadka zo suboru
	if(my_fgets(line, 1, f) == NULL)
	{
		std_err("Skontroluj prvy riadok suboru");
		fclose(f);
		return -1;
	}
    	//count=X
    	if(sscanf(line, "count=%d%c", &count, &c) != 2 || count < 0 || c != '\n')
	{
		std_err("Zle zadany pocet objektov v subore");
		fclose(f);
		return -1;
	}

	//allokacia pola shlukov
	if((clusters = malloc(count * sizeof(struct cluster_t))) == NULL)
	{
		std_err("Chyba alokacie pamati");
		fclose(f);
		return -1;
	}

	//citanie riadkov + tvorba shlukov a objektov
	while(i < count)
	{
		//inicializacia shluku
		init_cluster(&clusters[i], 0);
        //citanie riadka zo suboru
        if(my_fgets(line, i+2, f) == NULL)
		{
            clear_clusters(clusters, i+1);
			std_err("Malo objektov v subore");
			fclose(f);
			return -1;
        }
        //kontrola objektu v subore a ulozenie do struktury
        if(!get_coords(&obj.id, &obj.x, &obj.y, line))
		{
            clear_clusters(clusters, i+1);
			std_ferr("Zle zadany objekt na riadku %d", i+2);
			fclose(f);
			return -1;
        }
        //hladanie objektov s rovnakym id
        if(find_obj(clusters, obj.id, i+1))
		    rep_flag++;
        //pridanie objektu do shluku + kontrola
        append_cluster(&clusters[i], obj);
		if(clusters[i].obj == NULL)
		{
			std_err("Chyba priradenia objektu");
			clear_clusters(clusters, i+1);
			fclose(f);
			return -1;
		}
		i++;
	}
	if(rep_flag)	//v pripade ze neni nejake id unikatne
	{
		if(clusters != NULL)
			clear_clusters(clusters, i);
		fclose(f);
		return -1;
	}
    	//ulozi ukazatl na pole shlukov do arr
    	*arr = clusters;
	fclose(f);
	return i;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*
 funkcia na vymazanie clusterov a uvolnenie pamati
*/
void clear_clusters(struct cluster_t *c, int narr)
{
	assert(c);
	assert(narr >= 0);

	for(int i = 0; i < narr; i++)
		clear_cluster(&c[i]);
	free(c);
}


/*
 Kontrola argumentov
*/
int check_args_all(char *str_p[], int c, int *h, int *size)
{
	char *check_str;
    //chyba nazov suboru
	if(c < 2)
	{
		*h = 1;
		return 0;
	}
    //uzivatel zadal pocet zhlukov
	if(c >= 3)
	{
        //nacitanie poctu zhlukov + kontrola
		*size = strtol(str_p[2], &check_str, 10);
		if(*check_str != 0 || *size <= 0)
		{
			std_err("Zadal si zle cislo");
			return 1;
		}
	} else *size = DEFAULT_SIZE;	//uzivatel nezdal pocet shlukov
	return 0;
}

int main(int argc, char *argv[])
{
    	struct cluster_t *clusters;
	int help = 0, user_clc, cl_count, ic1 = 0, ic2 = 0, ic1s;
    	if(check_args_all(argv, argc, &help, &user_clc) == 0)	//kontrola argumentov
    	{
    		if(help)	//napoveda
		{
			print_help;
			return EXIT_SUCCESS;
		}
        //nacitanie zhlukov
		if((cl_count = load_clusters(argv[1], &clusters)) < 0)
		      return EXIT_FAILURE;
		//znizenie poctu zhlukov na uzivatelom zadany pocet
		if(cl_count > user_clc)
		{
			while(cl_count != user_clc)
			{
                //najde dva najblizsie zhluky
				find_neighbours(clusters, cl_count, &ic1, &ic2);
                //presun objektov druheho zhluku do prveho
                ic1s = clusters[ic1].size;
				merge_clusters(&clusters[ic1], &clusters[ic2]);
                //kontrla presunutia objektov
    			if(clusters[ic1].size != ic1s + clusters[ic2].size)
				{
					std_err("Chyba alokacie pamate");
					break;
				}
                //zmaze druhy zhluk
				cl_count = remove_cluster(clusters, cl_count, ic2);
			}
		}
        //tisk zhlukov
		print_clusters(clusters, cl_count);
        //uvolnenie pamate
		clear_clusters(clusters, cl_count);
		return EXIT_SUCCESS;
	}
    //pri ocakavanom fungovani sa sem program nedostane
	return EXIT_FAILURE;
}
