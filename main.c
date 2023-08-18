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

/**
 * Struct --> Rappresenta un nodo nel percorso - serve per l'algoritmo A*.
 */
struct PercorsoNode{
    /**
     * Distanza a cui si trova la stazione.
     */
    unsigned int distanza;
    /**
     * Distanza percorsa fino a questa stazione.
     */
    double g;
    /**
     * Valore euristico - distanza euristica (funzione euristica).
     */
    double h;
    /**
     * Funzione di valutazione - g+h.
     */
    double f;
    /**
     * Indice del nodo genitore nell'Array della stazioni.
     */
    int genitore;
};



//VARIABILI GLOBALI
/**
 * Autostrada.
 */
struct HashTableAutostrada *autostrada=NULL;



//COMPARAZIONE
/**
 * Funzione che compara 2 nodi.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la comparazioneAvanti.
 */
int comparazioneAvanti(const void *a, const void *b){
    return (int) (((struct PercorsoNode *)a)->distanza-((struct PercorsoNode *)b)->distanza);
}

/**
 * Funzione che compara 2 nodi.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la comparazioneAvanti.
 */
int comparazioneIndietro(const void *a, const void *b){
    return (int) (((struct PercorsoNode *)b)->distanza-((struct PercorsoNode *)a)->distanza);
}

/**
 * Funzione che compara 2 nodi - comparazione tra distanza.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la comparazione.
 */
int comparazioneInAvanti(const void *a, const void *b){
    return (int) (((struct PercorsoNode *)a)->distanza-((struct PercorsoNode *)b)->distanza);
}

/**
 * Funzione che compara 2 nodi - comparazione tra f.
 *
 * @param a primo nodo da comparare.
 * @param b secondo nodo da comparare.
 * @return la comparazione.
 */
int comparazioneAllIndietro(const void *a, const void *b){
    return (int) (((struct PercorsoNode *)a)->f - ((struct PercorsoNode *)b)->f);
}



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

    //eliminiamo il vecchio parco auto
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

    //eliminiamo le vecchie stazioni
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

    //TODO elimina macchine - sembra NON essenziale - se da execution killed il problema è che manca questo
//    for (int i = 0; i < stazioneCorrente->parcoAuto->capacita; ++i) {
//        /**
//         * Auto corrente presente in posizione i nel parco auto.
//         */
//        struct HashNodeAuto *corrente=stazioneCorrente->parcoAuto->autoNode[i];
//        while (corrente!=NULL){
//            /**
//             * Nodo precedente da eliminare.
//             */
//            struct HashNodeAuto *precedente=corrente;
//            corrente=corrente->successivo;
//            free(precedente);
//        }
//    }
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



//GESTIONE HEAP
// MAX-HEAP
/**
 * Utilizzata per mantenere corrette le proprietà del Max-Heap dopo un'eliminazione.
 *
 * @param heap lo Heap da prendere in esame.
 * @param dimensione la dimensione dello Heap.
 * @param indice indice del nodo corrente - nodo che sta venendo spostato.
 */
void maxHeapify(struct PercorsoNode heap[], const unsigned int *dimensione, unsigned int indice){
    /**
     * Indice della parte sinistra dello Heap.
     */
    int sx=(int) (2*indice+1);
    /**
     * Indice della parte destra dello Heap.
     */
    int dx=(int) (2*indice+2);
    /**
     * Indice del nodo con il valore massimo tra il nodo corrente e i figli sx e dx.
     */
    int massimo=(int) indice;


    while (1) {
        //se il nodo a sinistra è più grande di quello che stiamo valutando
        if (sx < (*dimensione) && heap[sx].distanza > heap[indice].distanza) {
            massimo = sx;
        }
        //se il nodo a destra è più grande di quello che stiamo valutando
        if (dx < (*dimensione) && heap[dx].distanza > heap[massimo].distanza) {
            massimo = dx;
        }

        //se c'è un nodo più grande rispetto a quello che stiamo valutando - SWAPPING
        if (massimo != indice) {
            /**
             * Nodo minimo nello Heap.
             */
            struct PercorsoNode tmp = heap[massimo];

            //swapping
            heap[massimo] = heap[indice];
            heap[indice] = tmp;

            indice = massimo;
        }
        else {
            break;
        }
    }
}

/**
 * Funzione di supporto per riorganizzare il Max-Heap dopo un inserimento nello stesso.
 *
 * @param heap lo Heap che deve essere sistemato.
 * @param indice indice del nodo da sistemare.
 */
void sistemareInserimentoNelMaxHeap(struct PercorsoNode heap[], int indice){
    /**
     * Genitore dell'elemento dell'indice.
     */
    int indiceGenitore=(indice-1)/2;
    /**
     * Temporaneo per salvare il nodo.
     */
    struct PercorsoNode tmp=heap[indice];


    //fino a che il genitore è più piccolo del figlio
    while (indice > 0 && heap[indiceGenitore].distanza < tmp.distanza) {
        heap[indice] = heap[indiceGenitore];
        indice = indiceGenitore;
        indiceGenitore = (indice - 1) / 2;
    }

    heap[indice] = tmp;
}

