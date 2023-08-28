//Alessandro Conti

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//DEFINE
/**
 * Define --> Lunghezza massima che un comando può assumere.
 */
#define LUNGHEZZA_MAX_COMANDI 20

/**
 * Define --> Dimensione iniziale della HashTable che rappresenta il parco auto.
 */
#define DIMENSIONE_INIZIALE_PARCO_AUTO 13

/**
 * Define --> Dimensione iniziale della HashTable che rappresenta l'autostrada.
 */
#define DIMENSIONE_INIZIALE_AUTOSTRADA 31

/**
 * Define --> Percentuale oltre al quale andrà reallocato la HashTable.
 */
#define PERCENTUALE_REALLOC 0.75

/**
 * Define --> Costante di Knuth, serve per la funzione di hash.
 */
#define COSTANTE_DI_KNUTH 2654435769



//STRUCT
/**
 * Struct --> Rappresenta un'auto.
 */
struct HashNodeAuto{
    /**
     * Autonomia dell'auto.
     */
    unsigned int autonomia;
    /**
     * Nodo successivo nella stessa posizione della HashTable.
     */
    struct HashNodeAuto *successivo;
};

/**
 * Struct --> Rappresenta l'HashTable del parco auto.
 */
struct HashTableParcoAuto{
    /**
     * Auto.
     */
    struct HashNodeAuto **autoNode;
    /**
     * Numero di elementi nella HashTable.
     */
    unsigned int dimensione;
    /**
     * Capacità massima della HastTable, poi andra reallocata.
     */
    unsigned int capacita;
};

/**
 * Struct --> Rappresenta una stazione.
 */
struct HashNodeStazione{
    /**
     * Distanza dall'origine in cui si trova la stazione.
     */
    unsigned int distanza;
    /**
     * Parco auto.
     */
    struct HashTableParcoAuto *parcoAuto;
    /**
     * Autonomia massima dell'auto presente nel parco auto della stazione.
     */
    unsigned int autonomiaMassima;
    /**
     * Nodo successivo nella stessa posizione della HashTable.
     */
    struct HashNodeStazione *successivo;
};

/**
 * Struct --> Rappresenta l'HashTable dell'autostrada.
 */
struct HashTableAutostrada{
    /**
     * Stazioni.
     */
    struct HashNodeStazione **stazioni;
    /**
     * Numero di elementi nella HashTable.
     */
    unsigned int dimensione;
    /**
     * Capacità massima della HastTable, poi andra reallocata.
     */
    unsigned int capacita;
};

/**
 * Struct --> Rappresenta una posizione nell'Array che contiene tutte le stazioni tra quella di partenza e quella di arrivo.
 */
struct ArrayNodeStazione{
    /**
     * Distanza a cui si trova la stazione.
     */
    unsigned int distanza;
    /**
     * Autonomia dell'auto massima presente nel parco auto.
     */
    unsigned int autonomiaMassima;
};



//VARIABILI GLOBALI
/**
 * Variabile Globale --> Rappresenta l'autostrada.
 */
struct HashTableAutostrada *autostrada=NULL;



//GESTIONE HASH
/**
 * Funzione di hash --> ritorna la posizione dato il valore richiesto.
 *
 * @param chiave valore d'interesse.
 * @param capacita capacità della HashTable.
 * @return la posizione della chiave nella HashTable.
 */
unsigned int funzioneDiHash(unsigned int chiave, unsigned int capacita){
    //(chiave * Knuth) mod capacità
    //funzione di hash di moltiplicazione modulare utilizzando la costante di Knuth
    return (int) ((long)(chiave * COSTANTE_DI_KNUTH) % capacita);
}

/**
 * Ritorna la nuova capacità della HashTable.
 *
 * @param capacitaIniziale capacità iniziale della HashTable.
 * @return la nuova capacità della HashTable.
 */
int nuovaCapacita(unsigned int capacitaIniziale){
    //capacità * 2 + 11 --> maggiore probabilità di ottenere numeri primi
    //ritorna la nuova capacità della HashTable
    return (int) capacitaIniziale*2+11;
}



//GESTIONE PARCO AUTO
/**
 * Creazione del parco auto.
 *
 * @param capacitaIniziale la capacità iniziale del parco auto.
 * @return il parco auto creato.
 */
struct HashTableParcoAuto *creaParcoAuto(unsigned int capacitaIniziale){
    /**
     * Nuova HashTable.
     */
    struct HashTableParcoAuto *hashTable=(struct HashTableParcoAuto *) malloc(sizeof(struct HashTableParcoAuto));


    //inizializzare i valori
    hashTable->dimensione=0;
    hashTable->capacita=capacitaIniziale;
    hashTable->autoNode=(struct HashNodeAuto **) calloc(capacitaIniziale, sizeof(struct HashNodeAuto *));

    //ritornare la HashTable inizializzata
    return hashTable;
}

/**
 * Reinizializza l'HashTable del parco auto.
 *
 * @param parcoAuto il parco auto da reallocare.
 */
void reHashParcoAuto(struct HashTableParcoAuto *parcoAuto){
    /**
     * Nuova capacità della HashTable.
     */
    int capacitaNuova= nuovaCapacita(parcoAuto->capacita);
    /**
     * Nuovo indice in cui spostare gli HashNode dalla HashTable vecchia a quella nuova.
     */
    unsigned int nuovoIndice;
    /**
     * Nuova HashTable.
     */
    struct HashNodeAuto **nuovaHashTable=(struct HashNodeAuto **) calloc(capacitaNuova, sizeof(struct HashNodeAuto *));
    /**
     * Nodo della HashTable corrente.
     */
    struct HashNodeAuto *corrente=NULL;
    /**
     * Nodo successivo da vedere.
     */
    struct HashNodeAuto *prossimo=NULL;


    //spostare dalla vecchia HashTable a quella nuova
    for (int i = 0; i < parcoAuto->capacita; ++i) {
        corrente=parcoAuto->autoNode[i];

        //fino a che NON si è finito di spostare tutte le HashNode
        while(corrente!=NULL){
            //salvare il prossimo HashNode da visitare
            prossimo=corrente->successivo;
            //salvare il nuovo indice in cui andare a mettere la HashNode corrente
            nuovoIndice= funzioneDiHash(corrente->autonomia, (unsigned int) capacitaNuova);

            //spostare il nodo
            corrente->successivo=nuovaHashTable[nuovoIndice];
            nuovaHashTable[nuovoIndice]=corrente;
            //il nodo corrente da vedere diventa il prossimo
            corrente=prossimo;
        }
    }

    //eliminare il vecchio parco auto
    free(parcoAuto->autoNode);
    //aggiungere il nuovo parco auto
    parcoAuto->autoNode=nuovaHashTable;
    //aggiornare la capacità
    parcoAuto->capacita=(unsigned int) capacitaNuova;
}

