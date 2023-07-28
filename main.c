//Alessandro Conti

#include <stdio.h>
#include <stdlib.h>



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
    struct HashNodeAuto **table;
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
    struct HashNodeStazione **table;
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
unsigned int nuovaCapacita(unsigned int capacitaIniziale){
    return capacitaIniziale*2+11;
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
    hashTable->table=(struct HashNodeAuto **) calloc(hashTable->capacita, sizeof(struct HashNodeAuto *));

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
    unsigned int capacitaNuova= nuovaCapacita(parcoAuto->capacita);
    /**
     * Nuovo indice in cui spostare gli HashNode dalla HashTable vecchia a quella nuova.
     */
    unsigned int nuovoIndice=0;
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
        corrente=parcoAuto->table[i];

        //fino a che NON abbiamo finito di spostare tutte le HashNode
        while(corrente!=NULL){
            //salviamo il prossimo HashNode da visitare
            prossimo=corrente->successivo;
            //salviamo il nuovo indice in cui andare a mettere la HashNode corrente
            nuovoIndice= funzioneDiHash(corrente->autonomia, capacitaNuova);

            //spostiamo il nodo
            corrente->successivo=nuovaHashTable[nuovoIndice];
            nuovaHashTable[nuovoIndice]=corrente;
            //il nodo corrente da vedere diventa il prossimo
            corrente=prossimo;
        }
    }

    //eliminiamo la vecchia table
    free(parcoAuto->table);
    //aggiungiamo la nuova table
    parcoAuto->table=nuovaHashTable;
    //aggiorniamo la capacità
    parcoAuto->capacita=capacitaNuova;
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
    nuovoNodo->successivo=parcoAuto->table[indice];
    parcoAuto->table[indice]=nuovoNodo;
    //è stato aggiunto un nuovo nodo, questo implica che bisogna aumentare la dimensione della HashTable
    parcoAuto->dimensione++;

    //bisogna reallocare la HashTable
    if(parcoAuto->dimensione > parcoAuto->capacita*PERCENTUALE_REALLOC){
        reHashParcoAuto(parcoAuto);
    }
}

/**
 * Elimina l'auto richiesta dal parco auto.
 *
 * @param parcoAuto il parco auto che contine la macchina da eliminare.
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
    struct HashNodeAuto *corrente=parcoAuto->table[indice];
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
            parcoAuto->table[indice]=corrente->successivo;
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
    struct HashNodeAuto *corrente=parcoAuto->table[indice];


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
        corrente=parcoAuto->table[i];
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
    hashTable->table=(struct HashNodeStazione **) calloc(hashTable->capacita, sizeof(struct HashNodeStazione *));

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
    unsigned int nuovoIndice=0;
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
        corrente=htAutostrada->table[i];

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

    //eliminiamo la vecchia table
    free(htAutostrada->table);
    //aggiungiamo la nuova table
    htAutostrada->table=nuovaHashTable;
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
    nuovoNodo->successivo=htAutostrada->table[indice];
    htAutostrada->table[indice]=nuovoNodo;
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
     * Nodo corrente,
     * corrente=corrente->successivo.
     */
    struct HashNodeStazione *corrente=htAutostrada->table[indice];
    /**
     * Nodo precedente,
     * precedente=corrente.
     */
    struct HashNodeStazione *precedente=NULL;


    //trova il nodo con la chiave corrispondente, assegna corrente AND precedente
    while (corrente!=NULL && corrente->distanza!=distanza){
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
            htAutostrada->table[indice]=corrente->successivo;
        }
        //il nodo da eliminare NON è il primo della lista presente nella posizione indicate
        else{
            precedente->successivo=corrente->successivo;
        }
    }

    //dealloca il HashNode
    free(corrente->parcoAuto);
    free(corrente);
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
    struct HashNodeStazione *corrente=htAutostrada->table[indice];

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
    return corrente;
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
    struct HashNodeStazione *corrente=htAutostrada->table[indice];


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
/**
 * Utilizzata per mantenere corrette le proprietà dello Heap dopo un'eliminazione.
 *
 * @param heap lo Heap da prendere in esame.
 * @param dimensione la dimensione dello Heap.
 * @param indice indice del nodo corrente - nodo che sta venendo spostato.
 */