/**
 * Inserisce nel Max-Heap un nuovo nodo.
 *
 * @param heap lo Heap a cui va aggiunto il nuovo nodo.
 * @param dimensione la dimensione dello Heap - # elementi nello Heap.
 * @param lunghezzaArray la lunghezza dell'Array che contiene lo Heap.
 * @param nodoDaAggiungere il nodo da aggiungere allo Heap.
 */
void inserimentoNelMaxHeap(struct PercorsoNode heap[], int *dimensione, int *lunghezzaArray, struct PercorsoNode nodoDaAggiungere){
    //se array è completamente pieno --> riallocare array
    if(*lunghezzaArray==*dimensione){
        //aumentare lunghezza array
        *lunghezzaArray=(*lunghezzaArray)+7;
        heap=(struct PercorsoNode *) realloc(heap, ((*lunghezzaArray)* sizeof(struct PercorsoNode)));
    }

    //aggiunta nodo in fondo allo Heap
    heap[(*dimensione)]=nodoDaAggiungere;
    //sistemiamo lo Heap
    sistemareInserimentoNelMaxHeap(heap, *dimensione);
    //aumentiamo il numero di elementi nello Heap
    ++(*dimensione);
}

/**
 * Restituisce il massimo nodo presente nel Max-Heap - lo elimina anche.
 *
 * @param heap lo Heap in cui va ricercato il nodo minimo.
 * @param dimensione la dimensione dello Heap.
 * @return il nodo minimo presente nello Heap.
 */
struct PercorsoNode estraiMassimoDelMaxHeap(struct PercorsoNode heap[], unsigned int *dimensione){
    /**
     * Nodo minimo presente nello Heap da ritornare e da eliminare da esso.
     */
    struct PercorsoNode nodoMassimo=heap[0];


    //sostituzione della radice con l'ultimo nodo
    heap[0]=heap[(*dimensione)-1];
    //riduci dimensione dello Heap
    --(*dimensione);

    //sistema lo Heap
    maxHeapify(heap, dimensione, 0);

    //ritorna il minimo
    return nodoMassimo;
}


// MIN-HEAP
/**
 * Utilizzata per mantenere corrette le proprietà del Min-Heap dopo un'eliminazione.
 *
 * @param heap lo Heap da prendere in esame.
 * @param dimensione la dimensione dello Heap.
 * @param indice indice del nodo corrente - nodo che sta venendo spostato.
 */
void minHeapify(struct PercorsoNode heap[], int *dimensione, unsigned int indice){
    /**
     * Indice della parte sinistra dello Heap.
     */
    int sx=(int) (2*indice);
    /**
     * Indice della parte destra dello Heap.
     */
    int dx=(int) (2*indice+1);
    /**
     * Indice del nodo con il valore più minimo tra il nodo corrente e i figli sx e dx.
     */
    int minimo=(int) indice;


    //se il nodo a sinistra è più piccolo di quello che stiamo valutando
    if(sx<(*dimensione) && heap[sx].distanza<heap[indice].distanza){
        //il più piccolo diventa quello a sinistra
        minimo=sx;
    }
    else{
        minimo=(int) indice;
    }
    //se il nodo a destra è più piccolo di quello che stiamo valutando
    if(dx<(*dimensione) && heap[dx].distanza<heap[indice].distanza){
        //il più piccolo diventa quello a destra
        minimo=dx;
    }

    //se c'è un nodo più piccolo rispetto a quello che stiamo valutando - SWAPPING
    if(minimo!=indice){
        /**
         * Nodo minimo nello Heap.
         */
        struct PercorsoNode tmp=heap[indice];

        //swapping
        heap[indice]=heap[minimo];
        heap[minimo]=tmp;

        printf(" ->MINHEAPIFY ");
        for (int i = 0; i < (*dimensione); ++i) {
            printf("%d ", heap[i].distanza);
        }
        printf("\n");

        //ricorsione per sistemare tutto lo Heap
        minHeapify(heap, dimensione, minimo);
    }

    printf(" ->MINHEAPIFY ");
    for (int i = 0; i < (*dimensione); ++i) {
        printf("%d ", heap[i].distanza);
    }
    printf("\n");
}

/**
 * Funzione di supporto per riorganizzare il Min-Heap dopo un inserimento nello stesso.
 *
 * @param heap lo Heap che deve essere sistemato.
 * @param indice indice del nodo da sistemare.
 */
void sistemareInserimentoNelMinHeap(struct PercorsoNode heap[], int indice){
    /**
     * Genitore dell'elemento dell'indice.
     */
    int indiceGenitore=(int) ((indice-1)/2);
    /**
     * Temporaneo per salvare il nodo.
     */
    struct PercorsoNode tmp=heap[indice];


    //fino a che genitore è più grande del figlio
    while (indice > 0 && heap[indiceGenitore].distanza > tmp.distanza) { //TODO forse è qua il problema
        heap[indice] = heap[indiceGenitore];
        heap[indiceGenitore]=tmp;
        indice = indiceGenitore;
        indiceGenitore = (indice - 1) / 2;
    }

    heap[indice] = tmp;
}

