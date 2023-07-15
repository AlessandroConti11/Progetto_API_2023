#include <stdio.h>

/**
 * Define --> Lunghezza massima che un comando può assumere.
 */
#define LUNGHEZZA_MAX_COMANDI 20










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







    //TODO da togliere
    printf("Hello, World!\n");

    return 0;

}