/**
 * Aggiungi auto al parco auto richiesto.
 *
 * @param parcoAuto il parco auto richiesto.
 * @param autonomia l'autonomia dell'auto da aggiungere al parco auto.
 */
void aggiungiAutoAlParcoAuto(struct HashTableParcoAuto *parcoAuto, unsigned int autonomia){
    /**
     * Indice in cui inserire la macchina.
     */
    unsigned int indice= funzioneDiHash(autonomia, parcoAuto->capacita);
    /**
     * Nuovo HashNode da inserire nella HashTable.
     */
    struct HashNodeAuto *nuovoNodo=(struct HashNodeAuto *) malloc(sizeof(struct HashNodeAuto));


    //inserire valori della nuova macchina
    nuovoNodo->autonomia=autonomia;
    nuovoNodo->successivo=parcoAuto->autoNode[indice];
    parcoAuto->autoNode[indice]=nuovoNodo;
    //è stato aggiunto un nuovo nodo, questo implica che bisogna aumentare la dimensione della HashTable
    parcoAuto->dimensione++;

    //se bisogna reallocare la HashTable
    if(parcoAuto->dimensione > (int) parcoAuto->capacita*PERCENTUALE_REALLOC){
        reHashParcoAuto(parcoAuto);
    }
}

/**
 * Elimina l'auto richiesta dal parco auto.
 *
 * @param parcoAuto il parco auto che contiene la macchina da eliminare.
 * @param autonomia la macchina da eliminare.
 */
void eliminaAutoDalParcoAuto(struct HashTableParcoAuto *parcoAuto, unsigned int autonomia){
    /**
     * Indice nel quale si trova la macchina.
     */
    unsigned int indice= funzioneDiHash(autonomia, parcoAuto->capacita);
    /**
     * Nodo corrente.
     */
    struct HashNodeAuto *corrente=parcoAuto->autoNode[indice];
    /**
     * Nodo precedente.
     */
    struct HashNodeAuto *precedente=NULL;


    //trovare il nodo con la chiave corrispondente, assegnare corrente AND precedente
    while (corrente!=NULL && corrente->autonomia!=autonomia){
        precedente=corrente;
        corrente=corrente->successivo;
    }

    //macchina NON trovata
    if(corrente==NULL){
        return;
    }
    //macchina trovata
    else{
        //il nodo da eliminare è il primo della lista presente nella posizione indicata
        if(precedente==NULL){
            parcoAuto->autoNode[indice]=corrente->successivo;
        }
        //il nodo da eliminare NON è il primo della lista presente nella posizione indicate
        else{
            precedente->successivo=corrente->successivo;
        }
    }

    //eliminare la macchina
    free(corrente);
    //ridurre il numero di elementi presenti nella HashTable
    parcoAuto->dimensione--;
}

/**
 * Ricerca l'auto richiesta nel parco auto.
 *
 * @param parcoAuto il parco auto che contiene la macchina da ricercare.
 * @param autonomia l'autonomia della macchina da ricercare.
 * @return 1 se la macchina è presente nel parco auto, 0 se NON è presente.
 */
