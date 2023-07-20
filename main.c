#include <stdio.h>
#include <stdlib.h>



/**
 * Define --> Lunghezza massima che un comando può assumere.
 */
#define LUNGHEZZA_MAX_COMANDI 20

/**
 * Define --> Dimensione iniziale della HashTable.
 */
#define DIMENSIONE_INIZIALE_PARCO_AUTO 11



/**
 * Struct --> Rappresenta un nodo di una HashTable.
 */
struct HashNode{
    /**
     * Chiave che contiene l'autonomia della macchina.
     */
    int autonomia;
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
    int dimensione;
    /**
     * Capacità massima della HastTable, poi andra reallocata.
     */
    int capacita;
};
//TODO sistemare la realloc - capacità



//GESTIONE HASH
/**
 * Ritorna la posizione in cui si trova la macchina.
 *
 * @param autonomia chiave che rappresenta l'autonomia che ha la macchina.
 * @return la posizione in cui si trova la macchina.
 */
int parcoAutoHash(int autonomia){
    //TODO algoritmo di hash decente
    return 1;
}

/**
 * Gestione della realloc, dopo che si è inserito troppe auto.
 *
 * @param hashTable la HashTable da reallocare
 */
void parcoAutoReHash(struct HashTable *hashTable){
    //TODO nuova capacità
    /**
     * Nuova capacità che avrà la nuova HashTable.
     */
    int nuovaCapacita=hashTable->capacita *2+11;
    /**
     * Nuova HashTable.
     */
     struct HashNode **nuovaTable=(struct HashNode **) calloc(nuovaCapacita, sizeof(struct HashNode));

     //rialloca le cose già presenti nella vecchia HashTable
    for (int i = 0; i < hashTable->capacita; ++i) {
        /**
         * Nuovo nodo da inserire nella nuova HashTable.
         */
        struct HashNode *corrente=hashTable->table[i];
        while (corrente->successivo!=NULL){
            /**
             * Nuovo nodo successivo.
             */
            struct HashNode *successivo=corrente->successivo;
            /**
             * Nuovo indice.
             */
            int nuovoIndice= parcoAutoHash(corrente->autonomia);
        }
    }
}

/**
 * Inserisci una macchina con la sua autonomia nel parco auto.
 *
 * @param hashTable il parco auto a cui si vuole aggiungere la stazione.
 * @param autonomia l'autonomia della macchina da aggiungere.
 */
void inserisciNelParcoAuto(struct HashTable *hashTable, int autonomia){
    /**
     * Indice in cui si dovrà aggiungere la macchina.
     */
    int indice= parcoAutoHash(autonomia);
    /**
     * Creazione nodo da aggiungere alla HashTable.
     */
    struct HashNode *nuovaMacchina=(struct HashNode *) malloc(sizeof(struct HashNode));
    nuovaMacchina->autonomia=autonomia; //salvataggio autonomia
    nuovaMacchina->successivo=hashTable->table[indice]; //salvataggio nodo successivo nella stessa posizione della HashTable

    //aggiunta nodo alla HashTable
    hashTable->table[indice]=nuovaMacchina;
}

/**
 * Ricerca di una macchina nella HashTable.
 *
 * @param hashTable il parco auto in cui si vuole cercare la macchina.
 * @param autonomia l'autonomia della macchina da cercare.
 * @return la macchina cercata oppure NULL.
 */
struct HashNode *cercaAuto(struct HashTable *hashTable, int autonomia){
    /**
     * Indice in sui si trova la macchina richiesta.
     */
    int indice= parcoAutoHash(autonomia);
    /**
     * Nodo corrente.
     */
    struct HashNode *corrente=hashTable->table[indice];

    //fino a che non ci sono più macchine nel nodo
    while(corrente!=NULL){
        //abbiamo trovato la macchina?
        if(corrente->autonomia==autonomia){
            return corrente;
        }
        //guardiamo la macchina successiva nella lista
        else{
            corrente=corrente->successivo;
        }
    }

    //macchina NON trovata
    return NULL;
}

void eliminaAuto(struct HashTable *hashTable, int autonomia){
    /**
     * Indice in sui si trova la macchina richiesta.
     */
    int indice= parcoAutoHash(autonomia);
    /**
     * Nodo corrente.
     */
    struct HashNode *corrente=hashTable->table[indice];
    /**
     * Nodo precedente rispetto a quello corrente.
     */
    struct HashNode *precedente=NULL;

    //ricerca nodo con la chiave corrispondente
    while (corrente!=NULL && corrente->autonomia!=autonomia){
        precedente=corrente;
        corrente=corrente->successivo;
    }

    //nodo NON è stato trovato
    if(corrente==NULL){
        //nodo NON è stato trovato
        return;
    }
    //nodo è stato trovato
    else{
        //il nodo da eliminare è il primo
        if(precedente==NULL){
            hashTable->table[indice]=corrente->successivo;
        }
        //il nodo da eliminare si trova in mezzo alla lista
        else{
            precedente->successivo=corrente->successivo;
        }
    }

    //dealloco il nodo
    free(corrente);

    //riduco numero di elementi nella HashTable

}




/**
 * Ritorna 1 se la stazione esiste sull'autostrada.
 *
 * @param distanza distanza in cui si trova la stazione di interesse.
 * @return 1 se la stazione esiste sull'autostrada, 0 altrimenti
 */
int StazioneGiaPresente(int distanza){
    //TODO da fare
    if(distanza==1)
        return 1;
    return 0;
}







/**
 * Aggiungi stazione all'autostrada.
 */
void AggiungiStazione(){
    /**
     * Distanza dall'origine  della stazione
     */
    int distanza=0;
    /**
     * Numero di auto da aggiungere alla stazione
     */
    int numeroAuto=0;
    /**
     * Autonomia dell'auto da aggiungere alla stazione
     */
    int autonomiaAuto=0;


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

        //TODO crea parco auto di dimensione numeroAuto

        //aggiungi auto con la loro autonomia al parco auto
        for (int i = 0; i < numeroAuto; ++i) {
            //leggi autonomia auto da aggiungere al parco auto
            scanf("%d", &autonomiaAuto);
            //TODO aggiungi autonomiaAuto al parco auto sopra creato
        }
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
        //leggi autonomia auto da aggiungere
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
        //leggi autonomia auto da rottamare
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



int main() {

    /**
     * Carattere per capire non abbiamo più niente da leggere da stIN.
     */
    int fine;

    /**
     * Stringa che rappresenta il comando che bisogna eseguire.
     */
    char comando[LUNGHEZZA_MAX_COMANDI]="";


    //continua a leggere fino a che non leggi EOF
    do{
        //leggi i comandi che arrivano da stIN
        fine=scanf("%s", comando);

        //processa il comando letto
        ProcessaComando(comando);



    }while(fine!=EOF); //se leggi EOF termina

    return 0;

}