void minHeapify(struct PercorsoNode heap[], unsigned int *dimensione, unsigned int indice){
    /**
     * Indice della parte sinistra dello Heap.
     */
    int sx=(int) (2*indice+1);
    /**
     * Indice della parte destra dello Heap.
     */
    int dx=(int) (2*indice+2);
    /**
     * Indice del nodo con il valore più minimo tra il nodo corrente e i figli sx e dx.
     */
    int minimo=(int) indice;


    //se il nodo a sinistra è più piccolo di quello che stiamo valutando
    if(sx<(*dimensione) && heap[sx].distanza<heap[indice].distanza){
        //il più piccolo diventa quello a sinistra
        minimo=sx;
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
        struct PercorsoNode tmp=heap[minimo];

        //swapping
        heap[minimo]=heap[indice];
        heap[indice]=tmp;

        //ricorsione per sistemare tutto lo Heap
        minHeapify(heap, dimensione, minimo);
    }
}

/**
 * Funzione di supporto per riorganizzare lo Heap dopo un inserimento nello stesso.
 *
 * @param heap lo Heap che deve essere sistemato.
 * @param indice indice del nodo da sistemare.
 */
void sistemareInserimentoNelloHeap(struct PercorsoNode heap[], int indice){
    /**
     * Genitore dell'elemento dell'indice.
     */
    int indiceGenitore=(indice-1)/2;
    /**
     * Temporaneo per salvare il nodo.
     */
    struct PercorsoNode tmp;


    //se il genitore è più grande del figlio
    if(heap[indiceGenitore].distanza > heap[indice].distanza){
        //salviamo il valore del genitore
        tmp=heap[indiceGenitore];
        heap[indiceGenitore]=heap[indice];
        heap[indice]=tmp;

        //facciamo la stessa cosa per tutti i nodi nello Heap
        sistemareInserimentoNelloHeap(heap, indiceGenitore);
    }
}

/**
 * Inserisce nello Heap un nuovo nodo.
 *
 * @param heap lo Heap a cui va aggiunto il nuovo nodo.
 * @param dimensione la dimensione dello Heap - # elementi nello Heap.
 * @param lunghezzaArray la lunghezza dell'Array che contiene lo Heap.
 * @param nodoDaAggiungere il nodo da aggiungere allo Heap.
 */
void inserimentoNelloHeap(struct PercorsoNode heap[], unsigned int *dimensione, int *lunghezzaArray, struct PercorsoNode nodoDaAggiungere){
    //se array è completamente pieno --> riallocare array
    if(*lunghezzaArray==*dimensione){
        //aumentare lunghezza array
        *lunghezzaArray=(*lunghezzaArray)+5;
        heap=(struct PercorsoNode *) realloc(heap, (*lunghezzaArray));
    }

    //aggiunta nodo in fondo allo Heap
    heap[(*dimensione)]=nodoDaAggiungere;
    //sistemiamo lo Heap
    sistemareInserimentoNelloHeap(heap, *dimensione);
    //aumentiamo il numero di elementi nello Heap
    ++(*dimensione);
}

/**
 * Restituisce il minimo nodo presente nello Heap - lo elimina anche.
 *
 * @param heap lo Heap in cui va ricercato il nodo minimo.
 * @param dimensione la dimensione dello Heap.
 * @return il nodo minimo presente nello Heap.
 */
struct PercorsoNode estraiMinimoDalloHeap(struct PercorsoNode heap[], unsigned int *dimensione){
    /**
     * Nodo minimo presente nello Heap da ritornare e da eliminare da esso.
     */
    struct PercorsoNode nodoMinimo=heap[0];


    //sostituzione della radice con l'ultimo nodo
    heap[0]=heap[(*dimensione)-1];
    //riduci dimensione dello Heap
    --(*dimensione);

    //sistema lo Heap
    minHeapify(heap, dimensione, 0);

    //ritorna il minimo
    return nodoMinimo;
}



