/**
  @mainpage Projekt 3

  @link
  proj3.h
  @endlink

  @brief Program uskutocnuje zhlukovu analyzu,
  metodou najblizsieho suseda. Uzivatel
  zadava subor obsahujuci objeky
  (v tvare ID X Y) a pocet zhlukov. Kde ID
  je jednoznacny identifikator objektu a 0<=X<=1000 a
  0<=Y<=1000

  @author Boris Strbak, xstrba05

  @date 11 dec 2018

  @file proj3.h
  Program sa spusta v tvare:<br>
  ./proj3 objekty [N]<br>
    objekty -> subor s objektami<br>
    N -> pocet zhlukov (volitelny parameter)<br>
    N je implicitne nastavene na 1.<br><br>
    Vzorovy tvar suboru:<br>
    count=N(musi byt v danom tvare a N ->
    pocet objektov v subore)<br>
    1 20 2<br>
    2 30 4<br>
 */

/**
  @defgroup struct Zakladne struktuy programu
  @ingroup struct
  @{
 */

/**
  @brief Struktura objektu obsahujuca suradnice.
 */
struct obj_t {
    /** jednoznacny identifikator objektu */
    int id;
    /** x-ova suradnica */
    float x;
    /** y-ova suradnica */
    float y;
};

/**
  @brief Struktura zhluku obsahujuca objekty.
 */
struct cluster_t {
    /** Velkost zhluku, t.j. pocet objektov */
    int size;
    /** Kapacita zhluku */
    int capacity;
    /** pole objektov daneho zhluku */
    struct obj_t *obj;
};

/**
  @}
  @defgroup clust Praca s jednotlivymi zhlukmi
  @ingroup clust
  @{
 */

/**
  Inicializacia zhluku 'c'. Allokuje pamat pre 'cap' objektov.
  Ukazatel NULL pri poli objektov znamena kapacitu 0.

  @pre
  'cap' >= 0

  @post
  Zhluk bude mat allokovanu pamat pre 'cap' objektov.
  Velkost zhluku bude 0.

  @param c zhuk urceny na inicializaciu
  @param cap capacita zhluku
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 Uvolni pamat alokovanu pre objekty v zhluku 'c'.
 Inicializuje zhluk na nulovu kapacitu.

 @pre
 c != NULL

 @post
 Pamat alokovana pre objekty v zhluku 'c' bude uvolnena.

 @param c zhluk pre vycistenie
 */
void clear_cluster(struct cluster_t *c);

/**
  @brief Konstanta odporucana pre realokaciu pola objektov
 */
extern const int CLUSTER_CHUNK;

/**
  Zmeni kapacitu zhluku 'c' na 'new_cap' a realokuje
  pamat pre dany pocet objektov.

  @pre
  kapacita zhluku bude vacsia alebo rovna 0

  @post
  Kapacita zhluku bude rovna 'new_cap', pokial nenastane
  chyba pri realokacii.

  @param c zhluk urceny na realokaciu objektov
  @param new_cap nova kapacita zhluku
  @return novy zhluk, v pripade chyby NULL
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
  Pridava objekt 'obj' na koniec zhluku 'c'.
  Pokial sa objekt nezmesti do zhluku, tak
  zvacsi zhluk.

  @pre
  c != NULL

  @post
  'obj' bude na konci zhluku 'c' a velkost tohto zhluku sa zvacsi o 1.

  @param c zhluk, do ktoreh sa pridava objekt
  @param obj objet, ktory sa pridava na koniec zhluku
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);
/**
  @}
  @defgroup clusts Praca s viacerymi zhlukmi
  @ingroup clusts
  @{
*/