int ricercaAutoNelParcoAuto(struct HashTableParcoAuto *parcoAuto, unsigned int autonomia){
    /**
     * Indice nel quale si trova la macchina.
     */
    unsigned int indice= funzioneDiHash(autonomia, parcoAuto->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNodeAuto *corrente=parcoAuto->autoNode[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //se la chiave è stata trovata
        if(corrente->autonomia==autonomia){
            //macchina trovata
            return 1;
        }
        //guardare nel prossimo HashNode
        else{
            corrente=corrente->successivo;
        }
    }

    //macchina NON trovata
    return 0;
}

/**
 * Ricerca l'autonomia massima nel parco auto richiesto.
 *
 * @param parcoAuto il parco auto d'interesse.
 * @param dimensioneMassima la dimensione che ha il parco auto - capacità attuale.
 * @return l'autonomia massima presente nel parco auto.
 */
unsigned int ricercaMassimaAutonomiaNelParcoAuto(struct HashTableParcoAuto *parcoAuto, unsigned int dimensioneMassima){
    /**
     * Valore massimo da ritornare.
     */
    unsigned int massimo=0;
    /**
     * HashNode corrente.
     */
    struct HashNodeAuto *corrente=NULL;


    //per tutta la dimensione della HashTable
    for (int i = 0; i < dimensioneMassima; ++i) {
        corrente=parcoAuto->autoNode[i];
        //controllare tutta la lista presente nella posizione
        while (corrente!=NULL){
            //se corrente è maggiore del massimo modificalo
            if(corrente->autonomia>massimo){
                //aggiornare il valore massimo da ritornare
                massimo=corrente->autonomia;
            }
            corrente=corrente->successivo;
        }
    }

    //ritornare la massima autonomia
    return massimo;
}



//GESTIONE AUTOSTRADA
/**
 * Creazione dell'autostrada.
 *
 * @param capacitaIniziale la capacità iniziale dell'autostrada.
 * @return l'autostrada creata.
 */
struct HashTableAutostrada *creaAutostrada(unsigned int capacitaIniziale){
    /**
     * Nuova HashTable.
     */
    struct HashTableAutostrada *hashTable=(struct HashTableAutostrada *) malloc(sizeof(struct HashTableAutostrada));


    //inizializzare i valori
    hashTable->capacita=capacitaIniziale;
    hashTable->dimensione=0;
    hashTable->stazioni=(struct HashNodeStazione **) calloc(hashTable->capacita, sizeof(struct HashNodeStazione *));

    //ritornare la HashTable inizializzata
    return hashTable;
}

/**
 * Reinizializza l'HashTable dell'autostrada.
 *
 * @param htAutostrada l'autostrada da reinizializzare.
 */
void reHashAutostrada(struct HashTableAutostrada *htAutostrada){
    /**
     * Nuova capacità della HashTable.
     */
    unsigned int capacitaNuova= nuovaCapacita(htAutostrada->capacita);
    /**
     * Nuovo indice in cui spostare gli HashNode dalla HashTable vecchia a quella nuova.
     */
    unsigned int nuovoIndice;
    /**
     * Nuova HashTable.
     */
    struct HashNodeStazione **nuovaHashTable=(struct HashNodeStazione **) calloc(capacitaNuova, sizeof(struct HashNodeStazione *));
    /**
     * Nodo della HashTable corrente.
     */
    struct HashNodeStazione *corrente=NULL;
    /**
     * Nodo successivo da vedere.
     */
    struct HashNodeStazione *prossimo=NULL;


    //spostare dalla vecchia HashTable a quella nuova
    for (int i = 0; i < htAutostrada->capacita; ++i) {
        corrente=htAutostrada->stazioni[i];

        //fino a che NON si è finito di spostare tutte le HashNode
        while(corrente!=NULL){
            //salvare il prossimo HashNode da visitare
            prossimo=corrente->successivo;
            //salvare il nuovo indice in cui andare a mettere la HashNode corrente
            nuovoIndice= funzioneDiHash(corrente->distanza, capacitaNuova);

            //spostare il nodo
            corrente->successivo=nuovaHashTable[nuovoIndice];
            nuovaHashTable[nuovoIndice]=corrente;
            //il nodo corrente da controllare diventa il prossimo
            corrente=prossimo;
        }
    }

    //eliminare la vecchia stazione
    free(htAutostrada->stazioni);
    //aggiungere la nuova stazione
    htAutostrada->stazioni=nuovaHashTable;
    //aggiornare la capacità
    htAutostrada->capacita=capacitaNuova;
}

/**
 * Aggiungi la stazione all'autostrada.
 *
 * @param htAutostrada l'autostrada a cui va aggiunta la stazione.
 * @param distanza la distanza a cui si trova la stazione che vogliamo aggiungere.
 * @param parcoAuto il parco auto contenuto nella stazione indicata.
 */
void aggiungiStazioneAllAutostrada(struct HashTableAutostrada *htAutostrada, unsigned int distanza, struct HashTableParcoAuto *parcoAuto, unsigned int maxAutonomia){
    /**
     * Indice in cui inserire la stazione.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * Nuovo HashNode da inserire nella HashTable.
     */
    struct HashNodeStazione *nuovoNodo=(struct HashNodeStazione *) malloc(sizeof(struct HashNodeStazione));


    //inserire i valori nel nuovo HashNode
    nuovoNodo->distanza=distanza;
    nuovoNodo->parcoAuto=parcoAuto;
    nuovoNodo->autonomiaMassima=maxAutonomia;
    nuovoNodo->successivo=htAutostrada->stazioni[indice];
    htAutostrada->stazioni[indice]=nuovoNodo;
    //è stato aggiunto un nuovo nodo, questo implica che bisogna aumentare la dimensione della HashTable
    htAutostrada->dimensione++;

    //se bisogna reallocare la HashTable
    if(htAutostrada->dimensione > htAutostrada->capacita*PERCENTUALE_REALLOC){
        reHashAutostrada(htAutostrada);
    }
}

/**
 * Elimina la stazione richiesta dall'autostrada.
 *
 * @param htAutostrada l'autostrada in cui si trova la stazione.
 * @param distanza la distanza a cui si trova la stazione da eliminare.
 */
void eliminaStazioneDallAutostrada(struct HashTableAutostrada *htAutostrada, unsigned int distanza){
    /**
     * Indice nel quale si trova la stazione.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * Stazione corrente.
     */
    struct HashNodeStazione *stazioneCorrente=htAutostrada->stazioni[indice];
    /**
     * Stazione precedente.
     */
    struct HashNodeStazione *stazionePrecedente=NULL;
    /**
     * Auto corrente presente in posizione i nel parco auto.
     */
    struct HashNodeAuto *autoCorrente;
    /**
     * Auto precedente da eliminare.
     */
    struct HashNodeAuto *autoPrecedente;


    //trovare il nodo con la chiave corrispondente, assegnare i valori a stazioneCorrente AND stazionePrecedente
    while (stazioneCorrente != NULL && stazioneCorrente->distanza != distanza){
        stazionePrecedente=stazioneCorrente;
        stazioneCorrente=stazioneCorrente->successivo;
    }

    //stazione NON trovata
    if(stazioneCorrente == NULL){
        return;
    }
    //stazione trovata
    else{
        //se il nodo da eliminare è il primo della lista presente nella posizione indicata
        if(stazionePrecedente == NULL){
            htAutostrada->stazioni[indice]=stazioneCorrente->successivo;
        }
        //se il nodo da eliminare NON è il primo della lista presente nella posizione indicate
        else{
            stazionePrecedente->successivo=stazioneCorrente->successivo;
        }
    }

    //eliminare le auto presenti nel parco auto della stazione
    for (int i = 0; i < stazioneCorrente->parcoAuto->capacita; ++i) {
        autoCorrente=stazioneCorrente->parcoAuto->autoNode[i];
        while (autoCorrente != NULL){
            autoPrecedente=autoCorrente;
            autoCorrente=autoCorrente->successivo;
            //eliminare l'auto
            free(autoPrecedente);
        }
    }
    //eliminare il parcoAuto
    free(stazioneCorrente->parcoAuto->autoNode);
    //eliminare l'HashTable che rappresenta il parco auto
    free(stazioneCorrente->parcoAuto);
    //eliminare la stazione
    free(stazioneCorrente);
    //ridurre il numero di elementi presenti nella HashTable
    htAutostrada->dimensione--;
}

/**
 * Ricerca la stazione nell'autostrada alla distanza data.
 *
 * @param htAutostrada l'autostrada su cui eseguire la ricerca.
 * @param distanza la distanza a cui si trova la stazione interessate.
 * @return la stazione ricercata, se esiste, altrimenti ritorna NULL.
 */
struct HashNodeStazione *ricercaStazioneNellAutostrada(struct HashTableAutostrada *htAutostrada, unsigned int distanza){
    /**
     * Indice nel quale si trova la stazione.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNodeStazione *corrente=htAutostrada->stazioni[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //se è stata trovata la stazione
        if(corrente->distanza==distanza){
            return corrente;
        }
        //guardare nel prossimo HashNode
        else{
            corrente=corrente->successivo;
        }
    }

    //stazione NON trovata
    return NULL;
}

/**
 * Ritorna 1 se la stazione esiste sull'autostrada.
 *
 * @param htAutostrada l'autostrada su cui eseguire la ricerca.
 * @param distanza distanza in cui si trova la stazione di interesse.
 * @return 1 se la stazione esiste sull'autostrada, 0 altrimenti
 */
int stazioneGiaPresenteNellAutostrada(struct HashTableAutostrada *htAutostrada, unsigned int distanza){
    /**
     * Indice nel quale si trova la stazione.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNodeStazione *corrente=htAutostrada->stazioni[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //se è stata trovata la stazione
        if(corrente->distanza==distanza){
            return 1;
        }
        //guardare nel prossimo HashNode
        else{
            corrente=corrente->successivo;
        }
    }

    //stazione NON trovata
    return 0;
}



//COMPARAZIONE
/**
 * Funzione che compara 2 nodi - comparazione in ordine crescente.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la differenza tra il nodo a e il nodo b.
 */
int comparazioneCrescente(const void *a, const void *b){
    return (int) (((struct ArrayNodeStazione *)a)->distanza-((struct ArrayNodeStazione *)b)->distanza);
}

/**
 * Funzione che compara 2 nodi - comparazione in ordine decrescente.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la differenza tra il nodo b e il nodo a.
 */
int comparazioneDecrescente(const void *a, const void *b){
    return (int) (((struct ArrayNodeStazione *)b)->distanza-((struct ArrayNodeStazione *)a)->distanza);
}



//PIANIFICA PERCORSO
/**
 * Indice della stazione data la distanza - ordine crescente.
 *
 * @param stazioni stazioni da controllare.
 * @param numeroStazioni numero di stazioni presenti.
 * @param distanza distanza da ricercare.
 * @return indice della stazione con distanza ricercata.
 */
int indiceStazioneOrdineCrescente(struct ArrayNodeStazione stazioni[], int numeroStazioni, unsigned int distanza){
    /**
     * Indice parte sinistra da controllare.
     */
    int sx=0;
    /**
     * Indice parte destra da controllare.
     */
    int dx=numeroStazioni-1;
    /**
     * Indice punto medio da controllare.
     */
    int medio;


    //fino a che sx è minore uguale a dx
    while (sx<=dx){
        //indice da controllare
        medio=((int) sx+((int) ((dx-sx)/2)));

        //controllare se il valore è uguale a quello cercato
        if(stazioni[medio].distanza==distanza){
            return medio;
        }
        //il valore da ricercare si trova nella metà superiore
        else if(stazioni[medio].distanza<distanza){
            sx=medio+1;
        }
        //il valore da ricercare si trova nella metà inferiore
        else{
            dx=medio-1;
        }
    }

    //indice NON trovato
    return -1;
}

/**
 * Indice della stazione data la distanza - ordine crescente.
 *
 * @param stazioni stazioni da controllare.
 * @param numeroStazioni numero di stazioni presenti.
 * @param distanza distanza da ricercare.
 * @return indice della stazione con distanza ricercata.
 */
int indiceStazioneOrdineDecrescente(struct ArrayNodeStazione stazioni[], int numeroStazioni, unsigned int distanza){
    /**
     * Indice parte sinistra da controllare.
     */
    int sx=0;
    /**
     * Indice parte destra da controllare.
     */
    int dx=numeroStazioni-1;
    /**
     * Indice punto medio da controllare.
     */
    int medio;


    //fino a che sx è minore uguale a dx
    while (sx<=dx){
        //indice da controllare
        medio=((int) sx+((int) ((dx-sx)/2)));

        //controllare se il valore è uguale a quello cercato
        if(stazioni[medio].distanza==distanza){
            return medio;
        }
        //il valore da ricercare si trova nella metà superiore
        else if(stazioni[medio].distanza>distanza){
            sx=medio+1;
        }
        //il valore da ricercare si trova nella metà inferiore
        else{
            dx=medio-1;
        }
    }

    //indice NON trovato
    return -1;
}

/**
 * Indice della stazione più lontana data l'autonomia massima della macchina.
 *
 * @param stazioni stazioni da controllare.
 * @param numeroStazioni numero di stazioni da controllare.
 * @param autonomiaMassima autonomia massima dell'auto.
 * @return indice della stazione più lontana.
 */
int stazionePiuLontana(struct ArrayNodeStazione stazioni[], int numeroStazioni, unsigned int autonomiaMassima){
    //ricercare per tutte le stazioni
    for (int i = 1; i < numeroStazioni; ++i) {
        //se la distanza tra la stazione corrente e quella iniziale è maggiore dell'autonomia massima
        if(autonomiaMassima<abs((int) stazioni[i].distanza-(int) stazioni[0].distanza)){
            //ritornare la posizione nell'Array
            return --i;
        }
    }
    //ritornare la posizione nell'Array - posizione finale
    return (numeroStazioni-1);
}

/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo - partenza < arrivo.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene tutte le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto presente nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioniInAvanti(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    /**
     * Dimensione iniziale array.
     */
    int x=1000;
    /**
     * Array delle stazioni con la loro autonomia massima.
     */
    struct ArrayNodeStazione *stazioniIntermedie=(struct ArrayNodeStazione *) calloc(x, sizeof(struct ArrayNodeStazione));
    /**
     * Posizione libera nell'Array stazioniIntermedie.
     */
    int posizioneArray=0;
    /**
     * Stazione ricercata.
     */
    struct HashNodeStazione *ricerca=NULL;
    /**
     * Nodo corrente presente nella cella i.
     */
    struct HashNodeStazione *corrente;


    //controllare solo le stazioni comprese tra partenza e arrivo
    if(((int) arrivo-partenza) < autostrada->dimensione){
        //per ogni stazione che ci potrebbe essere tra partenza e arrivo
        for (unsigned int i = partenza; i <= arrivo; ++i) {
            //salvare la stazione
            ricerca= ricercaStazioneNellAutostrada(autostrada, i);
            //se la stazione esiste la aggiungiamo alle stazioniIntermedie[]
            if(ricerca!=NULL){
                //se l'Array è pieno bisogna reallocarlo
                if(posizioneArray==x){
                    x=x*2;
                    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x*sizeof(struct ArrayNodeStazione)));
                }
                //aggiungere la stazione
                stazioniIntermedie[posizioneArray].distanza= ricerca->distanza;
                stazioniIntermedie[posizioneArray].autonomiaMassima=ricerca->autonomiaMassima;
                posizioneArray++;
            }
        }
    }
    //controllare tutte le stazioni presenti nell'autostrada e aggiungere all'array solo quelle comprese tra la partenza e l'arrivo
    else{
        //per tutte le stazioni presenti sull'autostrada
        for (int i = 0; i < autostrada->capacita; ++i) {
            //nella cella i c'è almeno 1 stazione
            if(autostrada->stazioni[i]!=NULL){
                //stazione corrente
                corrente=autostrada->stazioni[i];
                while (corrente!=NULL){
                    if(corrente->distanza>=partenza && corrente->distanza<=arrivo){
                        //se l'Array è pieno bisogna reallocarlo
                        if(posizioneArray==x){
                            x=x*2;
                            stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x*sizeof(struct ArrayNodeStazione)));
                        }
                        //aggiungere la stazione
                        stazioniIntermedie[posizioneArray].distanza= corrente->distanza;
                        stazioniIntermedie[posizioneArray].autonomiaMassima=corrente->autonomiaMassima;
                        posizioneArray++;
                    }
                    corrente=corrente->successivo;
                }
            }
        }

        //ordinare l'array stazioniIntermedie - partenza --> arrivo
        qsort(stazioniIntermedie, posizioneArray, sizeof(struct ArrayNodeStazione), comparazioneCrescente);
    }

    //numero di stazioni tra quella di partenza e quella d'arrivo
    (*numeroDiStazioni)=posizioneArray;
    //stazioni intermedie tra quella di partenza e quella di arrivo
    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (*numeroDiStazioni)*sizeof(struct ArrayNodeStazione));
    return stazioniIntermedie;
}

