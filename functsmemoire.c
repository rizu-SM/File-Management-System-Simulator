#include "functsmemoire.h"

#include "declarations.h"

#include "functsFichierMeta.h"

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <math.h>

#include <string.h>



void InitialiseDisk(MS* MemoireS){/* Cette fonction est utilisée lorsqu'on réinitialise la mémoire secondaire, et après avoir effectué
                                les opérations nécessaires, on sauvegarde l'état et les données de la mémoire secondaire dans un fichier binaire.
                                Si le fichier binaire de la mémoire secondaire existe, on n'utilise pas cette fonction ; on lit simplement
                                le fichier binaire et alloue un espace mémoire pour la mémoire secrondaire sauvegardée dans le fichier.
                                */

    MemoireS->bloc_size=FB;

    MemoireS->nbrBlocs = NbrBlocs;

    MemoireS->TailleMemoire = (NbrBlocs - ceil( (NbrBlocs - 1)/(FB*nbrMeta_enreg) ) ) * FB * data_size; //taille de memoire secondaire VALABLE POUR UTILISER en bytes ( 1 character = 1 byte)

    MemoireS->memoireS = fopen("memoire_secondaire.bin","wb+");

    if(MemoireS->memoireS == NULL){
        printf("\nErreur lors l'initialisation du disk.");
        return;
    }
    

    int nbr_MetaBlocs = ceil( (NbrBlocs - 1) / ( FB * nbrMeta_enreg ) );/* Le nombre maximal de blocs de métadonnées pour chaque fichier est une majoration du nombre maximal de blocs
    qui peuvent être occupés par un fichier, divisé par le facteur de blocage. On divise par le facteur de blocage
    car chaque enregistrement d'un bloc contient des métadonnées d'un fichier. */


//initialisation de la table d'allocation dans Bloc 0 :

    Bloc bloc0;

    bloc0.nbrE = 0;

int i,j=0,k=0,nbrBlocs_Libre=0;

    for(i=0; i<NbrBlocs; i++){

        if(j >= data_size){ /*cette condition est utilisé si par exemple on change le nombres de Blocs définé dans la Memoire Secrondaire
                            tellque il devient plus que le data_size, dans ce cas il faut que a chaque fois le tableau de charactéres (data)
                            de l'enregistrement k est plein, on va vers le prochaine tableau de charactéres dans l'enregistrement k+1 et continuée
                            
                            NOTE : le tableau de charachtéres (data) dans le Bloc 0 est représenté come une table d'allocation!*/
            k++;

            j=0;
        }

        if(i <= nbr_MetaBlocs){/*
            on initialise la table d'allocation tellque data[0] = 1 represente que le Bloc 0 est occupé,
            data[1] = 1 represente que le Bloc 1 est Occupé etc...,data[nbr_MetaBlocs +1] = 0 represente
            que Le Bloc nbr_MetaBlocs + 1 est libre...etc jusqu'a data[NbrBlocs].
            (MAIS PAS EXCATEMENT A data[NbrBlocs] Car le tableau d'enregistrement 0
            contient 140 charachtéres et donc on va vers prochaine enregistrement!)
            */
        bloc0.enregistrements[k].data[j] = 1;



        }else{
        
        bloc0.enregistrements[k].data[j] = 0;

        nbrBlocs_Libre++;

        }
        j++;
    }

    bloc0.nbrE=k+1; //nombres d'enregistrements occupé qui contient le contenu de la table d'allocation dans Bloc 0.

    bloc0.nextBloc = nbrBlocs_Libre; /* comme Bloc 0 est utilisé pour sauvgarder la table d'allocation de la Memoire Secrondaire,
                                                  la variable NextBloc ne sera pas étres utilisé, de plus le Bloc 0 ne sera pas étres concerné
                                                  par les opérations come le compactage ou défragmentation, c'est un Bloc presque isolé, et alors 
                                                  on a utilisé NextBloc come le nombres de blocs libres dans la table d'allocation.
                                                  */
    
    fwrite(&bloc0,sizeof(Bloc),1,MemoireS->memoireS); //sauvgarder le Bloc 0 dans la memoire secondaire aprés l'initialisation de la table d'allocation.


    //initialisation des autres blocs (de Bloc 1 vers Bloc 255):

    Bloc bloc;

    bloc.nbrE = 0;

    bloc.nextBloc =-1; /* le -1 représente que le next bloc n'existe pas.*/

    for(i=0; i<FB; i++){ /*initialiser les tableau de charactéres de chaque enregistrements par null '/0' */
    
    memset(bloc.enregistrements[i].data, 0, sizeof(bloc.enregistrements[i].data));

    bloc.enregistrements[i].isDeleted = false;

    }


    for(i=1; i<NbrBlocs; i++){
    
    fwrite(&bloc,sizeof(bloc),1,MemoireS->memoireS);

    }
    
    fclose(MemoireS->memoireS);
    printf("\nInitialisation du disk avec succes.");

}

