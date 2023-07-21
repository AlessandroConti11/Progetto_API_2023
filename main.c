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
//TODO per l'autostrada --> vanno create le stazioni che contengono distanza e parco auto



//VARIABILI GLOBALI
/**
 * Autostrada.
 */
struct HashTable *autostrada=NULL;



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
struct HashTable *creaParcoAuto(unsigned int capacitaIniziale){
    return inizializzazioneHashTable(capacitaIniziale);
}

/**
 * Aggiungi auto al parco auto richiesto.
 *
 * @param parcoAuto il parco auto richiesto.
 * @param autonomia l'autonomia dell'auto da aggiungere al parco auto.
 */
void aggiungiAuto(struct HashTable *parcoAuto, unsigned int autonomia){
    //inserimento dell'auto nel parco auto
    inserimentoNellaHashTable(parcoAuto, autonomia);
}



//GESTIONE STAZIONE
/**
 * Ritorna 1 se la stazione esiste sull'autostrada.
 *
 * @param distanza distanza in cui si trova la stazione di interesse.
 * @return 1 se la stazione esiste sull'autostrada, 0 altrimenti
 */
int StazioneGiaPresente(unsigned int distanza){
    //TODO da fare
    if(distanza==1)
        return 1;
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
    struct HashTable *parcoAuto=NULL;


    //lettura distanza della stazione da aggiungere
    scanf("%d", &distanza);

    //stazione già presente
    if(StazioneGiaPresente(distanza)==1){
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
        //TODO aggiungi stazione all'autostrada
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
    int distanzaStazione=0;
    /**
     * Autonomia dell'auto da aggiungere alla stazione
     */
    int autonomiaAutoDaAggiungere=0;


    //lettura distanzaStazione della stazione a cui aggiungere una macchina
    scanf("%d", &distanzaStazione);

    //stazione NON presente
    if(StazioneGiaPresente(distanzaStazione)==0){
        printf("non aggiunta");
    }
    //stazione presente
    else{
        //leggi chiave auto da aggiungere
        scanf("%d", &autonomiaAutoDaAggiungere);

        //TODO aggiungi auto al praco auto della stazione
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
    int distanza=0;

    //lettura distanza della stazione da demolire
    scanf("%d", &distanza);

    //Se stazione NON presente
    if(StazioneGiaPresente(distanza)==0){
        printf("non demolita");
    }
    //stazione presente
    else{
        //TODO demolisci stazione
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
    int distanzaStazione=0;
    /**
     * Autonomia dell'auto da rottamare dalla stazione
     */
    int autonomiaAutoDaRottamare=0;

    //lettura distanzaStazione della stazione da cui rottamare una macchina
    scanf("%d", &distanzaStazione);

    //Se stazione NON presente
    if(StazioneGiaPresente(distanzaStazione)==0){
        printf("non rottamata");
    }
    //stazione presente
    else{
        //leggi chiave auto da rottamare
        scanf("%d", &autonomiaAutoDaRottamare);

        //TODO if
        //Se auto NON presente nel parco auto della stazione
            //printf("non rottamata")
        //altrimenti
            //rottama macchina
            //printf("rottamata")
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
    if(StazioneGiaPresente(distanzaStazionePartenza)==0){
        printf("nessun percorso");
    }
    //se stazione arrivo NON esiste
    else if(StazioneGiaPresente(distanzaStazioneArrivo)==0){
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
    autostrada= inizializzazioneHashTable(DIMENSIONE_INIZIALE_AUTOSTRADA);

    //continua a leggere fino a che non leggi EOF
    do{
        //leggi i comandi che arrivano da stIN
        fine=scanf("%s", comando);

        //processa il comando letto
        ProcessaComando(comando);
    }while(fine!=EOF); //se leggi EOF termina

    return 0;

}