//PIANIFICA PERCORSO
/**
 * Ritorna un Array contenente tutte le stazioni tra quella di partenza e quella di arrivo.
 * Adesso la dimensione è arrivo-partenza+1.
 *
 * @param partenza distanza a cui si trova la stazione di partenza.
 * @param arrivo distanza a cui si trova la stazione di arrivo.
 * @param numeroDiStazioni numero di stazioni tra quella di partenza e quella di arrivo.
 * @return un Array che contiene le stazioni tra quella di partenza e quella di arrivo con l'autonomia massima dell'auto nel loro parco auto.
 */
struct ArrayNodeStazione *tutteLeStazioni(unsigned int partenza, unsigned int arrivo, int *numeroDiStazioni){
    /**
     * Dimensione iniziale array.
     */
    int x=(int) (arrivo-partenza+1);
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


    //percorso Andata
    if(partenza<arrivo){
        //per ogni stazione che ci potrebbe essere tra partenza e arrivo
        for (unsigned int i = partenza; i <= arrivo; ++i) {
            //salviamo la stazione cercata
            ricerca= ricercaStazione(autostrada, i);
            //se la stazione esiste la aggiungiamo alle stazioniIntermedie[]
            if(ricerca!=NULL){
                //se l'Array stazioniIntermedie[] è pieno lo reallochiamo
                if(posizioneArray>x){
                    x=x+7;
                    stazioniIntermedie=realloc(stazioniIntermedie, x*sizeof(struct ArrayNodeStazione));
                }
                //aggiungiamo la stazione
                stazioniIntermedie[ricerca->distanza-partenza].distanza= ricerca->distanza;
                stazioniIntermedie[ricerca->distanza-partenza].autonomiaMassima=ricerca->autonomiaMassima;
                //aumentiamo la prossima posizione libera
                posizioneArray++;
            }
        }
    }
    //percorso Ritorno
    else {
        //per ogni stazione che ci potrebbe essere tra partenza e arrivo
        for (unsigned int i = partenza; i >= arrivo; --i) {
            //salviamo la stazione cercata
            ricerca = ricercaStazione(autostrada, i);
            //se la stazione esiste la aggiungiamo alle stazioniIntermedie[]
            if (ricerca != NULL) {
                //se l'Array stazioniIntermedie[] è pieno lo reallochiamo
                if (posizioneArray > x) {
                    x = x + 7;
                    stazioniIntermedie = realloc(stazioniIntermedie, x * sizeof(struct ArrayNodeStazione));
                }
                //aggiungiamo la stazione
                stazioniIntermedie[ricerca->distanza-partenza].distanza = ricerca->distanza;
                stazioniIntermedie[ricerca->distanza-partenza].autonomiaMassima = ricerca->autonomiaMassima;
                //aumentiamo la prossima posizione libera
                posizioneArray++;
            }
        }
    }

    //numero di stazioni tra quella di partenza e quella d'arrivo
    (*numeroDiStazioni)=posizioneArray;
    //stazioni intermedie tra quella di partenza e quella di arrivo
    return stazioniIntermedie;
}

/**
 * Ritorna la distanza tra 2 stazioni - funzione euristica euclidea.
 *
 * @param a stazione A.
 * @param b stazione B.
 * @return la distanza tra la stazione A e B.
 */
double distanzaEuclideaEuristica(unsigned int a, unsigned int b){
    return abs((int) (b-a));
}