/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo - partenza > arrivo.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene tutte le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto presente nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioniAllIndietro(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    /**
     * Dimensione iniziale array.
     */
    int x=1000;
    /**
     * Array delle stazioni con la loro autonomia massima.
     */
    struct ArrayNodeStazione *stazioniIntermedie=(struct ArrayNodeStazione *) calloc(x, sizeof(struct ArrayNodeStazione));
    /**
     * Posizione libera nell'Array stazioniIntermedie.
     */
    int posizioneArray=0;
    /**
     * Stazione ricercata.
     */
    struct HashNodeStazione *ricerca=NULL;
    /**
     * Nodo corrente presente nella cella i.
     */
    struct HashNodeStazione *corrente;


    //controllare solo le stazioni comprese tra partenza e arrivo
    if(((int) arrivo-partenza) < autostrada->dimensione){
        //per ogni stazione che ci potrebbe essere tra partenza e arrivo
        for (int i = (int) partenza; i >= (int) arrivo; --i) {
            //ricercare la stazione
            ricerca = ricercaStazioneNellAutostrada(autostrada, i);
            //se la stazione esiste aggiungerla all'array
            if (ricerca != NULL) {
                //se l'Array è pieno bisogna reallocarlo
                if (posizioneArray == x) {
                    x=x*2;
                    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x * sizeof(struct ArrayNodeStazione)));
                }
                //aggiungere la stazione
                stazioniIntermedie[posizioneArray].distanza = ricerca->distanza;
                stazioniIntermedie[posizioneArray].autonomiaMassima = ricerca->autonomiaMassima;
                posizioneArray++;
            }
        }
    }
    //controllare tutte le stazioni presenti nell'autostrada e aggiungere all'array solo quelle comprese tra la partenza e l'arrivo
    else{
        //per tutte le stazioni presenti sull'autostrada
        for (int i = 0; i < autostrada->capacita; ++i) {
            //se nella cella i c'è almeno 1 stazione
            if(autostrada->stazioni[i]!=NULL){
                //stazione corrente
                corrente=autostrada->stazioni[i];

                while (corrente!=NULL){
                    if(corrente->distanza<=partenza && corrente->distanza>=arrivo){
                        //se l'Array è pieno bisogna reallocarlo
                        if(posizioneArray==x){
                            x=x*2;
                            stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x*sizeof(struct ArrayNodeStazione)));
                        }
                        //aggiungere la stazione
                        stazioniIntermedie[posizioneArray].distanza= corrente->distanza;
                        stazioniIntermedie[posizioneArray].autonomiaMassima=corrente->autonomiaMassima;
                        posizioneArray++;
                    }
                    corrente=corrente->successivo;
                }
            }
        }

        //ordinare l'array stazioniIntermedie - partenza --> arrivo
        qsort(stazioniIntermedie, posizioneArray, sizeof(struct ArrayNodeStazione), comparazioneDecrescente);
    }

    //numero di stazioni tra quella di partenza e quella d'arrivo
    (*numeroDiStazioni)=posizioneArray;
    //stazioni intermedie tra quella di partenza e quella di arrivo
    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, ((*numeroDiStazioni)*sizeof(struct ArrayNodeStazione)));
    return stazioniIntermedie;
}

