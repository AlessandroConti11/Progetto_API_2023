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
 * Define --> Dimensione iniziale della HashTable che rappresetna un parco auto.
 */
#define DIMENSIONE_INIZIALE_PARCO_AUTO 13

/**
 * Define --> Dimensione iniziale della HashTable che rappresenta l'autostrada.
 */
#define DIMENSIONE_INIZIALE_AUTOSTRADA 31

/**
 * Define --> Percentuale oltre al quale andrà reallocato la HashTable
 */
#define PERCENTUALE_REALLOC 0.75

/**
 * Define --> Costante di Knuth, serve per la funzione di hash
 */
#define COSTANTE_DI_KNUTH 2654435769



//STRUCT
/**
 * Struct --> Rappresenta una macchina.
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
 * Struct --> Rappresenta il parco auto.
 */
struct HashTableParcoAuto{
    /**
     * Parco auto.
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
 * Struct --> Rappresenta l'autostrada.
 */
struct HashTableAutostrada{
    /**
     * Parco auto.
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
 * Struct --> Rappresenta una posizione nell'Array contenente tutte le stazioni tra quella iniziale e quella di arrivo.
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
 * Autostrada.
 */
struct HashTableAutostrada *autostrada=NULL;



//GESTIONE HASH
/**
 * Funzione di hash --> ritorna la posizione dato il valore richiesto.
 *
 * @param chiave valore d'interesse.
 * @param capacita dimensione della HashTable.
 * @return la posizione della chiave nella HashTable.
 */
unsigned int funzioneDiHash(unsigned int chiave, unsigned int capacita){
    //funzione di hash di moltiplicazione modulare
    //utilizzando la costante di Knuth
    return (int) ((long)(chiave * COSTANTE_DI_KNUTH) % capacita);
}

/**
 * Ritorna la nuova capacità della HashTable.
 *
 * @param capacitaIniziale capacità iniziale della HashTable.
 * @return la nuova capacità della HashTable.
 */
int nuovaCapacita(unsigned int capacitaIniziale){
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


    //inizializza valori
    hashTable->dimensione=0;
    hashTable->capacita=capacitaIniziale;
    hashTable->autoNode=(struct HashNodeAuto **) calloc(capacitaIniziale, sizeof(struct HashNodeAuto *));

    //ritorna la HashTable inizializzata
    return hashTable;
}

/**
 * Reinizializza il parco auto --> da una HashTable già esistente la realloca in un'altra con capacità maggiore.
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


    //spostiamo dalla vecchia HashTable a quella nuova
    for (int i = 0; i < parcoAuto->capacita; ++i) {
        corrente=parcoAuto->autoNode[i];

        //fino a che NON abbiamo finito di spostare tutte le HashNode
        while(corrente!=NULL){
            //salviamo il prossimo HashNode da visitare
            prossimo=corrente->successivo;
            //salviamo il nuovo indice in cui andare a mettere la HashNode corrente
            nuovoIndice= funzioneDiHash(corrente->autonomia, (unsigned int) capacitaNuova);

            //spostiamo il nodo
            corrente->successivo=nuovaHashTable[nuovoIndice];
            nuovaHashTable[nuovoIndice]=corrente;
            //il nodo corrente da vedere diventa il prossimo
            corrente=prossimo;
        }
    }

    //eliminiamo la vecchia stazioni
    free(parcoAuto->autoNode);
    //aggiungiamo la nuova stazioni
    parcoAuto->autoNode=nuovaHashTable;
    //aggiorniamo la capacità
    parcoAuto->capacita=(unsigned int) capacitaNuova;
}

/**
 * Aggiungi auto al parco auto richiesto.
 *
 * @param parcoAuto il parco auto richiesto.
 * @param autonomia l'autonomia dell'auto da aggiungere al parco auto.
 */
void aggiungiAuto(struct HashTableParcoAuto *parcoAuto, unsigned int autonomia){
    /**
     * Indice in cui inserire la chiave.
     */
    unsigned int indice= funzioneDiHash(autonomia, parcoAuto->capacita);
    /**
     * Nuovo HashNode da inserire nella HashTable.
     */
    struct HashNodeAuto *nuovoNodo=(struct HashNodeAuto *) malloc(sizeof(struct HashNodeAuto));


    //inserimento valori nel nuovo HashNode
    nuovoNodo->autonomia=autonomia;
    nuovoNodo->successivo=parcoAuto->autoNode[indice];
    parcoAuto->autoNode[indice]=nuovoNodo;
    //è stato aggiunto un nuovo nodo, questo implica che bisogna aumentare la dimensione della HashTable
    parcoAuto->dimensione++;

    //bisogna reallocare la HashTable
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
void eliminaAuto(struct HashTableParcoAuto *parcoAuto, unsigned int autonomia){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(autonomia, parcoAuto->capacita);
    /**
     * Nodo corrente,
     * corrente=corrente->successivo.
     */
    struct HashNodeAuto *corrente=parcoAuto->autoNode[indice];
    /**
     * Nodo precedente,
     * precedente=corrente.
     */
    struct HashNodeAuto *precedente=NULL;


    //trova il nodo con la chiave corrispondente, assegna corrente AND precedente
    while (corrente!=NULL && corrente->autonomia!=autonomia){
        precedente=corrente;
        corrente=corrente->successivo;
    }

    //chiave NON trovata
    if(corrente==NULL){
        return;
    }
        //chiave trovata
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

    //dealloca il HashNode
    free(corrente);
    //riduciamo il numero di elementi presenti nella HashTable
    parcoAuto->dimensione--;
}

/**
 * Ricerca l'auto richiesta nel parco auto.
 *
 * @param parcoAuto il parco auto che contiene la macchina da ricercare.
 * @param autonomia l'autonomia della macchina da ricercare.
 * @return 1 se la macchina è presente nel parco auto, 0 se NON è presente.
 */
int ricercaAuto(struct HashTableParcoAuto *parcoAuto, unsigned int autonomia){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(autonomia, parcoAuto->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNodeAuto *corrente=parcoAuto->autoNode[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //abbiamo trovato la chiave
        if(corrente->autonomia==autonomia){
            return 1;
        }
        //guardiamo nel prossimo HashNode
        else{
            corrente=corrente->successivo;
        }
    }

    //NON abbiamo trovato la chiave
    return 0;
}

/**
 * Ricerca l'autonomia massima nel parco auto richiesto.
 *
 * @param parcoAuto il parco auto d'interesse.
 * @param dimensioneMassima la dimensione che ha il parco auto - capacità attuale.
 * @return l'autonomia massima presente nel parco auto.
 */
unsigned int ricercaMassimaAutonomia(struct HashTableParcoAuto *parcoAuto, unsigned int dimensioneMassima){
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
        //controllo tutta la lista presente nella posizione
        while (corrente!=NULL){
            //se corrente è maggiore del massimo modificalo
            if(corrente->autonomia>massimo){
                massimo=corrente->autonomia;
            }
            corrente=corrente->successivo;
        }
    }

    //ritorna la massima autonomia
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


    //inizializza valori
    hashTable->capacita=capacitaIniziale;
    hashTable->dimensione=0;
    hashTable->stazioni=(struct HashNodeStazione **) calloc(hashTable->capacita, sizeof(struct HashNodeStazione *));

    //ritorna la HashTable inizializzata
    return hashTable;
}

/**
 * Reinizializza il autostrada --> da una HashTable già esistente la realloca in un'altra con capacità maggiore.
 *
 * @param htAutostrada l'autostrada da reallocare.
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


    //spostiamo dalla vecchia HashTable a quella nuova
    for (int i = 0; i < htAutostrada->capacita; ++i) {
        corrente=htAutostrada->stazioni[i];

        //fino a che NON abbiamo finito di spostare tutte le HashNode
        while(corrente!=NULL){
            //salviamo il prossimo HashNode da visitare
            prossimo=corrente->successivo;
            //salviamo il nuovo indice in cui andare a mettere la HashNode corrente
            nuovoIndice= funzioneDiHash(corrente->distanza, capacitaNuova);

            //spostiamo il nodo
            corrente->successivo=nuovaHashTable[nuovoIndice];
            nuovaHashTable[nuovoIndice]=corrente;
            //il nodo corrente da vedere diventa il prossimo
            corrente=prossimo;
        }
    }

    //eliminiamo la vecchia stazioni
    free(htAutostrada->stazioni);
    //aggiungiamo la nuova stazioni
    htAutostrada->stazioni=nuovaHashTable;
    //aggiorniamo la capacità
    htAutostrada->capacita=capacitaNuova;
}

/**
 * Aggiungi stazione all'autostrada.
 *
 * @param htAutostrada l'autostrada a cui vanno aggiunte le stazioni.
 * @param distanza la distanza a cui si trova la stazione che vogliamo aggiungere.
 * @param parcoAuto il parco auto contenuto nella stazione indicata.
 */
void aggiungiStazione(struct HashTableAutostrada *htAutostrada, unsigned int distanza, struct HashTableParcoAuto *parcoAuto, unsigned int maxAutonomia){
    /**
     * Indice in cui inserire la chiave.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * Nuovo HashNode da inserire nella HashTable.
     */
    struct HashNodeStazione *nuovoNodo=(struct HashNodeStazione *) malloc(sizeof(struct HashNodeStazione));


    //inserimento valori nel nuovo HashNode
    nuovoNodo->distanza=distanza;
    nuovoNodo->parcoAuto=parcoAuto;
    nuovoNodo->autonomiaMassima=maxAutonomia;
    nuovoNodo->successivo=htAutostrada->stazioni[indice];
    htAutostrada->stazioni[indice]=nuovoNodo;
    //è stato aggiunto un nuovo nodo, questo implica che bisogna aumentare la dimensione della HashTable
    htAutostrada->dimensione++;

    //bisogna reallocare la HashTable
    if(htAutostrada->dimensione > htAutostrada->capacita*PERCENTUALE_REALLOC){
        reHashAutostrada(htAutostrada);
    }
}

/**
 * Elimina stazione dall'autostrada.
 *
 * @param htAutostrada l'autostrada in cui si trova la stazione.
 * @param distanza la distanza a cui si trova la stazione da eliminare.
 */
void eliminaStazione(struct HashTableAutostrada *htAutostrada, unsigned int distanza){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * Nodo Corrente,
     * stazioneCorrente=stazioneCorrente->successivo.
     */
    struct HashNodeStazione *stazioneCorrente=htAutostrada->stazioni[indice];
    /**
     * Nodo Precedente,
     * stazionePrecedente=stazioneCorrente.
     */
    struct HashNodeStazione *stazionePrecedente=NULL;
    /**
     * Auto corrente presente in posizione i nel parco auto.
     */
    struct HashNodeAuto *corrente;
    /**
     * Nodo precedente da eliminare.
     */
    struct HashNodeAuto *precedente;


    //trova il nodo con la chiave corrispondente, assegna stazioneCorrente AND stazionePrecedente
    while (stazioneCorrente != NULL && stazioneCorrente->distanza != distanza){
        stazionePrecedente=stazioneCorrente;
        stazioneCorrente=stazioneCorrente->successivo;
    }

    //chiave NON trovata
    if(stazioneCorrente == NULL){
        return;
    }
    //chiave trovata
    else{
        //il nodo da eliminare è il primo della lista presente nella posizione indicata
        if(stazionePrecedente == NULL){
            htAutostrada->stazioni[indice]=stazioneCorrente->successivo;
        }
        //il nodo da eliminare NON è il primo della lista presente nella posizione indicate
        else{
            stazionePrecedente->successivo=stazioneCorrente->successivo;
        }
    }

    //elimina macchine
    for (int i = 0; i < stazioneCorrente->parcoAuto->capacita; ++i) {
        //nodo corrente
        corrente=stazioneCorrente->parcoAuto->autoNode[i];
        while (corrente!=NULL){
            //nodo precedente
            precedente=corrente;
            corrente=corrente->successivo;
            free(precedente);
        }
    }
    //elimina parcoAuto
    free(stazioneCorrente->parcoAuto->autoNode);
    //elimina hashTable parco auto
    free(stazioneCorrente->parcoAuto);
    //elimina stazione
    free(stazioneCorrente);
    //riduciamo il numero di elementi presenti nella HashTable
    htAutostrada->dimensione--;
}

/**
 * Ricerca la stazione nell'autostrada alla distanza data.
 *
 * @param htAutostrada l'autostrada dove eseguire la ricerca.
 * @param distanza la distanza a cui si trova la stazione interessate.
 * @return la stazione ricercata, se esiste, altrimenti ritorna NULL.
 */
struct HashNodeStazione *ricercaStazione(struct HashTableAutostrada *htAutostrada, unsigned int distanza){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNodeStazione *corrente=htAutostrada->stazioni[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //abbiamo trovato la chiave
        if(corrente->distanza==distanza){
            return corrente;
        }
        //guardiamo nel prossimo HashNode
        else{
            corrente=corrente->successivo;
        }
    }

    //NON abbiamo trovato la chiave --> corrente=NULL
    return NULL;
}

/**
 * Ritorna 1 se la stazione esiste sull'autostrada.
 *
 * @param distanza distanza in cui si trova la stazione di interesse.
 * @return 1 se la stazione esiste sull'autostrada, 0 altrimenti
 */
int StazioneGiaPresente(struct HashTableAutostrada *htAutostrada, unsigned int distanza){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(distanza, htAutostrada->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNodeStazione *corrente=htAutostrada->stazioni[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //abbiamo trovato la chiave
        if(corrente->distanza==distanza){
            return 1;
        }
        //guardiamo nel prossimo HashNode
        else{
            corrente=corrente->successivo;
        }
    }

    //NON abbiamo trovato la chiave
    return 0;
}



//COMPARAZIONE
/**
 * Funzione che compara 2 nodi.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la comparazioneAvanti.
 */
int comparazioneAvanti(const void *a, const void *b){
    return (int) (((struct ArrayNodeStazione *)a)->distanza-((struct ArrayNodeStazione *)b)->distanza);
}

/**
 * Funzione che compara 2 nodi.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la comparazioneAvanti.
 */
int comparazioneIndietro(const void *a, const void *b){
    return (int) (((struct ArrayNodeStazione *)b)->distanza-((struct ArrayNodeStazione *)a)->distanza);
}



//PIANIFICA PERCORSO
/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo - Percorso All'Andata.
 * Dimensione iniziale è arrivo-partenza+1.
 * Dimensione finale è numeroDiStazioni.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioniInAvanti(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    /**
     * Dimensione iniziale array.
     */
    int x=1000;
    /**
     * Array di stazioni con la loro autonomia massima.
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
            //salviamo la stazione cercata
            ricerca= ricercaStazione(autostrada, i);
            //se la stazione esiste la aggiungiamo alle stazioniIntermedie[]
            if(ricerca!=NULL){
                //se l'Array stazioniIntermedie[] è pieno lo reallochiamo
                if(posizioneArray==x){
                    x=x*2;
                    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x*sizeof(struct ArrayNodeStazione)));
                }
                //aggiungiamo la stazione
                stazioniIntermedie[posizioneArray].distanza= ricerca->distanza;
                stazioniIntermedie[posizioneArray].autonomiaMassima=ricerca->autonomiaMassima;
                //aumentiamo la prossima posizione libera
                posizioneArray++;
            }
        }
    }
    //controllare tutte le stazioni se sono comprese tra partenza e arrivo
    else{
        //per ogni stazione nell'autostrada
        for (int i = 0; i < autostrada->capacita; ++i) {
            //nella cella i c'è almeno 1 stazione
            if(autostrada->stazioni[i]!=NULL){
                //stazione corrente
                corrente=autostrada->stazioni[i];
                while (corrente!=NULL){
                    if(corrente->distanza>=partenza && corrente->distanza<=arrivo){
                        //se l'Array stazioniIntermedie[] è pieno lo reallochiamo
                        if(posizioneArray==x){
                            x=x*2;
                            stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x*sizeof(struct ArrayNodeStazione)));
                        }
                        //aggiungiamo la stazione
                        stazioniIntermedie[posizioneArray].distanza= corrente->distanza;
                        stazioniIntermedie[posizioneArray].autonomiaMassima=corrente->autonomiaMassima;
                        //aumentiamo la prossima posizione libera
                        posizioneArray++;
                    }
                    corrente=corrente->successivo;
                }
            }
        }

        //ordina stazioniIntermedie - partenza --> arrivo
        qsort(stazioniIntermedie, posizioneArray, sizeof(struct ArrayNodeStazione), comparazioneAvanti);
    }

    //numero di stazioni tra quella di partenza e quella d'arrivo
    (*numeroDiStazioni)=posizioneArray;
    //stazioni intermedie tra quella di partenza e quella di arrivo
    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (*numeroDiStazioni)*sizeof(struct ArrayNodeStazione));
    return stazioniIntermedie;
}

/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo - Percorso Al Ritorno.
 * Dimensione iniziale è arrivo-partenza+1.
 * Dimensione finale è numeroDiStazioni.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioniAllIndietro(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    /**
     * Dimensione iniziale array.
     */
    int x=1000;
    /**
     * Array di stazioni con la loro autonomia massima.
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
            //salviamo la stazione cercata
            ricerca = ricercaStazione(autostrada, i);
            //se la stazione esiste la aggiungiamo alle stazioniIntermedie[]
            if (ricerca != NULL) {
                //se l'Array stazioniIntermedie[] è pieno lo reallochiamo
                if (posizioneArray == x) {
                    x=x*2;
                    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x * sizeof(struct ArrayNodeStazione)));
                }
                //aggiungiamo la stazione
                stazioniIntermedie[posizioneArray].distanza = ricerca->distanza;
                stazioniIntermedie[posizioneArray].autonomiaMassima = ricerca->autonomiaMassima;
                //aumentiamo la prossima posizione libera
                posizioneArray++;
            }
        }
    }
    //controllare tutte le stazioni se sono comprese tra partenza e arrivo
    else{
        for (int i = 0; i < autostrada->capacita; ++i) {
            //nella cella i c'è almeno 1 stazione
            if(autostrada->stazioni[i]!=NULL){
                //stazione corrente
                corrente=autostrada->stazioni[i];

                while (corrente!=NULL){
                    if(corrente->distanza<=partenza && corrente->distanza>=arrivo){
                        //se l'Array stazioniIntermedie[] è pieno lo reallochiamo
                        if(posizioneArray==x){
                            x=x*2;
                            stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (x*sizeof(struct ArrayNodeStazione)));
                        }
                        //aggiungiamo la stazione
                        stazioniIntermedie[posizioneArray].distanza= corrente->distanza;
                        stazioniIntermedie[posizioneArray].autonomiaMassima=corrente->autonomiaMassima;
                        //aumentiamo la prossima posizione libera
                        posizioneArray++;
                    }
                    corrente=corrente->successivo;
                }
            }
        }

        //ordinamento stazioniIntermedie - partenza --> arrivo
        qsort(stazioniIntermedie, posizioneArray, sizeof(struct ArrayNodeStazione), comparazioneIndietro);
    }

    //numero di stazioni tra quella di partenza e quella d'arrivo
    (*numeroDiStazioni)=posizioneArray;
    //stazioni intermedie tra quella di partenza e quella di arrivo
    stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, ((*numeroDiStazioni)*sizeof(struct ArrayNodeStazione)));
    return stazioniIntermedie;
}

/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo - Sceglie tra percorso all'Andate e al Ritorno.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioni(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    //ritorna le stazioni comprese tra la partenza e l'arrivo
    if(partenza<arrivo){
        return tutteLeStazioniInAvanti(partenza, arrivo, numeroDiStazioni);
    }
    else{
        return tutteLeStazioniAllIndietro(partenza, arrivo, numeroDiStazioni);
    }
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
    //ricerca per tutte le stazioni
    for (int i = 1; i < numeroStazioni; ++i) {
        //se la distanza tra la stazione corrente e quella iniziale è maggiore dell'autonomia massima
        if(autonomiaMassima<abs((int) stazioni[i].distanza-(int) stazioni[0].distanza)){
            //ritorna la posizione nell'Array
            return --i;
        }
    }
    //ritorna la posizione nell'Array - posizione finale
    return (numeroStazioni-1);
}

/**
 * Indice della stazione data la distanza - ordine crescente.
 *
 * @param stazioni stazioni da controllare.
 * @param numeroStazioni numero di stazioni presenti.
 * @param distanza distanza da ricercare.
 * @return indice della stazione con distanza ricercata.
 */
int indiceStazioneInAvanti(struct ArrayNodeStazione stazioni[], int numeroStazioni, unsigned int distanza){
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

        //controllo se il valore è uguale a quello cercato
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
int indiceStazioneAllIndietro(struct ArrayNodeStazione stazioni[], int numeroStazioni, unsigned int distanza){
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

        //controllo se il valore è uguale a quello cercato
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
 * Pianifica percorso in avanti - partenza < arrivo.
 *
 * @param stazioni stazioni tra partenza e arrivo.
 * @param numeroDiStazioni numero di stazioni tra partenza e arrivo.
 * @param partenza stazione di partenza.
 * @param arrivo stazione di arrivo.
 * @return percorso tra la stazione di partenza e quella di arrivo, NULL se NON esiste.
 */
int *percorsoPianificatoInAvanti(struct ArrayNodeStazione stazioni[], int numeroDiStazioni, int partenza, int arrivo){
    /**
     * Percorso da ritornare.
     */
    int *percorsoTrovato=(int *) calloc(numeroDiStazioni, sizeof(int));
    /**
     * Indice della posizione dell'array percorsoTrovato.
     */
    int indicePercorsoTrovato=0;
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
     * Indice della posizione dell'array percorsoTrovato da controllare se rispetta i vincoli richiesti.
     */
    int indiceDaControllare;
    /**
     * È l'ultimo controllo - sono stati rispettati tutti i vincoli richiesti.
     */
    int ultimoControllo;
    /**
     * Distanza da controllare.
     */
    int distanzaDaControllare;
    /**
     * Indice della stazione da controllare.
     */
    int indiceStazioneControllare;
    /**
     * Indice della stazione precedente rispetto a indiceStazioneControllare.
     */
    int indiceStazionePrecedente;
    /**
     * Nuova dimensione del percorso da ritornare - per realloc.
     */
    int nuovaDimensionePercorso;


    //aggiunta della partenza al percorso da ricercare
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
        //ricerca migliore stazione fino a questo momento
        for (int i = indiceStazionePiuLontana-1; i > indiceCorrente; --i) {
            if(stazioni[i].autonomiaMassima>=(stazioni[indiceStazioneDaSuperare].distanza-stazioni[i].distanza)){
                indiceStazionePiuLontana=i;
                indiceStazioneDaSuperare=indiceStazionePiuLontana+ stazionePiuLontana((stazioni+indiceStazionePiuLontana), (numeroDiStazioni-indiceStazionePiuLontana), stazioni[indiceStazionePiuLontana].autonomiaMassima);
            }
        }

        indiceDaControllare=indicePercorsoTrovato-1;
        ultimoControllo=0;
        distanzaDaControllare=(int) stazioni[indiceStazionePiuLontana].distanza;
        //ricerca stazioni ottimali precedenti
        while (indiceDaControllare>=1 && !ultimoControllo){
            ultimoControllo=1;
            indiceStazioneControllare= indiceStazioneInAvanti(stazioni, numeroDiStazioni,
                                                              percorsoTrovato[indiceDaControllare]);
            indiceStazionePrecedente= indiceStazioneInAvanti(stazioni, numeroDiStazioni,
                                                             percorsoTrovato[indiceDaControllare - 1]);
            //ricerca stazioni ottimali
            for (int i = indiceStazioneControllare-1; i > indiceStazionePrecedente; --i) {
                if (stazioni[i].autonomiaMassima>=(distanzaDaControllare-stazioni[i].distanza)){
                    ultimoControllo=0;
                    //aggiunta stazione con autonomia massima al percorso da trovare
                    percorsoTrovato[indiceDaControllare]=(int) stazioni[i].distanza;
                }
            }
            //nuova distanza da controllare
            distanzaDaControllare=percorsoTrovato[indiceDaControllare];
            --indiceDaControllare;
        }

        //aggiunta ultima stazione - stazione più lontana
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorso=indicePercorsoTrovato*2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorso* sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=(int) stazioni[indiceStazionePiuLontana].distanza;
        ++indicePercorsoTrovato;

        //la stazione più lontana è quella massima ==> percorso NON trovato
        if (indiceStazionePiuLontana==indiceStazioneDaSuperare){
            //percorso NON trovato
            return NULL;
        }

        indiceCorrente=indiceStazionePiuLontana;
        indiceStazionePiuLontana=indiceStazioneDaSuperare;
    }

    //possiamo arrivare alla stazione di arrivo
    if (stazioni[indiceStazioneInAvanti(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato - 1])].autonomiaMassima >= (arrivo - percorsoTrovato[indicePercorsoTrovato - 1])){
        //aggiunta stazione di arrivo
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorso=indicePercorsoTrovato*2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorso* sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=arrivo;
        return percorsoTrovato;
    }
    else{
        free(percorsoTrovato);
        return NULL;
    }
}

/**
 * Pianifica percorso all'indietro - partenza > arrivo.
 *
 * @param stazioni stazioni tra partenza e arrivo.
 * @param numeroDiStazioni numero di stazioni tra partenza e arrivo.
 * @param partenza stazione di partenza.
 * @param arrivo stazione di arrivo.
 * @return percorso tra la stazione di partenza e quella di arrivo, NULL se NON esiste.
 */
int *percorsoPianificatoAllIndietro(struct ArrayNodeStazione stazioni[], int numeroDiStazioni, int partenza, int arrivo){
    /**
     * Percorso da ritornare.
     */
    int *percorsoTrovato=(int *) calloc(numeroDiStazioni, sizeof(int));
    /**
     * Indice della posizione dell'array percorsoTrovato.
     */
    int indicePercorsoTrovato=0;
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
//    /**
//     * Indice della posizione dell'array percorsoTrovato da controllare se rispetta i vincoli richiesti.
//     */
//    int indiceDaControllare=indicePercorsoTrovato;
//    /**
//     * È l'ultimo controllo - sono stati rispettati tutti i vincoli richiesti.
//     */
//    int ultimoControllo=0;
//    /**
//     * Distanza da controllare.
//     */
//    int distanzaDaControllare=0;
//    /**
//     * Indice della stazione da controllare.
//     */
//    int indiceStazioneControllare=0;
//    /**
//     * Indice della stazione precedente rispetto a indiceStazioneControllare.
//     */
//    int indiceStazionePrecedente=0;
    /**
     * Nuova dimensione del percorso da ritornare - per realloc.
     */
    int nuovaDimensionePercorso;
//    /**
//     * Indice dell'ultima stazione prima dell'arrivo - serve per controllare se ce ne sono altre con kilometraggio minore.
//     */
//    int indiceUltimaStazioneCercata=0;
//    /**
//     * Indice della penultima stazione prima dell'arrivo - serve per controllare se ce ne sono altre con kilometraggio minore.
//     */
//    int indicePenultimaStazioneCercata=0;
//    /**
//     * Indice che serve per dare un limite nella ricerca delle stazioni più ottimali.
//     */
//    int indicePosizioneLimite=0;


    //aggiunta della partenza al percorso da ricercare
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
        //ricerca migliore stazione fino a questo momento
        for (int i = indiceStazionePiuLontana-1; i > indiceCorrente; --i) {
            if(stazioni[i].autonomiaMassima>=abs((int) stazioni[indiceStazioneDaSuperare].distanza-(int) stazioni[i].distanza)){
                indiceStazionePiuLontana=i;
                indiceStazioneDaSuperare=indiceStazionePiuLontana+ stazionePiuLontana((stazioni+indiceStazionePiuLontana), (numeroDiStazioni-indiceStazionePiuLontana), stazioni[indiceStazionePiuLontana].autonomiaMassima);
            }
        }

        //aggiunta ultima stazione - stazione più lontana
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorso=indicePercorsoTrovato*2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorso* sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=(int) stazioni[indiceStazionePiuLontana].distanza;
        ++indicePercorsoTrovato;

        //la stazione più lontana è quella massima ==> percorso NON trovato
        if (indiceStazionePiuLontana==indiceStazioneDaSuperare){
            //percorso NON trovato
            free(percorsoTrovato);
            return NULL;
        }

        indiceCorrente=indiceStazionePiuLontana;
        indiceStazionePiuLontana=indiceStazioneDaSuperare;
    }

    //possiamo arrivare alla stazione di arrivo
    if (stazioni[indiceStazioneAllIndietro(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato - 1])].autonomiaMassima >= abs(arrivo - percorsoTrovato[indicePercorsoTrovato - 1])){
        //aggiunta stazione di arrivo
        if(indicePercorsoTrovato==numeroDiStazioni){
            nuovaDimensionePercorso=indicePercorsoTrovato*2;
            percorsoTrovato=(int *) realloc(percorsoTrovato, nuovaDimensionePercorso* sizeof(int));
        }
        percorsoTrovato[indicePercorsoTrovato]=arrivo;
    }
    else{
        free(percorsoTrovato);
        return NULL;
    }

    //controllo per stazioni finali più vicine all'origine
    while(indicePercorsoTrovato>1){
        int b= indiceStazioneAllIndietro(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato]);
        int a= indiceStazioneAllIndietro(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato-2]);

        for (int i = a+1; i < b; ++i) {
            if (stazioni[a].autonomiaMassima>=abs((int) stazioni[i].distanza-(int) stazioni[a].distanza) &&
                stazioni[i].autonomiaMassima>=abs((int) stazioni[b].distanza-(int) stazioni[i].distanza)){
                percorsoTrovato[indicePercorsoTrovato-1]=(int) stazioni[i].distanza;
            }
        }
        --indicePercorsoTrovato;
    }






//    while (indicePercorsoTrovato>1){
//        indiceStazionePiuLontana= indiceStazioneAllIndietro(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato]);
//        indiceUltimaStazioneCercata= indiceStazioneAllIndietro(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato-1]);
//        indicePenultimaStazioneCercata= indiceStazioneAllIndietro(stazioni, numeroDiStazioni, percorsoTrovato[indicePercorsoTrovato-2]);
//        indicePosizioneLimite= stazionePiuLontana(stazioni, numeroDiStazioni, stazioni[indicePenultimaStazioneCercata].autonomiaMassima);
//
//        for (int i = indicePosizioneLimite; i <= indiceUltimaStazioneCercata; ++i) {
//            if (stazioni[i].autonomiaMassima>= abs((int) stazioni[i].distanza-(int) stazioni[indiceStazionePiuLontana].distanza)){
//                printf(" ->%d\n", percorsoTrovato[indicePercorsoTrovato-1]);
//                percorsoTrovato[indicePercorsoTrovato-1]=(int) stazioni[i].distanza;
//
//            }
//        }
//        printf(" ->PercorsoTrovato ");
//        for (int j = 0; j < indicePercorsoTrovato; ++j) {
//            printf("%d ", percorsoTrovato[j]);
//        }
//        printf("\n");
//        --indicePercorsoTrovato;
//    }