void vider_MS(MS *ms) {
    // Initialisation d'un bloc vide
    Bloc blocVide = {0}; // Tous les champs sont initialisés à 0
    Bloc buffer;

    int i;
    // Réinitialisation de tous les blocs dans la mémoire secondaire
    for (i = 1; i < ms->nbrBlocs; i++) {
        fseek(ms->memoireS, i * sizeof(Bloc), SEEK_SET);
        fwrite(&blocVide, sizeof(Bloc), 1, ms->memoireS);

        // Marquer tous les blocs comme libres dans le tableau d'allocation

        rewind(ms->memoireS);
        fread(&buffer, sizeof(Bloc), 1, ms->memoireS);

        if (i < 140)
        {
            buffer.enregistrements[0].data[i] == 0;
        }else
        {
            buffer.enregistrements[1].data[i-140] == 0;
        }
    }

    // Réinitialisation des blocs réservés
    InitialiseDisk(ms);

    printf("\nMemoire secondaire videe avec succes.\n");
}

bool GES_Creation(MetaD *fichierinfo,Bloc *Bloc0){


if(Bloc0->nextBloc >= fichierinfo->taille_Blocs){

    int i=0,j=0,k=0;

    if(fichierinfo->OrganisationE == true){ //si l'Organisation est contigue

        int counter=0;

        while(i<NbrBlocs && counter != fichierinfo->taille_Blocs){

            if(j>data_size){
                k++;
                j=0;
            }
        
        if(Bloc0->enregistrements[k].data[j] == 0){

            counter++;

        }else{//on chereche n blocs contigue dans la memoire secrondaire ou n = nbrBlocs

            counter=0;

        }

        j++;

        i++;
            
        }
        if(counter == fichierinfo->taille_Blocs){

            if(k == 0){

            fichierinfo->adresse_PremierBloc = j - counter; //le index est l'adresse du Bloc de départ du fichier qui sera étre allouer dans la mémoire secrondaire.

            }else{

            fichierinfo->adresse_PremierBloc = k *(data_size - 1) + j + 1 - counter;
            }

            return true;

        }else{

            //pas d'espace contigue ---> compactage , pour le moment on fait return false.

            fichierinfo->adresse_PremierBloc = -1;

            return false;
        }


    }else{
            //Organisation Chainé, dans cette organisation il faut juste de trouver le premier bloc libre.

            bool trouver = false;

            while(i<NbrBlocs && trouver == false){

            if(j>data_size){

                k++;

                j=0;

            }

            if(Bloc0->enregistrements[k].data[j] == 0){

                trouver = true;

                if(k == 0){

                    fichierinfo->adresse_PremierBloc = j;

                }else{
                
                fichierinfo->adresse_PremierBloc = k*(data_size - 1) + j + 1;

                }
            }
            
            j++;
            i++;
    }
    
        return trouver; // pas de compactage si on peut pas trouver un Bloc libre dans l'organisation Chainé.
    }

}else{

    //nombres de bloc libres insuffisant pour ajouter le fichier (dans les deux modes!) ---> pas d'espace memoire!.

    printf("\nLa Memoire Secrondaire est pleine!");

    fichierinfo->adresse_PremierBloc = -1;

    return false;
}

}

Bloc LireBloc0(MS MemoireS){

Bloc bloc0;

rewind(MemoireS.memoireS);

fread(&bloc0,sizeof(Bloc),1,MemoireS.memoireS);

return bloc0;

}