/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene tutte le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto presente nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioni(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    //ritornare le stazioni comprese tra la partenza e l'arrivo
    if(partenza<arrivo){
        return tutteLeStazioniInAvanti(partenza, arrivo, numeroDiStazioni);
    }
    else{
        return tutteLeStazioniAllIndietro(partenza, arrivo, numeroDiStazioni);
    }
}

/**
 * Pianifica il percorso ottimale in avanti - partenza < arrivo.
 *
 * @param stazioni stazioni tra quella di partenza e quella di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @param partenza stazione di partenza.
 * @param arrivo stazione di arrivo.
 * @return percorso tra la stazione di partenza e quella di arrivo, NULL se NON esiste.
 */
int *percorsoPianificatoInAvanti(struct ArrayNodeStazione stazioni[], int numeroDiStazioni, int partenza, int arrivo){
    /**
     * Percorso ottimale da ritornare.
     */
    int *percorsoTrovato=(int *) calloc(numeroDiStazioni, sizeof(int));
    /**
     * Indice dell'array percorsoTrovato.
     */
    int indicePercorsoTrovato=0;
    /**
     * Nuova dimensione del percorso da ritornare - per realloc.
     */
    int nuovaDimensionePercorsoTrovato;
    /**
     * Indice stazione corrente.
     */
    int indiceCorrente=0;
    /**
     * Indice della stazione più lontana data la stazione corrente.
     */
    int indiceStazionePiuLontana= stazionePiuLontana(stazioni, numeroDiStazioni, stazioni[0].autonomiaMassima);
    /**
     * Indice della stazione da superare.
     */
    int indiceStazioneDaSuperare;
    /**
     * Indice più vicino alla partenza - serve nel momento in cui bisogna scegliere le stazioni con distanza minore.
     */
    int a;
    /**
     * Indice più vicino all'arrivo - serve nel momento in cui bisogna scegliere le stazioni con distanza minore.
     */
    int b;


    //aggiungere la stazione di partenza al percorso ottimale da ritornare
    percorsoTrovato[indicePercorsoTrovato]=partenza;
    ++indicePercorsoTrovato;

    //se dalla stazione di partenza si arriva direttamente a quella di arrivo
    if (stazioni[0].autonomiaMassima>= abs(arrivo-partenza)){
        percorsoTrovato[indicePercorsoTrovato]=arrivo;
        return percorsoTrovato;
    }

    //fino a che NON siamo arrivati alla stazione di arrivo
    while (stazioni[indiceStazionePiuLontana].distanza!=arrivo){
        indiceStazioneDaSuperare=indiceStazionePiuLontana+ stazionePiuLontana((stazioni+indiceStazionePiuLontana), (numeroDiStazioni-indiceStazionePiuLontana), stazioni[indiceStazionePiuLontana].autonomiaMassima);
        //ricercare la migliore stazione fino a questo momento
        for (int i = indiceStazionePiuLontana-1; i > indiceCorrente; --i) {
            if(stazioni[i].autonomiaMassima>=(stazioni[indiceStazioneDaSuperare].distanza-stazioni[i].distanza)){
                indiceStazionePiuLontana=i;
                indiceStazioneDaSuperare=indiceStazionePiuLontana+ stazionePiuLontana((stazioni+indiceStazionePiuLontana), (numeroDiStazioni-indiceStazionePiuLontana), stazioni[indiceStazionePiuLontana].autonomiaMassima);
            }
        }

        //aggiungere la stazione più lontana al percorso ottimale da ritornare
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorsoTrovato= indicePercorsoTrovato * 2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorsoTrovato * sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=(int) stazioni[indiceStazionePiuLontana].distanza;
        ++indicePercorsoTrovato;

        //se la stazione più lontana è quella massima ==> percorso NON trovato
        if (indiceStazionePiuLontana==indiceStazioneDaSuperare){
            //percorso NON trovato
            free(percorsoTrovato);
            return NULL;
        }

        indiceCorrente=indiceStazionePiuLontana;
        indiceStazionePiuLontana=indiceStazioneDaSuperare;
    }

    //se è possibile arrivare alla stazione di arrivo
    if (stazioni[indiceStazioneOrdineCrescente(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato - 1])].autonomiaMassima >= (arrivo - percorsoTrovato[indicePercorsoTrovato - 1])){
        //aggiungere la stazione di arrivo al percorso ottimale da ritornare
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorsoTrovato= indicePercorsoTrovato * 2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorsoTrovato * sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=arrivo;
    }
    //percorso NON trovato
    else{
        free(percorsoTrovato);
        return NULL;
    }

    //scegliere le stazioni più vicine all'origine
    while(indicePercorsoTrovato>1){
        //A -- stazione da controllare -- B
        //limite inferiore per il controllo delle stazioni più vicine all'origine
        a= indiceStazioneOrdineCrescente(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato - 2]);
        //limite superiore per il controllo delle stazioni più vicine all'origine
        b= indiceStazioneOrdineCrescente(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato]);

        //controllare tutte le stazioni dal punto B al punto A - ricercare se tra i 2 punti esiste una stazioni più piccola
        for (int i = b-1; i > a; --i) {
            //A.autonomia >= medio.distanza - A.distanza
            //medio.autonomia >= B.distanza - medio.distanza
            if (stazioni[a].autonomiaMassima>= abs((int) stazioni[i].distanza-(int) stazioni[a].distanza) &&
                stazioni[i].autonomiaMassima>= abs((int) stazioni[b].distanza-(int) stazioni[i].distanza)){
                percorsoTrovato[indicePercorsoTrovato-1]=(int) stazioni[i].distanza;
            }
        }
        --indicePercorsoTrovato;
    }

    //percorso trovato
    return percorsoTrovato;
}

