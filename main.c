#include <stdio.h>

/**
 * Define --> Lunghezza massima che un comando può assumere.
 */
#define LUNGHEZZA_MAX_COMANDI 20




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

    //TODO if
    //Se stazione già presente:
        //printf("non aggiunta")
    //altrimenti
        //crea stazione
        //leggi numero di macchine che bisogna aggiungere alla stazione
        //leggi autonomia macchine che bisogna aggiungere
        //aggiungi macchina alla stazione
        //printf("aggiunta")
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

    //TODO if
    //Se stazione NON presente:
        //printf("non aggiunta")
    //altrimenti
        //leggi autonomia macchina che bisogna aggiungere
        //aggiungi macchina alla stazione
        //printf("aggiunta")
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

    //TODO if
    //Se stazione NON presente:
        //printf("non demolita")
    //altrimenti
        //demolisci stazione
        //printf("demolita")
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

    //TODO if
    //Se stazione NON presente:
        //printf("non rottamata")
    //altrimenti
        //leggi autonomia macchina che bisogna rottamare
        //Se macchina NON presente:
            //printf("non rottamata")
        //altrimenti
            //rottama macchina
            //printf("rottamata)"
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

    //TODO if
    //Se stazione partenza NON esiste:
        //printf("nessun percorso")
    //Se stazione arrivo NON esiste:
        //printf("nessun percorso")
    //Se stazione partenza == stazione arrivo
        //printf("%d", distanzaStazionePartenza)
    //altrimenti
        //pianificare il percorso
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







    //TODO da togliere
    printf("Hello, World!\n");

    return 0;

}