/**
 * Inserisce nel Min-Heap un nuovo nodo.
 *
 * @param heap lo Heap a cui va aggiunto il nuovo nodo.
 * @param dimensione la dimensione dello Heap - # elementi nello Heap.
 * @param lunghezzaArray la lunghezza dell'Array che contiene lo Heap.
 * @param nodoDaAggiungere il nodo da aggiungere allo Heap.
 */
void inserimentoNelMinHeap(struct PercorsoNode heap[], int *dimensione, int *lunghezzaArray, struct PercorsoNode nodoDaAggiungere){
    //se array è completamente pieno --> riallocare array
    if(*lunghezzaArray==*dimensione){
        //aumentare lunghezza array
        (*lunghezzaArray)=(*lunghezzaArray)+7;
        heap=(struct PercorsoNode *) realloc(heap, ((*lunghezzaArray)* sizeof(struct PercorsoNode)));
    }

    //aggiunta nodo in fondo allo Heap
    heap[(*dimensione)]=nodoDaAggiungere;
    printf(" ->INSERIMENTO ");
    for (int i = 0; i < (*dimensione)+1; ++i) {
        printf("%d ", heap[i].distanza);
    }
    printf("\n");
    //aumentiamo il numero di elementi nello Heap
    ++(*dimensione);
    //sistemiamo lo Heap
    sistemareInserimentoNelMinHeap(heap, *dimensione);

    printf(" ->SISTEMA INSERIMENTO ");
    for (int i = 0; i < (*dimensione); ++i) {
        printf("%d ", heap[i].distanza);
    }
    printf("\n");
}

/**
 * Restituisce il minimo nodo presente nel Min-Heap - lo elimina anche.
 *
 * @param heap lo Heap in cui va ricercato il nodo minimo.
 * @param dimensione la dimensione dello Heap.
 * @return il nodo minimo presente nello Heap.
 */
struct PercorsoNode estraiMinimoDalMinHeap(struct PercorsoNode heap[], int *dimensione){
    /**
     * Nodo minimo presente nello Heap da ritornare e da eliminare da esso.
     */
    struct PercorsoNode nodoMinimo=heap[0];


    //sostituzione della radice con l'ultimo nodo
    heap[0]=heap[(*dimensione)-1];
    //riduci dimensione dello Heap
    --(*dimensione);

    printf(" ->POP HEAPIFY ");
    for (int i = 0; i < (*dimensione); ++i) {
        printf("%d ", heap[i].distanza);
    }
    printf("\n");

    //sistema lo Heap
    minHeapify(heap, dimensione, 0);

    printf(" ->POP ");
    for (int i = 0; i < (*dimensione); ++i) {
        printf("%d ", heap[i].distanza);
    }
    printf("\n");

    //ritorna il minimo
    return nodoMinimo;
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