/**
 * Pianifica il percorso ottimale all'indietro - partenza > arrivo.
 *
 * @param stazioni stazioni tra quella di partenza e quella di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @param partenza stazione di partenza.
 * @param arrivo stazione di arrivo.
 * @return percorso tra la stazione di partenza e quella di arrivo, NULL se NON esiste.
 */
int *percorsoPianificatoAllIndietro(struct ArrayNodeStazione stazioni[], int numeroDiStazioni, int partenza, int arrivo){
    /**
     * Percorso ottimale da ritornare.
     */
    int *percorsoTrovato=(int *) calloc(numeroDiStazioni, sizeof(int));
    /**
     * Indice dell'array percorsoTrovato.
     */
    int indicePercorsoTrovato=0;
    /**
     * Nuova dimensione del percorso da ritornare - per realloc.
     */
    int nuovaDimensionePercorsoTrovato;
    /**
     * Indice della stazione corrente.
     */
    int indiceCorrente=0;
    /**
     * Indice della stazione più lontana data la stazione corrente.
     */
    int indiceStazionePiuLontana= stazionePiuLontana(stazioni, numeroDiStazioni, stazioni[0].autonomiaMassima);
    /**
     * Indice della stazione da superare.
     */
    int indiceStazioneDaSuperare;
    /**
     * Indice più vicino alla partenza - serve nel momento in cui bisogna scegliere le stazioni con distanza minore.
     */
    int a;
    /**
     * Indice più vicino all'arrivo - serve nel momento in cui bisogna scegliere le stazioni con distanza minore.
     */
    int b;


    //aggiungere la stazione di partenza al percorso ottimale da ritornare
    percorsoTrovato[indicePercorsoTrovato]=partenza;
    ++indicePercorsoTrovato;

    //se dalla stazione di partenza si arriva direttamente a quella di arrivo
    if (stazioni[0].autonomiaMassima>= abs(arrivo-partenza)){
        percorsoTrovato[indicePercorsoTrovato]=arrivo;
        return percorsoTrovato;
    }

    //fino a che NON siamo arrivati alla stazione di arrivo
    while (stazioni[indiceStazionePiuLontana].distanza!=arrivo){
        indiceStazioneDaSuperare=indiceStazionePiuLontana+ stazionePiuLontana((stazioni+indiceStazionePiuLontana), (numeroDiStazioni-indiceStazionePiuLontana), stazioni[indiceStazionePiuLontana].autonomiaMassima);
        //ricercare la migliore stazione fino a questo momento
        for (int i = indiceStazionePiuLontana-1; i > indiceCorrente; --i) {
            if(stazioni[i].autonomiaMassima>=abs((int) stazioni[indiceStazioneDaSuperare].distanza-(int) stazioni[i].distanza)){
                indiceStazionePiuLontana=i;
                indiceStazioneDaSuperare=indiceStazionePiuLontana+ stazionePiuLontana((stazioni+indiceStazionePiuLontana), (numeroDiStazioni-indiceStazionePiuLontana), stazioni[indiceStazionePiuLontana].autonomiaMassima);
            }
        }

        //aggiungere la stazione più lontana al percorso ottimale da ritornare
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorsoTrovato= indicePercorsoTrovato * 2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorsoTrovato * sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=(int) stazioni[indiceStazionePiuLontana].distanza;
        ++indicePercorsoTrovato;

        //se la stazione più lontana è quella massima ==> percorso NON trovato
        if (indiceStazionePiuLontana==indiceStazioneDaSuperare){
            //percorso NON trovato
            free(percorsoTrovato);
            return NULL;
        }

        indiceCorrente=indiceStazionePiuLontana;
        indiceStazionePiuLontana=indiceStazioneDaSuperare;
    }

    //se è possibile arrivare alla stazione di arrivo
    if (stazioni[indiceStazioneOrdineDecrescente(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato - 1])].autonomiaMassima >= abs(arrivo - percorsoTrovato[indicePercorsoTrovato - 1])){
        //aggiungere la stazione di arrivo al percorso ottimale da ritornare
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorsoTrovato= indicePercorsoTrovato * 2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorsoTrovato * sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=arrivo;
    }
    //se NON è possibile arrivare alla stazione di arrivo
    else{
        //percorso NON trovato
        free(percorsoTrovato);
        return NULL;
    }

    //scegliere le stazioni più vicine all'origine
    while(indicePercorsoTrovato>1){
        //A -- stazione da controllare -- B
        //limite inferiore per il controllo delle stazioni più vicine all'origine
        a= indiceStazioneOrdineDecrescente(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato - 2]);
        //limite superiore per il controllo delle stazioni più vicine all'origine
        b= indiceStazioneOrdineDecrescente(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato]);

        //controllare tutte le stazioni dal punto B al punto A - ricercare se tra i 2 punti esiste una stazioni più piccola
        for (int i = a+1; i < b; ++i) {
            //A.autonomia >= medio.distanza - A.distanza
            //medio.autonomia >= B.distanza - medio.distanza
            if (stazioni[a].autonomiaMassima>=abs((int) stazioni[i].distanza-(int) stazioni[a].distanza) &&
                stazioni[i].autonomiaMassima>=abs((int) stazioni[b].distanza-(int) stazioni[i].distanza)){
                percorsoTrovato[indicePercorsoTrovato-1]=(int) stazioni[i].distanza;
            }
        }
        --indicePercorsoTrovato;
    }

    //percorso trovato
    return percorsoTrovato;
}