/**
  Spoji dva zhluky. Zkopiruje objekty zo zhluku 'c2' do zhluku
  'c1'. Zhluk 'c2' zostava nezmeneny.

  @pre
  c1 != NULL
  c2 != NULL

  @post
  Velkost zhluku 'c1' sa zvacsi o velkost zhluku 'c2' a objekty
  zhluku 'c2' budu priradene na koniec zhluku 'c1'. Zhluk 'c2' je
  nezeneny.

  @param c1 zhluk, do ktoreho sa kopiruju objekty
  @param c2 zhluk, z ktoreho sa kopiruju objekty
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
/**
  @}
  @ingroup clust
  @{
 */


/**
  Odstrani zhluk z pola zhlukov 'carr', ktore obsahuje 'narr'
  poloziek. Zhluk lezi na indexe 'idx'.

  @pre
  carr != NULL
  narr > 0
  idx < narr

  @post
  Polozka na indexe 'idx' bude odstranena z pola.
  Vsetky polozky za indexom budu posunute o jeden
  index dolava. Pocet poloziek v 'carr' bude o jeden
  mensi.

  @param carr pole zhlukov
  @param narr pocet poloziek pola
  @param idx index zhluku, ktory sa odstrani

  @return novy pocet zhlukov
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);
/**
  @}
  @ingroup clusts
  @{
 */

/**
  Pocita euklidovsku vzdialenost dvoch objektov.

  @pre
  o1 != NULL
  o2 != NULL

  @param o1 objekt
  @param o2 objekt
  @return vzdialenost dvoch objektov
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
  Pomocou funkcie 'obj_distance' pocita vzdialenost dvoch zhlukov.
  Ich vzdialenost je rovna vzdialenosti najblizsich dvoch objektov
  jednotlivych zhlukov.

  @pre
  c1 != NULL
  c2 != NULL

  @param c1 zhluk
  @param c2 zhluk
  @return vzdialenost zhlukov
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 Pomocou funkcie 'cluster_distance' hlada dva najblizsie zhluky v poli
 zhlukov 'carr' o velkosti 'narr'. Indexy najblizsich zhlukov uklada do pamate
 kam ukazuju 'c1' a 'c2'.

 @pre
 carr != NULL
 narr > 1
 c1 != NULL
 c2 != NULL

 @post
 Hodnota v pamati, kam ukazuju 'c1' a 'c2' budu rovne indexom najdenych
 zhlukov.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
  @}
  @ingroup clust
  @{
 */

 /**
  Zoradi objekty v zhluku vzostupne podla ich indexov.

  @pre
  c != NULL

  @post
  Objekty v zhluku budu zoradene vzostupne podla ich indexov.
  @param c zhluk, ktoreho objekty budu zoradene
  */
void sort_cluster(struct cluster_t *c);

/**
  Vytlaci zhluk, resp. jeho objekty na standardny vystup.

  @pre
  c != NULL
 */
void print_cluster(struct cluster_t *c);

/**
  @}
  @ingroup clusts
  @{
 */

/**
  Nacitava ojekty zo suboru 'filename', pre kazdy inicializuje
  zhluk, ktory pridava do pola, na ktore ukazuje 'arr'. Alokuje
  miesto pre pole vsetkych shlukov a ukazatel na prvni polozku
  ulozi do pamati. V pripade chyby uklada do pamati NULL.

  @pre
  Existuje subor pod nazvom 'filename' a podarilo sa ho otvorit

  @post
  Bude alokovany priestor pre pocet zhlukov uvedeny v subore.
  Pre kazdy objekt v subore bude vytvoreny cluster a objekt
  bude do neho pridany. V pripade chyby bude 'arr' ukazovat na NULL.
  Subor bude zatvoreny.

  @param filename meno suboru s objektmi.
  @param arr ukazatel na pole zhlukov, v pripade chyby na NULL
  @return pocet nacitanych objektov alebo v pripade chyby -1)
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 Tlaci pole zhlukov na standardny vystup.

 @pre
 carr != NULL
 narr > 0

 @post
 Zhluky budu vypisane na stdout

 @param carr pole zhlukov
 @param narr velkost pola
 */
void print_clusters(struct cluster_t *carr, int narr);
/**@}*/
