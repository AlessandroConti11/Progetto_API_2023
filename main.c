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
 * Struct --> Rappresenta un nodo di una HashTable.
 */
struct HashNode{
    /**
     * Chiave.
     */
    unsigned int chiave;
    /**
     * Nodo successivo nella stessa posizione della HashTable.
     */
    struct HashNode *successivo;
};

/**
 * Struct --> Rappresenta la HashTable.
 */
struct HashTable{
    /**
     * Tabella di Hash.
     */
    struct HashNode **table;
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

/**
 * Inizializzazione della HashTable.
 *
 * @param capacita la dimensione iniziale della HashTable.
 * @return la HasTable inizializzata.
 */
struct HashTable *inizializzazioneHashTable(unsigned int capacita){
    /**
     * Nuova HashTable.
     */
    struct HashTable *hashTable=(struct HashTable *) malloc(sizeof(struct HashTable));

    //inizializza valori
    hashTable->dimensione=0;
    hashTable->capacita=capacita;
    hashTable->table=(struct HashNode **) calloc(hashTable->capacita, sizeof(struct HashNode *));

    //ritorna la HashTable inizializzata
    return hashTable;
}

/**
 * Reinizializza la HashTable --> da una HashTable già esistente la realloca in un'altra con capacità maggiore
 *
 * @param hashTable la HashTable da reinizializzare.
 */
void reHashTable(struct HashTable *hashTable){
    /**
     * Nuova capacità della HashTable.
     */
    unsigned int capacitaNuova= nuovaCapacita(hashTable->capacita);
    /**
     * Nuovo indice in cui spostare gli HashNode dalla HashTable vecchia a quella nuova.
     */
    unsigned int nuovoIndice=0;
    /**
     * Nuova HashTable.
     */
    struct HashNode **nuovaHashTable=(struct HashNode **) calloc(capacitaNuova, sizeof(struct HashNode *));
    /**
    * Nodo della HashTable corrente.
    */
    struct HashNode *corrente=NULL;
    /**
     * Nodo successivo da vedere.
     */
    struct HashNode *prossimo=NULL;


    //spostiamo dalla vecchia HashTable a quella nuova
    for (int i = 0; i < hashTable->capacita; ++i) {
        corrente=hashTable->table[i];

        //fino a che NON abbiamo finito di spostare tutte le HashNode
        while(corrente!=NULL){
            //salviamo il prossimo HashNode da visitare
            prossimo=corrente->successivo;
            //salviamo il nuovo indice in cui andare a mettere la HashNode corrente
            nuovoIndice= funzioneDiHash(corrente->chiave, capacitaNuova);

            //spostiamo il nodo
            corrente->successivo=nuovaHashTable[nuovoIndice];
            nuovaHashTable[nuovoIndice]=corrente;
            //il nodo corrente da vedere diventa il prossimo
            corrente=prossimo;
        }
    }

    //eliminiamo la vecchia table
    free(hashTable->table);
    //aggiungiamo la nuova table
    hashTable->table=nuovaHashTable;
    //aggiorniamo la capacità
    hashTable->capacita=capacitaNuova;
}

/**
 * Inserimento in una HashTable di un HashNode.
 *
 * @param hashTable la HashTable alla quale va aggiunto il nodo.
 * @param chiave la chiave da aggiungere alla HashTable.
 */
void inserimentoNellaHashTable(struct HashTable *hashTable, unsigned int chiave){
    /**
     * Indice in cui inserire la chiave.
     */
    unsigned int indice= funzioneDiHash(chiave, hashTable->capacita);
    /**
     * Nuovo HashNode da inserire nella HashTable.
     */
    struct HashNode *nuovoNodo=(struct HashNode *) malloc(sizeof(struct HashNode));


    //inserimento valori nel nuovo HashNode
    nuovoNodo->chiave=chiave;
    nuovoNodo->successivo=hashTable->table[indice];
    hashTable->table[indice]=nuovoNodo;
    //è stato aggiunto un nuovo nodo, questo implica che bisogna aumentare la dimensione della HashTable
    hashTable->dimensione++;

    //bisogna reallocare la HashTable
    if(hashTable->dimensione > hashTable->capacita*PERCENTUALE_REALLOC){
        reHashTable(hashTable);
    }
}

/**
 * Ricerca di una chiave nella HashTable.
 *
 * @param hashTable la HashTable nella quale dobbiamo eseguire la ricerca.
 * @param chiave la chiave da cercare.
 * @return il HashNode che contiene la chiave di interesse
 */
struct HashNode *ricercaHashNode(struct HashTable *hashTable, unsigned int chiave){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(chiave, hashTable->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNode *corrente=hashTable->table[indice];

    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //abbiamo trovato la chiave
        if(corrente->chiave==chiave){
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
 * Ricerca esistenza chiave nella HashTable.
 *
 * @param hashTable la HashTable nella quale dobbiamo eseguire la ricerca.
 * @param chiave la chiave da cercare.
 * @return 1 se la chiave è presente, 0 se NON è presente.
 */
unsigned int esisteChiaveNellaHashTable(struct HashTable *hashTable, unsigned int chiave){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(chiave, hashTable->capacita);
    /**
     * HashNode corrente.
     */
    struct HashNode *corrente=hashTable->table[indice];


    //fino a che ci sono altri HashNode nella posizione richiesta
    while(corrente!=NULL){
        //abbiamo trovato la chiave
        if(corrente->chiave==chiave){
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
 * Elimina la chiave dalla HashTable.
 *
 * @param hashTable la HashTable nella quale dobbiamo eliminare una chiave.
 * @param chiave la chiave da eliminare.
 */
void eliminaHashNode(struct HashTable *hashTable, unsigned int chiave){
    /**
     * Indice nel quale si trova la chiave.
     */
    unsigned int indice= funzioneDiHash(chiave, hashTable->capacita);
    /**
     * Nodo corrente,
     * corrente=corrente->successivo.
     */
    struct HashNode *corrente=hashTable->table[indice];
    /**
     * Nodo precedente,
     * precedente=corrente.
     */
    struct HashNode *precedente=NULL;


    //trova il nodo con la chiave corrispondente, assegna corrente AND precedente
    while (corrente!=NULL && corrente->chiave!=chiave){
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
            hashTable->table[indice]=corrente->successivo;
        }
        //il nodo da eliminare NON è il primo della lista presente nella posizione indicate
        else{
            precedente->successivo=corrente->successivo;
        }
    }

    //dealloca il HashNode
    free(corrente);
    //riduciamo il numero di elementi presenti nella HashTable
    hashTable->dimensione--;
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
void aggiungiStazione(struct HashTableAutostrada *htAutostrada, unsigned int distanza, struct HashTableParcoAuto *parcoAuto){
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


    //lettura distanza della stazione da aggiungere
    scanf("%d", &distanza);

    //stazione già presente
    if(StazioneGiaPresente(autostrada, distanza)==1){
        //la stazione NON è stata aggiunta
        printf("non aggiunta");
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
            //aggiungi autonomiaAuto al parco auto sopra creato
            aggiungiAuto(parcoAuto, autonomiaAuto);
        }
        //aggiunta la stazione
        aggiungiStazione(autostrada, distanza, parcoAuto);

        //la stazione è stata aggiunta
        printf("aggiunta");
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
        printf("non aggiunta");
    }
    //stazione presente
    else{
        //leggi chiave auto da aggiungere
        scanf("%d", &autonomiaAutoDaAggiungere);

        //aggiungi auto al praco auto della stazione
        aggiungiAuto(ricercaStazione(autostrada, distanzaStazione)->parcoAuto, autonomiaAutoDaAggiungere);

        //auto aggiunta la parco auto
        printf("aggiunta");
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
        printf("non demolita");
    }
    //stazione presente
    else{
        //demolisci stazione
        eliminaStazione(autostrada, distanza);

        //stazione demolita
        printf("demolita");
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
     * Stazione di interesse.
     */
    struct HashNodeStazione *stazione= ricercaStazione(autostrada, distanzaStazione);

    //lettura distanzaStazione della stazione da cui rottamare una macchina
    scanf("%d", &distanzaStazione);

    //Se stazione NON presente
    if(StazioneGiaPresente(autostrada, distanzaStazione)==0){
        //auto NON rottamata
        printf("non rottamata");
    }
    //stazione presente
    else{
        //leggi chiave auto da rottamare
        scanf("%d", &autonomiaAutoDaRottamare);

        //TODO if
        //Se auto NON presente nel parco auto della stazione
        if(ricercaAuto(stazione->parcoAuto, autonomiaAutoDaRottamare)==0){
            //auto NON rottamata
            printf("non rottamata");
        }
        //auto presente nel parco auto della stazione
        else{
            eliminaAuto(stazione->parcoAuto, autonomiaAutoDaRottamare);
            //auto rottamata
            printf("rottamata");
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

    //Se stazione partenza NON esiste
    if(StazioneGiaPresente(autostrada, distanzaStazionePartenza)==0){
        printf("nessun percorso");
    }
    //se stazione arrivo NON esiste
    else if(StazioneGiaPresente(autostrada, distanzaStazioneArrivo)==0){
        printf("nessun percorso");
    }
    //se stazione partenza == stazione arrivo
    else if(distanzaStazionePartenza==distanzaStazioneArrivo){
        printf("%d", distanzaStazionePartenza);
    }
    //ricerca il percorso ottimale per andare dalla partenza all'arrivo
    else{
        //TODO pianifica percorso
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