/**
 * Pianifica il percorso ottimale per andare dalla stazione di partenza fino a quella di arrivo.
 *
 * @param stazioni stazioni tra quella di partenza e quella di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @param partenza stazione di partenza.
 * @param arrivo stazione di arrivo.
 * @return percorso tra la stazione di partenza e quella di arrivo, NULL se NON esiste.
 */
int *percorsoPianificato(struct ArrayNodeStazione stazioni[], int numeroDiStazioni, int partenza, int arrivo){
    //calcolare il percorso ottimale per l'andata
    if(partenza<arrivo){
        return percorsoPianificatoInAvanti(stazioni, numeroDiStazioni, partenza, arrivo);
    }
    //calcolare il percorso ottimale per il ritorno
    else{
        return percorsoPianificatoAllIndietro(stazioni, numeroDiStazioni, partenza, arrivo);
    }
}

/**
 * Stampa il percorso ottimale trovato.
 *
 * @param percorso percorso da stampare.
 * @param arrivo stazione di arrivo.
 */
void stampaPercorsoPianificato(const int *percorso, int arrivo){
    //se NON esiste un percorso
    if (percorso==NULL){
        printf("nessun percorso\n");
    }
    //se esiste un percorso
    else{
        int i=0;
        do {
            printf("%d ", percorso[i]);
            ++i;
        } while (percorso[i]!=arrivo);
        //stampa l'ultima stazione - l'arrivo
        printf("%d\n", percorso[i]);
    }
}



//GESTIONE COMANDI
/**
 * Aggiungi la stazione all'autostrada.
 */
void aggiungiStazione(){
    /**
     * Distanza dall'origine della stazione.
     */
    unsigned int distanza=0;
    /**
     * Numero di auto da aggiungere alla stazione.
     */
    unsigned int numeroAuto=0;
    /**
     * Autonomia dell'auto da aggiungere alla stazione.
     */
    unsigned int autonomiaAuto=0;
    /**
     * Parco auto della stazione.
     */
    struct HashTableParcoAuto *parcoAuto=NULL;
    /**
     * Autonomia massima presente nel parco auto.
     */
    unsigned int maxAutonomia=0;
    /**
     * Serve per leggere il valore di ritorno della scanf, se no NON compila.
     */
    int returnScanf;


    //lettura dell distanza della stazione da aggiungere
    returnScanf=scanf("%d", &distanza);
    if(returnScanf==EOF){
        return;
    }

    //stazione già presente
    if(stazioneGiaPresenteNellAutostrada(autostrada, distanza) == 1){
        //la stazione NON è stata aggiunta
        printf("non aggiunta\n");

        //devo comunque leggere le auto inserite in input altrimenti restano nel buffer
        do {
            returnScanf= getc_unlocked(stdin);
        } while (returnScanf!='\n');
    }
    //stazione NON presente
    else{
        //leggere il numero di auto che bisogna aggiungere alla stazione
        returnScanf=scanf("%d", &numeroAuto);
        if(returnScanf==EOF){
            return;
        }

        //creare il parco auto
        parcoAuto= creaParcoAuto(DIMENSIONE_INIZIALE_PARCO_AUTO);

        //aggiungere le auto con la loro autonomia al parco auto
        for (int i = 0; i < numeroAuto; ++i) {
            //leggere l'autonomia delle auto da aggiungere al parco auto
            returnScanf=scanf("%d", &autonomiaAuto);
            if(returnScanf==EOF){
                return;
            }

            //salvare il valore massimo di autonomia
            if(autonomiaAuto>maxAutonomia){
                maxAutonomia=autonomiaAuto;
            }

            //aggiungere l'auto al parco auto sopra creato
            aggiungiAutoAlParcoAuto(parcoAuto, autonomiaAuto);
        }

        //aggiungere la stazione all'autostrada
        aggiungiStazioneAllAutostrada(autostrada, distanza, parcoAuto, maxAutonomia);

        //la stazione è stata aggiunta
        printf("aggiunta\n");
    }
}

/**
 * Aggiungi l'auto alla stazione indicata.
 */
void aggiungiAuto(){
    /**
    * Distanza della stazione dall'origine.
    */
    unsigned int distanzaStazione=0;
    /**
     * Autonomia dell'auto da aggiungere alla stazione.
     */
    unsigned int autonomiaAutoDaAggiungere=0;
    /**
     * Serve per leggere il valore di ritorno della scanf, se no NON compila.
     */
    int returnScanf;
    /**
     * Stazione di interesse.
     */
    struct HashNodeStazione *stazione;


    //lettura della distanza dove si trova la stazione a cui bisogna aggiungere una macchina
    returnScanf=scanf("%d", &distanzaStazione);
    if(returnScanf==EOF){
        return;
    }

    //stazione NON presente
    if(stazioneGiaPresenteNellAutostrada(autostrada, distanzaStazione) == 0){
        //auto NON aggiunta
        printf("non aggiunta\n");

        //devo comunque leggere l'auto inserita in input altrimenti resta nel buffer
        do {
            returnScanf= getc_unlocked(stdin);
        } while (returnScanf!='\n');
    }
    //stazione presente
    else{
        stazione= ricercaStazioneNellAutostrada(autostrada, distanzaStazione);

        //leggere l'autonomia dell'auto da aggiungere
        returnScanf=scanf("%d", &autonomiaAutoDaAggiungere);
        if(returnScanf==EOF){
            return;
        }

        //salvare l'autonomia massima che si può percorrere dalla stazione
        if(autonomiaAutoDaAggiungere>stazione->autonomiaMassima){
            stazione->autonomiaMassima=autonomiaAutoDaAggiungere;
        }

        //aggiungere l'auto al parco auto della stazione
        aggiungiAutoAlParcoAuto(stazione->parcoAuto, autonomiaAutoDaAggiungere);

        //auto aggiunta la parco auto
        printf("aggiunta\n");
    }
}

/**
 * Demolisci la stazione dall'autostrada.
 */