//TODO controllare indici degli array
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
int aStarInAvanti(struct ArrayNodeStazione stazioni[], int numeroStazioni, int partenza, int arrivo, int *percorso){
    /**
     * Insieme dei nodi visitati.
     */
    int *visitati=(int *) calloc((int) (arrivo-partenza+1), sizeof(int));
    /**
     * Nodo di partenza.
     */
     //TODO indice stazioni
    struct PercorsoNode nodoPartenza={partenza, 0.0, distanzaEuclideaEuristica(stazioni[partenza-partenza].distanza, stazioni[arrivo-partenza].distanza), 0.0, -1};
    /**
     * Lunghezza Heap.
     */
    int lunghezzaHeap=numeroStazioni;
    /**
     * Heap dove verranno salvati i nodi da esaminare.
     */
    struct PercorsoNode *heapPrioritario=(struct PercorsoNode *) calloc(lunghezzaHeap, sizeof(struct PercorsoNode));
    /**
     * Dimensione dello Heap - # elementi nello Heap.
     */
    unsigned int dimensioneHeap=0;
    /**
     * Lunghezza del percorso minimo.
     */
    int indicePercorso=0;
    /**
     * Distanza tra 2 nodi.
     */
    double distanza=0;
    /**
     * Nodo corrente da controllare nello heap.
     */
    struct PercorsoNode corrente={0, 0.0, 0.0, 0.0, -1};


    //inserimento del nodo di partenza nello heap
    inserimentoNelloHeap(heapPrioritario, &dimensioneHeap, &lunghezzaHeap, nodoPartenza);

    while (dimensioneHeap>0){
        //nodo con il valore minimo nello Heap
        corrente= estraiMinimoDalloHeap(heapPrioritario, &dimensioneHeap);

        //verifica se siamo arrivati a destinazione
        if(corrente.distanza==arrivo){
            indicePercorso=0;
            //costruiamo il percorso attraverso i nodi genitori
            while (corrente.genitore!=-1){
                //aggiungiamo il nodo corrente al percorso
                percorso[indicePercorso]=(int) corrente.distanza;
                corrente=heapPrioritario[corrente.genitore];
                indicePercorso++;
            }
            percorso[indicePercorso]=(int) corrente.distanza;

            free(heapPrioritario);
            //ritorna il numero di tappe
            return indicePercorso; //TODO forse stampato al contrario
        }

        //aggiungo nodo corrente all'Array dei nodi visitati
        visitati[corrente.distanza-partenza]=1;

        //esamino i vicini del nodo corrente
        for (int i = 0; i < (arrivo-partenza+1); ++i) {
            //nodo deve essere stato visitato AND NON è quello che stiamo controllando adesso
            if(!visitati[i] && i!=corrente.distanza-partenza){
                if(stazioni[i].distanza!=0) {
                    //calcolo della distanza tra 2 nodi - nodo corrente e nodo vicino
                    distanza = distanzaEuclideaEuristica(stazioni[corrente.distanza - partenza].distanza,
                                                         stazioni[i].distanza); //TODO indice

                    //controllo se auto può raggiungere stazione successiva
                    if (distanza <= corrente.g + stazioni[corrente.distanza - partenza].autonomiaMassima) {
                        //creo nuovo nodo per il successivo
                        /**
                         * Nodo che rappresenta il successivo rispetto a quello controllato adesso.
                         */
                        struct PercorsoNode successivo;
                        successivo.distanza = stazioni[i].distanza;
                        successivo.g = corrente.g + distanza;
                        successivo.h = distanzaEuclideaEuristica(stazioni[i].distanza,
                                                                 stazioni[arrivo - partenza].distanza);
                        successivo.f = successivo.g + successivo.h;
                        successivo.genitore = (int) (corrente.distanza - partenza);

                        //inserisci il successivo nello Heap
                        inserimentoNelloHeap(heapPrioritario, &dimensioneHeap, &lunghezzaHeap, successivo);
                    }
                }
            }
        }
    }

    free(heapPrioritario);
    //nessun percorso trovato
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
int aStarAllIndietro(struct ArrayNodeStazione stazioni[], int numeroStazioni, int partenza, int arrivo, int *percorso){
    //TODO da fare --> dovrebbe essere simile all'andata cambiando solo alcuni controlli - forse, spero ＞﹏＜

    //nessun percorso trovato
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
int aStar(struct ArrayNodeStazione stazioni[], int numeroStazioni, int partenza, int arrivo, int *percorso){
    //calcolare il percorso all'andata
    if(partenza<arrivo){
        return aStarInAvanti(stazioni, numeroStazioni, partenza, arrivo, percorso);
    }
    //calcolare il percorso al ritorno
    else{
        return aStarAllIndietro(stazioni, numeroStazioni, partenza, arrivo, percorso);
    }
}

/**
 * Stampa il percorso calcolato.
 *
 * @param numeroDiStazioni numero di stazioni toccate.
 * @param percorso percorso dalla stazione di partenza a quella di arrivo.
 */
void stampaPercorso(int numeroDiStazioni, int *percorso){
    for (int i = 0; i < numeroDiStazioni; ++i) {
        printf("%d ", percorso[i]);
    }
    printf("\n");
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


    //lettura distanza della stazione da aggiungere
    scanf("%d", &distanza);

    //stazione già presente
    if(StazioneGiaPresente(autostrada, distanza)==1){
        //la stazione NON è stata aggiunta
        printf("non aggiunta\n");

        //devo comunque leggere le auto inserite in input altrimenti restano nel buffer
        scanf("%d", &numeroAuto);
        for (int i = 0; i < numeroAuto; ++i) {
            scanf("%d", &autonomiaAuto);
        }
    }
    //stazione NON presente
    else{
        //leggi numero di macchine che bisogna aggiungere alla stazione
        scanf("%d", &numeroAuto);

        //crea il parco auto
        parcoAuto= creaParcoAuto(DIMENSIONE_INIZIALE_PARCO_AUTO);

        //aggiungi auto con la loro chiave al parco auto
        for (int i = 0; i < numeroAuto; ++i) {
            //leggi chiave auto da aggiungere al parco auto
            scanf("%d", &autonomiaAuto);

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


    //lettura distanzaStazione della stazione a cui aggiungere una macchina
    scanf("%d", &distanzaStazione);

    //stazione NON presente
    if(StazioneGiaPresente(autostrada, distanzaStazione)==0){
        //auto NON aggiunta
        printf("non aggiunta\n");

        //devo comunque leggere l'auto inserita in input altrimenti resta nel buffer
        scanf("%d", &autonomiaAutoDaAggiungere);
    }
    //stazione presente
    else{
        /**
         * Stazione di interesse.
         */
        struct HashNodeStazione *stazione= ricercaStazione(autostrada, distanzaStazione);

        //leggi chiave auto da aggiungere
        scanf("%d", &autonomiaAutoDaAggiungere);

        //distanza massima percorribile da quella stazione
        if(stazione->autonomiaMassima<autonomiaAutoDaAggiungere){
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

    //lettura distanza della stazione da demolire
    scanf("%d", &distanza);

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


    //lettura distanzaStazione della stazione da cui rottamare una macchina
    scanf("%d", &distanzaStazione);

    //Se stazione NON presente
    if(StazioneGiaPresente(autostrada, distanzaStazione)==0){
        //auto NON rottamata
        printf("non rottamata\n");

        //devo comunque leggere le auto inserite in input altrimenti restano nel buffer
        scanf("%d", &autonomiaAutoDaRottamare);
    }
    //stazione presente
    else{
        /**
         * Stazione di interesse.
         */
        struct HashNodeStazione *stazione= ricercaStazione(autostrada, distanzaStazione);

        //leggi chiave auto da rottamare
        scanf("%d", &autonomiaAutoDaRottamare);

        //Se auto NON presente nel parco auto della stazione
        if(ricercaAuto(stazione->parcoAuto, autonomiaAutoDaRottamare)==0){
            //auto NON rottamata
            printf("non rottamata\n");
        }
        //auto presente nel parco auto della stazione
        else{
            //salviamo la nuova autonomia massima
            stazione->autonomiaMassima= ricercaMassimaAutonomia(stazione->parcoAuto, stazione->parcoAuto->capacita);

            //eliminiamo l'auto dal parco auto della stazione
            eliminaAuto(stazione->parcoAuto, autonomiaAutoDaRottamare);
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

    //lettura distanza delle stazioni di partenza e di arrivo
    scanf("%d", &distanzaStazionePartenza);
    scanf("%d", &distanzaStazioneArrivo);

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
        int *percorso=(int *) calloc(numeroDiStazioni, sizeof(int));
        /**
         * Numero di fermate che bisogna compiere nel percorso.
         */
        int numeroFermate= aStar(stazioniIntermedie, numeroDiStazioni, distanzaStazionePartenza, distanzaStazioneArrivo, percorso);

        //se NON esiste il percorso
        if(numeroFermate==0){
            printf("nessun percorso\n");
        }
        //stampare il percorso
        else{
            stampaPercorso(numeroFermate, percorso);
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





/*
 * tutteLeStazioni --> per ridurre memoria si potrebbe fare realloc alla fine prima di restituire l'Array
 *
 * aStar --> controllare tutti gli indici degli array
 */