    //percorso trovato
    return percorsoTrovato;
}

/**
 * Pianifica percorso.
 *
 * @param stazioni stazioni tra partenza e arrivo.
 * @param numeroDiStazioni numero di stazioni tra partenza e arrivo.
 * @param partenza stazione di partenza.
 * @param arrivo stazione di arrivo.
 * @return percorso tra la stazione di partenza e quella di arrivo, NULL se NON esiste.
 */
int *percorsoPianificato(struct ArrayNodeStazione stazioni[], int numeroDiStazioni, int partenza, int arrivo){
    //calcolare il percorso all'andata
    if(partenza<arrivo){
        return percorsoPianificatoInAvanti(stazioni, numeroDiStazioni, partenza, arrivo);
    }
    //calcolare il percorso al ritorno
    else{
        return percorsoPianificatoAllIndietro(stazioni, numeroDiStazioni, partenza, arrivo);
    }
}

/**
 * Stampa il percorso trovato.
 *
 * @param percorso percorso da stampare.
 * @param arrivo stazione di arrivo.
 */
void stampaPercorsoPianificato(const int *percorso, int arrivo){
    //se NON esiste un percorso
    if (percorso==NULL){
        printf("nessun percorso\n");
    }
    //se esiste un percorso stampalo
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
 * Aggiungi stazione all'autostrada.
 */
void AggiungiStazione(){
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
     * Serve per far ritornare la scanf, se no NON compila.
     */
    int returnScanf;


    //lettura distanza della stazione da aggiungere
    returnScanf=scanf("%d", &distanza);
    if(returnScanf==EOF){
        return;
    }

    //stazione già presente
    if(StazioneGiaPresente(autostrada, distanza)==1){
        //la stazione NON è stata aggiunta
        printf("non aggiunta\n");

        //devo comunque leggere le auto inserite in input altrimenti restano nel buffer
        do {
            returnScanf= getc_unlocked(stdin);
        } while (returnScanf!='\n');
    }
    //stazione NON presente
    else{
        //leggi numero di macchine che bisogna aggiungere alla stazione
        returnScanf=scanf("%d", &numeroAuto);
        if(returnScanf==EOF){
            return;
        }

        //crea il parco auto
        parcoAuto= creaParcoAuto(DIMENSIONE_INIZIALE_PARCO_AUTO);

        //aggiungi auto con la loro chiave al parco auto
        for (int i = 0; i < numeroAuto; ++i) {
            //leggi chiave auto da aggiungere al parco auto
            returnScanf=scanf("%d", &autonomiaAuto);
            if(returnScanf==EOF){
                return;
            }

            //salviamo il valore massimo di autonomia
            if(autonomiaAuto>maxAutonomia){
                maxAutonomia=autonomiaAuto;
            }

            //aggiungi autonomiaAuto al parco auto sopra creato
            aggiungiAuto(parcoAuto, autonomiaAuto);
        }

        //aggiunta la stazione
        aggiungiStazione(autostrada, distanza, parcoAuto, maxAutonomia);

        //la stazione è stata aggiunta
        printf("aggiunta\n");
    }
}

/**
 * Aggiungi auto alla stazione indicata.
 */
void AggiungiAuto(){
    /**
    * Distanza della stazione dall'origine
    */
    unsigned int distanzaStazione=0;
    /**
     * Autonomia dell'auto da aggiungere alla stazione
     */
    unsigned int autonomiaAutoDaAggiungere=0;
    /**
     * Serve per far ritornare la scanf, se no NON compila.
     */
    int returnScanf;
    /**
     * Stazione di interesse.
     */
    struct HashNodeStazione *stazione;


    //lettura distanzaStazione della stazione a cui aggiungere una macchina
    returnScanf=scanf("%d", &distanzaStazione);
    if(returnScanf==EOF){
        return;
    }

    //stazione NON presente
    if(StazioneGiaPresente(autostrada, distanzaStazione)==0){
        //auto NON aggiunta
        printf("non aggiunta\n");

        //devo comunque leggere l'auto inserita in input altrimenti resta nel buffer
        do {
            returnScanf= getc_unlocked(stdin);
        } while (returnScanf!='\n');
    }
    //stazione presente
    else{
        stazione= ricercaStazione(autostrada, distanzaStazione);

        //leggi chiave auto da aggiungere
        returnScanf=scanf("%d", &autonomiaAutoDaAggiungere);
        if(returnScanf==EOF){
            return;
        }

        //distanza massima percorribile da quella stazione
        if(autonomiaAutoDaAggiungere>stazione->autonomiaMassima){
            stazione->autonomiaMassima=autonomiaAutoDaAggiungere;
        }

        //aggiungi auto al praco auto della stazione
        aggiungiAuto(stazione->parcoAuto, autonomiaAutoDaAggiungere);

        //auto aggiunta la parco auto
        printf("aggiunta\n");
    }
}

/**
 * Demolisci stazione dall'autostrada.
 */
void DemolisciStazione(){
    /**
    * Distanza dall'origine della stazione
    */
    unsigned int distanza=0;
    /**
     * Serve per far ritornare la scanf, se no NON compila.
     */
    int returnScanf;


    //lettura distanza della stazione da demolire
    returnScanf=scanf("%d", &distanza);
    if(returnScanf==EOF){
        return;
    }

    //Se stazione NON presente
    if(StazioneGiaPresente(autostrada, distanza)==0){
        //stazione NON demolita
        printf("non demolita\n");
    }
    //stazione presente
    else{
        //demolisci stazione
        eliminaStazione(autostrada, distanza);

        //stazione demolita
        printf("demolita\n");
    }
}

/**
 * Rottama auto alla stazione indicata.
 */
void RottamaAuto(){
    /**
    * Distanza della stazione dall'origine
    */
    unsigned int distanzaStazione=0;
    /**
     * Autonomia dell'auto da rottamare dalla stazione
     */
    unsigned int autonomiaAutoDaRottamare=0;
    /**
     * Serve per far ritornare la scanf, se no NON compila.
     */
    int returnScanf;
    /**
     * Stazione di interesse.
     */
    struct HashNodeStazione *stazione;


    //lettura distanzaStazione della stazione da cui rottamare una macchina
    returnScanf=scanf("%d", &distanzaStazione);
    if(returnScanf==EOF){
        return;
    }

    //Se stazione NON presente
    if(StazioneGiaPresente(autostrada, distanzaStazione)==0){
        //auto NON rottamata
        printf("non rottamata\n");

        //devo comunque leggere le auto inserite in input altrimenti restano nel buffer
        do {
            returnScanf= getc_unlocked(stdin);
        } while (returnScanf!='\n');
    }
    //stazione presente
    else{
        stazione= ricercaStazione(autostrada, distanzaStazione);

        //leggi chiave auto da rottamare
        returnScanf=scanf("%d", &autonomiaAutoDaRottamare);
        if(returnScanf==EOF){
            return;
        }

        //Se auto NON presente nel parco auto della stazione
        if(ricercaAuto(stazione->parcoAuto, autonomiaAutoDaRottamare)==0){
            //auto NON rottamata
            printf("non rottamata\n");
        }
        //auto presente nel parco auto della stazione
        else{
            //eliminiamo l'auto dal parco auto della stazione
            eliminaAuto(stazione->parcoAuto, autonomiaAutoDaRottamare);

            //salviamo la nuova autonomia massima
            stazione->autonomiaMassima= ricercaMassimaAutonomia(stazione->parcoAuto, stazione->parcoAuto->capacita);

            //auto rottamata
            printf("rottamata\n");
        }
    }
}

/**
 * Gestione comando di pianifica percorso dalla stazione di partenza a quella di arrivo.
 */
void PianificaPercorso(){
    /**
     * Distanza dall'origine della stazione di partenza
     */
    int distanzaStazionePartenza=0;
    /**
     * Distanza dall'origine della stazione di arrivo
     */
    int distanzaStazioneArrivo=0;
    /**
     * Serve per far ritornare la scanf, se no NON compila.
     */
    int returnScanf;
    /**
     * Numero di stazioni esistenti dalla partenza all'arrivo.
     */
    int numeroDiStazioni=0;
    /**
     * Tutte le stazioni comprese tra la partenza e l'arrivo.
     */
    struct ArrayNodeStazione *stazioniIntermedie;
    /**
     * Percorso tra la stazione di partenza e quella di arrivo ottimale.
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

    //stazione di partenza NON ha macchine
    if(ricercaStazione(autostrada, distanzaStazionePartenza)->autonomiaMassima==0){
        printf("nessun percorso\n");
    }
    //se stazione partenza == stazione arrivo
    else if(distanzaStazionePartenza==distanzaStazioneArrivo){
        printf("%d\n", distanzaStazionePartenza);
    }
    //ricerca il percorso ottimale per andare dalla partenza all'arrivo
    else{
        //selezionare tutte le stazioni tra quella di partenza e quella di arrivo
        stazioniIntermedie= tutteLeStazioni(distanzaStazionePartenza, distanzaStazioneArrivo, &numeroDiStazioni);
        //percorso ricercato
        percorsoTrovato= percorsoPianificato(stazioniIntermedie, numeroDiStazioni, distanzaStazionePartenza, distanzaStazioneArrivo);

        //se NON esiste il percorso
        if(percorsoTrovato==NULL){
            printf("nessun percorso\n");
        }
        //se esiste il percorso stamparlo
        else{
            stampaPercorsoPianificato(percorsoTrovato, distanzaStazioneArrivo);
        }

        //deallochiamo memoria che NON ci serve più per i prossimi percorsi
        free(stazioniIntermedie);
        if (percorsoTrovato!=NULL){
            free(percorsoTrovato);
        }
    }
}

/**
 * Eseguire il comando letto da stIN.
 *
 * @param comando stringa che rappresenta il comando da eseguire.
 */
void ProcessaComando(const char comando[]){
    switch (comando[0]) {
        //aggiungi: -stazione OR -auto
        case 'a':
            //aggiungi-stazione
            if(comando[9]=='s'){
                AggiungiStazione();
            }
            //aggiungi-auto
            else{
                AggiungiAuto();
            }
            break;
        //demolisci-stazione
        case 'd':
            DemolisciStazione();
            break;
        //rottama-auto
        case 'r':
            RottamaAuto();
            break;
        //pianifica-percorso
        case 'p':
            PianificaPercorso();
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
     * Carattere per capire non abbiamo più niente da leggere da stIN.
     */
    int fine;
    /**
     * Stringa che rappresenta il comando che bisogna eseguire.
     */
    char comando[LUNGHEZZA_MAX_COMANDI]="";


    //inizializzazione autostrada
    autostrada= creaAutostrada(DIMENSIONE_INIZIALE_AUTOSTRADA);

    //continua a leggere fino a che non leggi EOF
    do{
        //leggi i comandi che arrivano da stIN
        fine=scanf("%s", comando);

        //processa il comando letto
        ProcessaComando(comando);
    }while(fine!=EOF); //se leggi EOF termina

    return 0;
}