        //numero di stazioni tra quella di partenza e quella d'arrivo
        (*numeroDiStazioni)=posizioneArray;
        //stazioni intermedie tra quella di partenza e quella di arrivo
        stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (*numeroDiStazioni)*sizeof(struct ArrayNodeStazione));
        return stazioniIntermedie;
    }
    //controllare tutte le stazioni se sono comprese tra partenza e arrivo
    else{
        for (int i = 0; i < autostrada->capacita; ++i) {
            //nella cella i c'è almeno 1 stazione
            if(autostrada->stazioni[i]!=NULL){
                /**
                 * Nodo corrente presente nella cella i.
                 */
                struct HashNodeStazione *corrente=autostrada->stazioni[i];

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

        //numero di stazioni tra quella di partenza e quella d'arrivo
        (*numeroDiStazioni)=posizioneArray;
        //stazioni intermedie tra quella di partenza e quella di arrivo
        stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (*numeroDiStazioni)*sizeof(struct ArrayNodeStazione));
        qsort(stazioniIntermedie, (*numeroDiStazioni), sizeof(struct ArrayNodeStazione), comparazioneAvanti);
        return stazioniIntermedie;
    }
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

        //numero di stazioni tra quella di partenza e quella d'arrivo
        (*numeroDiStazioni)=posizioneArray;
        //stazioni intermedie tra quella di partenza e quella di arrivo
        stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, ((*numeroDiStazioni)*sizeof(struct ArrayNodeStazione)));
        return stazioniIntermedie;
    }
    //controllare tutte le stazioni se sono comprese tra partenza e arrivo
    else{
        for (int i = 0; i < autostrada->capacita; ++i) {
            //nella cella i c'è almeno 1 stazione
            if(autostrada->stazioni[i]!=NULL){
                /**
                 * Nodo corrente presente nella cella i.
                 */
                struct HashNodeStazione *corrente=autostrada->stazioni[i];

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

        //numero di stazioni tra quella di partenza e quella d'arrivo
        (*numeroDiStazioni)=posizioneArray;
        //stazioni intermedie tra quella di partenza e quella di arrivo
        stazioniIntermedie=(struct ArrayNodeStazione *) realloc(stazioniIntermedie, (*numeroDiStazioni)*sizeof(struct ArrayNodeStazione));
        qsort(stazioniIntermedie, (*numeroDiStazioni), sizeof(struct ArrayNodeStazione), comparazioneIndietro);
        return stazioniIntermedie;
    }
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
    if(partenza<arrivo){
        return tutteLeStazioniInAvanti(partenza, arrivo, numeroDiStazioni);
    }
    else{
        return tutteLeStazioniAllIndietro(partenza, arrivo, numeroDiStazioni);
    }
}

/**
 * Ritorna la distanza tra 2 stazioni - funzione euristica euclidea.
 *
 * @param a stazione A.
 * @param b stazione B.
 * @return la distanza tra la stazione A e B.
 */
double distanzaEuclideaEuristica(unsigned int a, unsigned int b){
    return abs((int) ((int) b - (int) a));
}

/**
 * Ritorna il punteggio che ha il percorso (dalla partenza all'arrivo) - ( -Distanza Euristica ).
 *
 * @param partenza punto di partenza.
 * @param arrivo punto di arrivo.
 * @return il punteggio che ha la stazione vicina.
 */
double punteggioPartenzaArrivo(int partenza, int arrivo){
    return -distanzaEuclideaEuristica(partenza, arrivo);
}

/**
 * Estrai il nodo minimo - comparazione tra distanze.
 *
 * @param set il Set da cui estrarre il mimino.
 * @param setSize la dimensione del Set.
 * @return il nodo minimo.
 */
struct PercorsoNode popMin(struct PercorsoNode set[], int *setSize){
    //ordinamento in base alla comparazioneInAvanti
    qsort(set, *setSize, sizeof(struct PercorsoNode), comparazioneInAvanti);
    /**
     * Nodo minimo da ritornare.
     */
    struct PercorsoNode minimo=set[0];

    //rimuovi il nodo minimo dal set
    for (int i = 0; i < *setSize; ++i) {
        set[i]=set[i+1];
    }
    --(*setSize);

    //ritorna il nodo minimo
    return minimo;
}

/**
 * Estrai il nodo massimo - comparazione tra .f.
 *
 * @param set il Set da cui estrarre il massimo.
 * @param setSize la dimensione del Set.
 * @return il nodo massimo.
 */
struct PercorsoNode popMax(struct PercorsoNode set[], int *setSize){
    //ordinamento in base alla comparazioneAllIndietro
    qsort(set, *setSize, sizeof(struct PercorsoNode), comparazioneAllIndietro);
    /**
     * Nodo massimo da ritornare.
     */
    struct PercorsoNode massimo=set[(*setSize)-1];

    //rimuovi nodo massimo dal set
    set[(*setSize)-1].distanza=0;
    set[(*setSize)-1].g=0;
    set[(*setSize)-1].h=0;
    set[(*setSize)-1].f=0;
    set[(*setSize)-1].genitore=0;
    --(*setSize);

    //ritorna nodo massimo
    return massimo;
}

/**
 * Ritorna l'indice della stazione corrente - ordinamento crescente.
 *
 * @param stazione le stazioni presenti tra il nodo di partenza e quello di arrivo - ordine crescente.
 * @param numeroStazioni numero di stazioni intermedie.
 * @param corrente stazione corrente.
 * @param indicePrecedente indice della stazione precedente a quella corrente attualmente.
 * @return indice della stazione corrente.
 */
int ricercaIndiceStazioneCorrenteInAvanti(struct ArrayNodeStazione stazione[], int numeroStazioni, struct PercorsoNode corrente){
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
    int medio=0;


    //fino a che sx è minore uguale a dx
    while (sx<=dx){
        //indice da controllare
        medio=((int) sx+((int) ((dx-sx)/2)));

        //controllo se il valore è uguale a quello cercato
        if(stazione[medio].distanza==corrente.distanza){
            return medio;
        }
        //il valore da ricercare si trova nella metà superiore
        else if(stazione[medio].distanza<corrente.distanza){
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
 * Ritorna l'indice della stazione corrente - ordinamento decrescente.
 *
 * @param stazione le stazioni presenti tra il nodo di partenza e quello di arrivo - ordine decrescente.
 * @param numeroStazioni numero di stazioni intermedie.
 * @param corrente stazione corrente.
 * @param indicePrecedente indice della stazione precedente a quella corrente attualmente.
 * @return indice della stazione corrente.
 */
int ricercaIndiceStazioneCorrenteAllIndietro(struct ArrayNodeStazione stazione[], int numeroStazioni, struct PercorsoNode corrente){
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
    int medio=0;


    //fino a che sx è minore uguale a dx
    while (sx<=dx){
        //indice da controllare
        medio=((int) sx+((int) ((dx-sx)/2)));

        //controllo se il valore è uguale a quello cercato
        if(stazione[medio].distanza==corrente.distanza){
            return medio;
        }
            //il valore da ricercare si trova nella metà superiore
        else if(stazione[medio].distanza>corrente.distanza){
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
 * Algoritmo A* per la ricerca del percorso minimo - calcola il percorso all'andata.
 *
 * @param stazioni insieme dei tutte le stazioni tra partenza e arrivo.
 * @param numeroStazioni numero di stazioni tra partenza e arrivo.
 * @param partenza indice della stazione di partenza.
 * @param arrivo indice della stazione di arrivo.
 * @param percorso percorso minimo tra partenza e arrivo.
 * @return la lunghezza del percorso minimo, 0 se NON ha trovato un percorso.
 */
int aStarInAvanti(struct ArrayNodeStazione stazioni[], int numeroStazioni, int partenza, int arrivo, struct PercorsoNode **percorso){
    /**
     * Nodo di partenza.
     */
    struct PercorsoNode start={partenza, 0.0, 0.0, 0.0, -1};
    /**
     * Contiene tutti i tentativi di nodi da valutare.
     */
    struct PercorsoNode *openSet=(struct PercorsoNode *) calloc(numeroStazioni, sizeof(struct PercorsoNode));
    /**
     * Contiene tutti i nodi già valutati.
     */
    struct PercorsoNode *closeSet=(struct PercorsoNode *) calloc(numeroStazioni, sizeof(struct PercorsoNode));
    /**
     * Numero di elementi nell'openSet.
     */
    int openSize=0;
    /**
     * Numero di elementi nel closeSet.
     */
    int closeSize=0;
    /**
     * Nodo minimo corrente presente nell'openSet.
     */
    struct PercorsoNode corrente;
    /**
     * Nodo raggiungibile da quello corrente.
     */
    struct PercorsoNode vicino;
    /**
     * Indice della stazione corrente che stiamo visitando.
     */
    int indiceStazioneCorrente=0;
    /**
     * Indice della stazione raggiungibile da quella corrente.
     */
    int indiceVicino;
    /**
     * h appena calcolata - costo stimato.
     */
    double h;
    /**
     * g appena calcolata - costo reale.
     */
    double g;
    /**
     * f appena calcolata - g+h.
     */
    double f;
    /**
     * Nodo è stato trovato.
     */
    int trovato;
    /**
     * Indici per i for.
     */
    int i;
    /**
     * Distanza (ID) stazione vicino a quella corrente.
     */
    int distanzaVicino;
    /**
     * Autonomia massima stazione corrente.
     */
    int autonomiaMassimaCorrente;
    /**
     * Nuova dimensione del closeSet.
     */
    int nuovaDimensioneCloseSet=numeroStazioni;
    /**
     * Nuova dimensione del openSet.
     */
    int nuovaDimensioneOpenSet=numeroStazioni;


    //aggiunta nodo di partenza all'openSet
    inserimentoNelMinHeap(openSet, &openSize, &nuovaDimensioneOpenSet, start);

    //fino a che ci sono nodi da valutare
    while (openSize>0){
        //estrazione nodo minimo dall'openSet
        corrente= estraiMinimoDalMinHeap(openSet, &openSize);
        //aggiungi nodo corrente al closeSet
        if(closeSize!=0 && closeSize%numeroStazioni==0){
            nuovaDimensioneCloseSet=closeSize+numeroStazioni;
            closeSet=(struct PercorsoNode *) realloc(closeSet, (nuovaDimensioneCloseSet* sizeof(struct PercorsoNode)));
        }
        closeSet[closeSize]=corrente;
        ++closeSize;


        //indice della stazione corrente nell'Array delle stazioni
        indiceStazioneCorrente= ricercaIndiceStazioneCorrenteInAvanti(stazioni, numeroStazioni, corrente);

        //se siamo arrivati alla fine
        if(corrente.distanza==arrivo){
            /**
             * Per reallocare il percorso.
             */
            struct PercorsoNode *daRitornare=(struct PercorsoNode *) malloc((closeSize* sizeof(struct PercorsoNode)));
            //copia il closeSet che contiene il percorso
            memcpy(daRitornare, closeSet, (closeSize* sizeof(struct PercorsoNode)));
            //libera la memoria occupata precedentemente
            free(*percorso);
            //salva il percorso
            *percorso=daRitornare;

            free(openSet);
            free(closeSet);
            return closeSize;
        }

        indiceVicino=indiceStazioneCorrente+1;
        distanzaVicino=(int) stazioni[indiceVicino].distanza;
        autonomiaMassimaCorrente=(int) stazioni[indiceStazioneCorrente].autonomiaMassima;
        autonomiaMassimaCorrente=(int) ((int) corrente.distanza+autonomiaMassimaCorrente);
        //per ogni nodo vicino ==> raggiungibile
        while(distanzaVicino <= autonomiaMassimaCorrente){
            //calcolo costi
            h= distanzaEuclideaEuristica(corrente.distanza, stazioni[indiceVicino].distanza);
            g=corrente.g+h;
            f=g+h;

            //il vicino è già stato visitato - è nel closeSet
            trovato=0;
            for (i = 0; i < closeSize; ++i) {
                //il vicino è già stato visitato
                if(closeSet[i].distanza==stazioni[indiceVicino].distanza){
                    trovato=1;
                    break;
                }
            }
            //vicino già stato visitato e nuovo percorso NON è meglio
            if(trovato && f>=closeSet[i].f){
                //guardiamo il prossimo vicino
                ++indiceVicino;
                //se abbiamo raggiunto il numeroStazioni (parte da 1) dobbiamo interrompere la ricerca nelle stazioni vicine
                if(indiceVicino==numeroStazioni){
                    break;
                }
                distanzaVicino=(int) stazioni[indiceVicino].distanza;
                continue;
            }
            //vicino NON trovato e/o percorso migliore
            vicino.distanza=stazioni[indiceVicino].distanza;
            vicino.h=h;
            vicino.g=g;
            vicino.f=f;
            vicino.genitore=closeSize-1;

            //il vicino è già presente tra quelli da controllare - è nell'openSet
            trovato=0;
            for (i = 0; i < openSize; ++i) {
                //il vicino è già stato visitato
                if(openSet[i].distanza==stazioni[indiceVicino].distanza){
                    trovato=1;
                    break;
                }
            }
            //vicino NON è ancora presente
            if(!trovato){
                inserimentoNelMinHeap(openSet, &openSize, &nuovaDimensioneOpenSet, vicino);
            }

            //guardiamo il prossimo vicino
            ++indiceVicino;
            //se abbiamo raggiunto il numeroStazioni (parte da 1) dobbiamo interrompere la ricerca nelle stazioni vicine
            if(indiceVicino==numeroStazioni){
                break;
            }
            distanzaVicino=(int) stazioni[indiceVicino].distanza;
        }
    }

    //percorso NON trovato
    free(openSet);
    free(closeSet);
    return 0;
}

/**
 * Algoritmo A* per la ricerca del percorso minimo - calcola il percorso al ritorno.
 *
 * @param stazioni insieme dei tutte le stazioni tra partenza e arrivo.
 * @param numeroStazioni numero di stazioni tra partenza e arrivo.
 * @param partenza indice della stazione di partenza.
 * @param arrivo indice della stazione di arrivo.
 * @param percorso percorso minimo tra partenza e arrivo.
 * @return la lunghezza del percorso minimo, 0 se NON ha trovato un percorso.
 */
int aStarAllIndietro(struct ArrayNodeStazione stazioni[], int numeroStazioni, int partenza, int arrivo, struct PercorsoNode **percorso){
    /**
     * Nodo di partenza.
     */
    struct PercorsoNode start={partenza, 0.0, 0.0, 0.0, -1};
    /**
     * Contiene tutti i tentativi di nodi da valutare.
     */
    struct PercorsoNode *openSet=(struct PercorsoNode *) calloc(numeroStazioni, sizeof(struct PercorsoNode)); //TODO trasformarlo in un MAX HEAP - prima vanno sistemate le funzioni del max heap
    /**
     * Contiene tutti i nodi già valutati.
     */
    struct PercorsoNode *closeSet=(struct PercorsoNode *) calloc(numeroStazioni, sizeof(struct PercorsoNode));
    /**
     * Numero di elementi nell'openSet.
     */
    int openSize=0;
    /**
     * Numero di elementi nel closeSet.
     */
    int closeSize=0;
    /**
     * Nodo minimo corrente presente nell'openSet.
     */
    struct PercorsoNode corrente;
    /**
     * Nodo raggiungibile da quello corrente.
     */
    struct PercorsoNode vicino;
    /**
     * Indice della stazione corrente che stiamo visitando.
     */
    int indiceStazioneCorrente;
    /**
     * Indice della stazione raggiungibile da quella corrente.
     */
    int indiceVicino;
    /**
     * h appena calcolata - costo stimato.
     */
    double h;
    /**
     * g appena calcolata - costo reale.
     */
    double g;
    /**
     * f appena calcolata - g+h.
     */
    double f;
    /**
     * Nodo è stato trovato.
     */
    int trovato;
    /**
     * Indici per i for.
     */
    int i;
    /**
     * Distanza (ID) stazione vicino a quella corrente.
     */
    int distanzaVicino;
    /**
     * Autonomia massima stazione corrente.
     */
    int autonomiaMassimaCorrente;
    /**
     * Nuova dimensione del closeSet.
     */
    int nuovaDimensioneCloseSet=0;
    /**
     * Nuova dimensione del openSet.
     */
    int nuovaDimensioneOpenSet=0;


    //aggiunta nodo di partenza all'openSet
    openSet[openSize]=start;
    ++openSize;

    //fino a che ci sono nodi da valutare
    while (openSize>0){
        //estrazione nodo minimo dall'openSet --> deve avere minima la .f
        corrente= popMax(openSet, &openSize);
        //aggiunta nodo corrente al closeSet
        if(closeSize!=0 && closeSize%numeroStazioni==0){
            nuovaDimensioneCloseSet=closeSize+numeroStazioni;
            closeSet=(struct PercorsoNode *) realloc(closeSet, (nuovaDimensioneCloseSet*sizeof(struct PercorsoNode)));
        }
        closeSet[closeSize]=corrente;
        ++closeSize;


        //indice della stazione corrente nell'Array delle stazioni
        indiceStazioneCorrente= ricercaIndiceStazioneCorrenteAllIndietro(stazioni, numeroStazioni, corrente);

        //se siamo arrivati alla fine
        if(corrente.distanza==arrivo){
            /**
             * Per reallocare il percorso.
             */
            struct PercorsoNode *daRitornare=(struct PercorsoNode *) malloc((closeSize* sizeof(struct PercorsoNode)));
            //copia il closeSet che contiene il percorso
            memcpy(daRitornare, closeSet, (closeSize* sizeof(struct PercorsoNode)));
            //libera la memoria occupata precedentemente
            free(*percorso);
            //salva il percorso
            *percorso=daRitornare;

            free(openSet);
            free(closeSet);
            return closeSize;
        }

        indiceVicino=indiceStazioneCorrente+1;
        distanzaVicino=(int) stazioni[indiceVicino].distanza;
        autonomiaMassimaCorrente=(int) stazioni[indiceStazioneCorrente].autonomiaMassima;
        autonomiaMassimaCorrente=(int) (((int) corrente.distanza)-autonomiaMassimaCorrente);
        //per ogni nodo vicino ==> raggiungibile
        while(distanzaVicino >= autonomiaMassimaCorrente){
            //calcolo costi
            h= distanzaEuclideaEuristica(corrente.distanza, stazioni[indiceVicino].distanza) +
                    punteggioPartenzaArrivo(partenza, arrivo);
            g=corrente.g+h;
            f=g+h;

            //il vicino è già stato visitato - è nel closeSet
            trovato=0;
            for (i = 0; i < closeSize; ++i) {
                //il vicino è già stato visitato
                if(closeSet[i].distanza==stazioni[indiceVicino].distanza){
                    trovato=1;
                    break;
                }
            }
            //vicino già stato visitato e nuovo percorso NON è meglio
            if(trovato && f>=closeSet[i].f){
                //guardiamo il prossimo vicino
                ++indiceVicino;
                //se abbiamo raggiunto il numeroStazioni (parte da 1) dobbiamo interrompere la ricerca nelle stazioni vicine
                if(indiceVicino==numeroStazioni){
                    break;
                }
                distanzaVicino=(int) stazioni[indiceVicino].distanza;
                continue;
            }
            //vicino NON trovato e/o percorso migliore
            vicino.distanza=stazioni[indiceVicino].distanza;
            vicino.h=h;
            vicino.g=g;
            vicino.f=f;
            vicino.genitore=closeSize-1;

            //il vicino è già presente tra quelli da controllare - è nell'openSet
            trovato=0;
            for (i = 0; i < openSize; ++i) {
                //il vicino è già stato visitato
                if(openSet[i].distanza==stazioni[indiceVicino].distanza){
                    trovato=1;
                    break;
                }
            }
            //vicino NON è ancora presente
            if(!trovato){
                if(openSize!=0 && openSize%numeroStazioni==0){
                    nuovaDimensioneOpenSet=openSize+numeroStazioni;
                    openSet=(struct PercorsoNode *) realloc(openSet, (nuovaDimensioneOpenSet* sizeof(struct PercorsoNode)));
                }
                openSet[openSize++]=vicino;
            }

            //guardiamo il prossimo vicino
            ++indiceVicino;
            //se abbiamo raggiunto il numeroStazioni (parte da 1) dobbiamo interrompere la ricerca nelle stazioni vicine
            if(indiceVicino==numeroStazioni){
                break;
            }
            distanzaVicino=(int) stazioni[indiceVicino].distanza;
        }
    }

    //percorso NON trovato
    free(openSet);
    free(closeSet);
    return 0;
}

/**
 * Algoritmo A* per la ricerca del percorso minimo - sceglie se il percorso da calcolare è all'andata o al ritorno.
 *
 * @param stazioni insieme dei tutte le stazioni tra partenza e arrivo.
 * @param numeroStazioni numero di stazioni tra partenza e arrivo.
 * @param partenza indice della stazione di partenza.
 * @param arrivo indice della stazione di arrivo.
 * @param percorso percorso minimo tra partenza e arrivo.
 * @return la lunghezza del percorso minimo, 0 se NON ha trovato un percorso.
 */
int aStar(struct ArrayNodeStazione stazioni[], int numeroStazioni, int partenza, int arrivo, struct PercorsoNode **percorso){ //TODO da migliorare efficienza
    //calcolare il percorso all'andata
    if(partenza<arrivo){
        return aStarInAvanti(stazioni, numeroStazioni, partenza, arrivo, percorso);
    }
    //calcolare il percorso al ritorno
    else{
        return 0;
//        aStarAllIndietro(stazioni, numeroStazioni, partenza, arrivo, percorso);
    }
}

/**
 * Stampa il percorso calcolato.
 *
 * @param numeroDiStazioni numero di stazioni toccate.
 * @param percorso percorso dalla stazione di partenza a quella di arrivo.
 * @param partenza distanza stazione di partenza.
 * @param arrivo distanza stazione di arrivo.
 */
void stampaPercorso(int numeroDiStazioni, struct PercorsoNode *percorso, unsigned int partenza, unsigned int arrivo){ //TODO si può fare meglio??
    /**
     * Stazione corrente.
     */
    struct PercorsoNode corrente=percorso[numeroDiStazioni-1];
    /**
     * Fermate effettivamente fatte.
     */
    unsigned int fermate[numeroDiStazioni];
    /**
     * Numero di fermate effettivamente fatte.
     */
    int numeroDiFermate=0;


    //fino a che NON siamo arrivati al nodo di partenza
    while (corrente.genitore!=-1){
        //salviamo le fermate in ordine inverso
        fermate[numeroDiFermate]=corrente.distanza;
        corrente=percorso[corrente.genitore];
        ++numeroDiFermate;
    }
    //stazione di partenza
    fermate[numeroDiFermate]=partenza;
    ++numeroDiFermate;

    //stampa le stazioni nell'ordine giusto
    for (int i = numeroDiFermate-1; i > 0 ; --i) {
        printf("%d ", fermate[i]);
    }
    printf("%d\n", arrivo);
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
        if(returnScanf==EOF){
            return;
        }
    }
    //stazione presente
    else{
        /**
         * Stazione di interesse.
         */
        struct HashNodeStazione *stazione= ricercaStazione(autostrada, distanzaStazione);

        //leggi chiave auto da aggiungere
        returnScanf=scanf("%d", &autonomiaAutoDaAggiungere);
        if(returnScanf==EOF){
            return;
        }

        //distanza massima percorribile da quella stazione
        if(autonomiaAutoDaAggiungere>stazione->autonomiaMassima){
            stazione->autonomiaMassima=autonomiaAutoDaAggiungere;
        }

        //aggiungi auto al parco auto della stazione
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
        /**
         * Stazione di interesse.
         */
        struct HashNodeStazione *stazione= ricercaStazione(autostrada, distanzaStazione);

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
 * Pianifica percorso dalla stazione di partenza a quella di arrivo.
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
        /**
         * Numero di stazioni esistenti dalla partenza all'arrivo.
         */
        int numeroDiStazioni=0;
        /**
         * Tutte le stazioni comprese tra la partenza e l'arrivo.
         */
        struct ArrayNodeStazione *stazioniIntermedie= tutteLeStazioni(distanzaStazionePartenza, distanzaStazioneArrivo, &numeroDiStazioni);
        /**
         * Percorso minimo dalla stazione di partenza fino a quella di arrivo.
         */
        struct PercorsoNode *percorso=(struct PercorsoNode *) calloc(numeroDiStazioni, sizeof(struct PercorsoNode));
        /**
         * Numero di fermate che bisogna compiere nel percorso.
         */
        int numeroFermate= aStar(stazioniIntermedie, numeroDiStazioni, distanzaStazionePartenza, distanzaStazioneArrivo, &percorso);

        //se NON esiste il percorso
        if(numeroFermate==0){
            printf("nessun percorso\n");
        }
        //stampare il percorso
        else{
            stampaPercorso(numeroFermate, percorso, distanzaStazionePartenza, distanzaStazioneArrivo);
        }

        //deallochiamo memoria che NON ci serve più per i prossimi percorsi
        free(stazioniIntermedie);
        free(percorso);
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



/**
 * Deallocare tutta la memoria usata nel programma.
 */
void deallocaMemoria(){
    //per tutte le auto
    for (int i = 0; i < autostrada->capacita; ++i) {
        if(autostrada->stazioni[i]!=NULL){
            //per tutto il parco auto nella stazione
            for (int j = 0; j < autostrada->stazioni[i]->parcoAuto->capacita; ++j) {
                /**
                 * Nodo corrente - auto.
                 */
                struct HashNodeAuto *corrente=autostrada->stazioni[i]->parcoAuto->autoNode[j];
                //elimina auto
                while (corrente!=NULL){
                    /**
                     * Nodo successivo a quello corrente - auto.
                     */
                    struct HashNodeAuto *successivo=corrente->successivo;
                    //elimina auto
                    free(corrente);
                    corrente=successivo;
                }
            }
            //elimina parco auto dalla stazione
            free(autostrada->stazioni[i]->parcoAuto);
        }
    }

    //elimina tutte le stazioni
    free(autostrada->stazioni);

    //elimina autostrada
    free(autostrada);
}



//MAIN
/**
 * Main del programma.
 *
 * @return 0 se il programma ha terminato l'esecuzione correttamente.
 */
int main() {
    /**
     * Carattere per capire non abbiamo più niente da leggere da stdIN.
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

//    deallocaMemoria(); TODO NON serve ma se servisse già c'è
    return 0;
}


/*TODO
 * aStar --> al posto di pila usare heap
 */