void demolisciStazione(){
    /**
    * Distanza dall'origine della stazione.
    */
    unsigned int distanza=0;
    /**
     * Serve per leggere il valore di ritorno della scanf, se no NON compila.
     */
    int returnScanf;


    //lettura della distanza della stazione da demolire
    returnScanf=scanf("%d", &distanza);
    if(returnScanf==EOF){
        return;
    }

    //se la stazione NON è presente
    if(stazioneGiaPresenteNellAutostrada(autostrada, distanza) == 0){
        //stazione NON demolita
        printf("non demolita\n");
    }
    //se la stazione è presente
    else{
        //demolire stazione
        eliminaStazioneDallAutostrada(autostrada, distanza);

        //stazione demolita
        printf("demolita\n");
    }
}

/**
 * Rottama l'auto alla stazione indicata.
 */
void rottamaAuto(){
    /**
    * Distanza della stazione dall'origine.
    */
    unsigned int distanzaStazione=0;
    /**
     * Autonomia dell'auto da rottamare.
     */
    unsigned int autonomiaAutoDaRottamare=0;
    /**
     * Serve per leggere il valore di ritorno della scanf, se no NON compila.
     */
    int returnScanf;
    /**
     * Stazione di interesse.
     */
    struct HashNodeStazione *stazione;


    //lettura della distanza a cui si trova la stazione da cui bisogna rottamare una macchina
    returnScanf=scanf("%d", &distanzaStazione);
    if(returnScanf==EOF){
        return;
    }

    //Se stazione NON presente
    if(stazioneGiaPresenteNellAutostrada(autostrada, distanzaStazione) == 0){
        //auto NON rottamata
        printf("non rottamata\n");

        //bisogna comunque leggere le auto inserite in input altrimenti restano nel buffer
        do {
            returnScanf= getc_unlocked(stdin);
        } while (returnScanf!='\n');
    }
    //stazione presente
    else{
        //ricercare la stazione di interesse
        stazione= ricercaStazioneNellAutostrada(autostrada, distanzaStazione);

        //leggere l'autonomia dell'auto da rottamare
        returnScanf=scanf("%d", &autonomiaAutoDaRottamare);
        if(returnScanf==EOF){
            return;
        }

        //Se l'auto NON è presente nel parco auto della stazione
        if(ricercaAutoNelParcoAuto(stazione->parcoAuto, autonomiaAutoDaRottamare) == 0){
            //auto NON rottamata
            printf("non rottamata\n");
        }
        //se l'auto è presente nel parco auto della stazione
        else{
            //eliminare l'auto dal parco auto della stazione
            eliminaAutoDalParcoAuto(stazione->parcoAuto, autonomiaAutoDaRottamare);

            //salvare la nuova autonomia massima
            stazione->autonomiaMassima= ricercaMassimaAutonomiaNelParcoAuto(stazione->parcoAuto,
                                                                            stazione->parcoAuto->capacita);

            //auto rottamata
            printf("rottamata\n");
        }
    }
}

/**
 * Gestione del comando di pianifica percorso dalla stazione di partenza a quella di arrivo.
 */
void pianificaPercorso(){
    /**
     * Distanza dall'origine della stazione di partenza.
     */
    int distanzaStazionePartenza=0;
    /**
     * Distanza dall'origine della stazione di arrivo.
     */
    int distanzaStazioneArrivo=0;
    /**
     * Serve per leggere il valore di ritorno della scanf, se no NON compila.
     */
    int returnScanf;
    /**
     * Numero di stazioni presenti tra quella di partenza e quella di arrivo.
     */
    int numeroDiStazioni=0;
    /**
     * Stazioni comprese tra la partenza e l'arrivo.
     */
    struct ArrayNodeStazione *stazioniIntermedie;
    /**
     * Percorso ottimale tra la stazione di partenza e quella di arrivo.
     */
    int *percorsoTrovato;


    //lettura distanza delle stazioni di partenza e di arrivo
    returnScanf=scanf("%d", &distanzaStazionePartenza);
    if(returnScanf==EOF){
        return;
    }
    returnScanf=scanf("%d", &distanzaStazioneArrivo);
    if(returnScanf==EOF){
        return;
    }

    //se stazione di partenza NON ha macchine
    if(ricercaStazioneNellAutostrada(autostrada, distanzaStazionePartenza)->autonomiaMassima == 0){
        printf("nessun percorso\n");
    }
    //se la stazione di partenza è anche quella di arrivo
    else if(distanzaStazionePartenza==distanzaStazioneArrivo){
        printf("%d\n", distanzaStazionePartenza);
    }
    //ricercare il percorso ottimale per andare dalla partenza all'arrivo
    else{
        //selezionare tutte le stazioni tra quella di partenza e quella di arrivo
        stazioniIntermedie= tutteLeStazioni(distanzaStazionePartenza, distanzaStazioneArrivo, &numeroDiStazioni);
        //ricercare il percorso ottimale per andare dalla partenza all'arrivo
        percorsoTrovato= percorsoPianificato(stazioniIntermedie, numeroDiStazioni, distanzaStazionePartenza, distanzaStazioneArrivo);

        //se NON esiste il percorso
        if(percorsoTrovato==NULL){
            printf("nessun percorso\n");
        }
        //se esiste il percorso stamparlo
        else{
            stampaPercorsoPianificato(percorsoTrovato, distanzaStazioneArrivo);
        }

        //deallocare la memoria che NON serve per i prossimi percorsi
        free(stazioniIntermedie);
        if (percorsoTrovato!=NULL){
            free(percorsoTrovato);
        }
    }
}

/**
 * Gestione dell'interpretazione del comando letto dal stIN.
 *
 * @param comando stringa che rappresenta il comando da eseguire.
 */
void processaComando(const char comando[]){
    //che comando è stato inserito
    switch (comando[0]) {
        //aggiungi: -stazione OR -auto
        case 'a':
            //aggiungi-stazione
            if(comando[9]=='s'){
                aggiungiStazione();
            }
            //aggiungi-auto
            else{
                aggiungiAuto();
            }
            break;
        //demolisci-stazione
        case 'd':
            demolisciStazione();
            break;
        //rottama-auto
        case 'r':
            rottamaAuto();
            break;
        //pianifica-percorso
        case 'p':
            pianificaPercorso();
            break;
    }
}



//MAIN
/**
 * Main del programma.
 *
 * @return 0 se il programma ha terminato l'esecuzione correttamente.
 */
int main() {
    /**
     * Carattere per capire che NON bisogna più leggere da stIN.
     */
    int fine;
    /**
     * Stringa che rappresenta il comando da eseguire.
     */
    char comando[LUNGHEZZA_MAX_COMANDI]="";


    //inizializzazione autostrada
    autostrada= creaAutostrada(DIMENSIONE_INIZIALE_AUTOSTRADA);

    //continuare a leggere fino a che non si legge EOF
    do{
        //legge i comandi che arrivano da stIN
        fine=scanf("%s", comando);

        //processa il comando letto
        processaComando(comando);
    }while(fine!=EOF);

    //il programma è terminato correttamente
    return 0;
}
