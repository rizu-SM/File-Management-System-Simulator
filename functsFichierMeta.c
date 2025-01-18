#include "functsFichierMeta.h"

#include "declarations.h"

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <string.h>

#include <math.h>

MetaD CreeFichier(MS MemoireS){


MetaD fichierinfo;

bool found = false;

int index,adress,niveau;

char nom[50];

    printf("\nLe nom du fichier: ");

    do{

    fgets(fichierinfo.nom,sizeof(fichierinfo.nom),stdin);

    RechercheMetaD(MemoireS,fichierinfo.nom,&found,&index,&adress,&niveau); //vérifier si le nom donné déja existe ou pas dans la memoire secondaire

    if(found == true){ //si il existe!

        strcpy(nom,fichierinfo.nom);

        nom[strcspn(nom, "\n")] = '\0';  // Supprimer le newline caractére si il est présent (pour un affichage plus mieux de nom)

        printf("\nFICHIER AVEC LE NOM \" %s \" EXISTE DEJA DANS LA MEMOIRE!\n",nom);

        printf("\nDonner un nom different (le nom est sensible a la casse): ");
    }


    }while(found == true); // tanque le nom donné est déja réserver pour un fichier il faire une loop pour demander un nouveau nom.
    do{

    printf("\nNombre d'Enregistrements: ");

    scanf("%d",&fichierinfo.taille_Enrgs);

    }while(fichierinfo.taille_Enrgs<=0);

    do{

    printf("\nMode d'Organisation Externe:\n1: ORGANISATION CONTIGUE\t0: ORGANISATION CHAINEE\nEntrer: ");
    
    scanf("%d",&fichierinfo.OrganisationE);

    }while(fichierinfo.OrganisationE != 1 && fichierinfo.OrganisationE != 0);

    do{

    printf("\nMode d'Organisation Interne:\n1: ORGANISATION ORDONNE\t0: ORGANISATION NON-ORDONNE\nEntrer: ");

        scanf("%d",&fichierinfo.OrganisationI);

    }while(fichierinfo.OrganisationI != 1 && fichierinfo.OrganisationI != 0);

    fichierinfo.taille_Blocs = fichierinfo.taille_Enrgs / FB; // le nombres de blocs nécessaire pour crée le fichier dans la memoire secrondaire.

    if(fichierinfo.taille_Enrgs % FB != 0){

        fichierinfo.taille_Blocs += 1;/*si par exemple le nombre d'enregistrements d'un fichier n'est pas un multiple de FB par exemple 13 enregistrements,
                        alors le nombres de Bloc nécessaire pour stocker le fichier est 2 Blocs pour 12 enregistrements + 1 Bloc pour le reste.*/

    }

    getchar(); //consomer le charactére '\n'.

    return fichierinfo;

}

void EcrireMetaD_Enreg(Bloc *MetaBloc, MetaD fichierinfo, int niveau,int adress) { //cette fonction est utiliser  pour ecrire MetaDonné d'un fichier d'aprés niveau et adress dans la MetaBloc
/*
cette fonction est la base d'écriture de MetaDonné d'un fichier dans
un bloc de MetaDonne, puisque chaque enregistrement contient un tableau de 140 charactérs, 
et notre metadonné reserve un espace equivalent de 70 charactérs(voire la structure de métadonné) 
alors on peut sauvgardé métadonné de deux fichier niveau 1 et niveau 2 telleque :
(Enregistrement[0].data de 0 a 70 == niveau 1 et Enregistrement[0].data de 70 a 140 == niveau 2)
*/

    if(niveau == 1){

    memcpy(MetaBloc->enregistrements[adress].data, fichierinfo.nom, 50); 

    memcpy(MetaBloc->enregistrements[adress].data + 50, &fichierinfo.taille_Blocs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 54, &fichierinfo.taille_Enrgs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 58, &fichierinfo.adresse_PremierBloc, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 62, &fichierinfo.OrganisationE, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 66, &fichierinfo.OrganisationI, sizeof(int));

        }else{

        if(niveau == 2){

    memcpy(MetaBloc->enregistrements[adress].data +70, fichierinfo.nom, 50);
    memcpy(MetaBloc->enregistrements[adress].data + +120, &fichierinfo.taille_Blocs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 124, &fichierinfo.taille_Enrgs, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 128, &fichierinfo.adresse_PremierBloc, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 132, &fichierinfo.OrganisationE, sizeof(int));

    memcpy(MetaBloc->enregistrements[adress].data + 136, &fichierinfo.OrganisationI, sizeof(int));

        }else{
            printf("\nErreur!, dans function *EcrireMetaD_Enreg*, niveau saisie n'existe pas!.");
            return;
        }
        }

}

void SupprimerMetaD(Bloc *MetaBloc, int niveau, int adress){ //supprimer la metadonné d'un metabloc d'aprés niveau et adress

    if(niveau == 1){

        memset(MetaBloc->enregistrements[adress].data, 0, 50);

        memset(MetaBloc->enregistrements[adress].data + 50, 0, sizeof(int));

        memset(MetaBloc->enregistrements[adress].data + 54, 0, sizeof(int));

        memset(MetaBloc->enregistrements[adress].data + 58, 0, sizeof(int));

        memset(MetaBloc->enregistrements[adress].data + 62, 0, sizeof(int));

        memset(MetaBloc->enregistrements[adress].data + 66, 0, sizeof(int));

    }else{
        if(niveau == 2){


            memset(MetaBloc->enregistrements[adress].data +70, 0, 50);

            memset(MetaBloc->enregistrements[adress].data + 120, 0, sizeof(int));

            memset(MetaBloc->enregistrements[adress].data + 124, 0, sizeof(int));

            memset(MetaBloc->enregistrements[adress].data + 128, 0, sizeof(int));

            memset(MetaBloc->enregistrements[adress].data + 132, 0, sizeof(int));

            memset(MetaBloc->enregistrements[adress].data + 136, 0, sizeof(int));

        }else{
    }
        printf("\nErreur! function *SupprimerMetaD*, niveau inserer n'existe pas!.");
    }
}

MetaD LireMetaD_Enreg(Buffer MetaBloc,int niveau , int adress){ //lire Metadonnés d'un metabloc d'aprés niveau et adress

    /*
    Cette fonction fait l'inverse de EcrireMetaD_Enreg. Elle lit la métadonnée d'un fichier dans un bloc de métadonnée avec l'adresse et le niveau
    correspondant.
    */

    
    MetaD fichierinfo;

        if(niveau == 1){

        
        memcpy(fichierinfo.nom, MetaBloc.enregistrements[adress].data, 50);

        memcpy(&fichierinfo.taille_Blocs, MetaBloc.enregistrements[adress].data + 50, sizeof(int));

        memcpy(&fichierinfo.taille_Enrgs, MetaBloc.enregistrements[adress].data + 54, sizeof(int));

        memcpy(&fichierinfo.adresse_PremierBloc, MetaBloc.enregistrements[adress].data + 58, sizeof(int));

        memcpy(&fichierinfo.OrganisationE, MetaBloc.enregistrements[adress].data + 62, sizeof(int));

        memcpy(&fichierinfo.OrganisationI, MetaBloc.enregistrements[adress].data + 66,sizeof(int));

        }else{

            if(niveau == 2){
        
        memcpy(fichierinfo.nom, MetaBloc.enregistrements[adress].data + 70 , 50);

        memcpy(&fichierinfo.taille_Blocs, MetaBloc.enregistrements[adress].data + 120, sizeof(int));

        memcpy(&fichierinfo.taille_Enrgs, MetaBloc.enregistrements[adress].data + 124, sizeof(int));

        memcpy(&fichierinfo.adresse_PremierBloc, MetaBloc.enregistrements[adress].data + 128, sizeof(int));

        memcpy(&fichierinfo.OrganisationE, MetaBloc.enregistrements[adress].data + 132, sizeof(int));

        memcpy(&fichierinfo.OrganisationI, MetaBloc.enregistrements[adress].data + 136,sizeof(int));

        }else{
            printf("\nErreur dans function *LireMetaD_Enreg*, niveau saise n'existe pas!");
            return fichierinfo;
        }
        }
        

        return fichierinfo;

}

void sauvgarder_MetaD(MS MemoireS, MetaD fichierinfo,Buffer MetaBloc){ /*cette function sauvgarde les métadonnés d'un fichier dans la memoire secondaire*/
    
    rewind(MemoireS.memoireS);

    bool trouver = false;

    int bloc_index = 1;

    fseek(MemoireS.memoireS,sizeof(Bloc),SEEK_CUR);

    while(trouver == false && bloc_index <= ceil( (NbrBlocs - 1) / ( FB * nbrMeta_enreg ) )){

        fread(&MetaBloc,sizeof(Buffer),1,MemoireS.memoireS);

        if(MetaBloc.nbrE < FB){ // on vérfifier est-ce-que les enregistrements de bloc meta donnes n'est pas pleine!.

        trouver = true;

        if(MetaBloc.enregistrements[MetaBloc.nbrE].data[0] == '\0'){ //on vérifier est-ce-que niveau 1 de l'enregistrement contient déja des information de metadonné d'un seule fichier ou non.

        EcrireMetaD_Enreg(&MetaBloc,fichierinfo,1,MetaBloc.nbrE);

        }else{

        EcrireMetaD_Enreg(&MetaBloc,fichierinfo,2,MetaBloc.nbrE);

        
        MetaBloc.nbrE++;
    /*
    Dans ce cas, l'enregistrement de l'adresse MetaBloc.nbrE devient plein ! (car les niveaux 1 et 2 deviennent pleins) alors on fait MetaBloc.nbrE++; .
    NOTE IMPORTANTE : Le nbrE dans les blocs réservés comme MetaBlocs représente le nombre d'enregistrements PLEINS et non le nombre d'enregistrements OCCUPÉS.
    */


        }

    }else{

        bloc_index++;

    }
    }

    if(trouver == true){
        
        fseek(MemoireS.memoireS, bloc_index * sizeof(Bloc),SEEK_SET);

        fwrite(&MetaBloc,sizeof(Buffer),1,MemoireS.memoireS);

    }else{
        printf("\nMetaData de fichier %s ne peut pas etre sauvgarder.",fichierinfo.nom);
    }

    

}

MetaD RechercheMetaD(MS MemoireS,char nom[50],bool* trouver,int* index,int *adress, int* niveau){ //recherche metadonné d'un fichier d'aprés son nom

Buffer MetaBloc;

MetaD fichierinfo_niveau1,fichierinfo_niveau2;

    memset(&fichierinfo_niveau1, 0, sizeof(MetaD));

    memset(&fichierinfo_niveau2, 0, sizeof(MetaD));

    fseek(MemoireS.memoireS,sizeof(Bloc),SEEK_SET);

*trouver=false;

int i=1,j=0;
*index=0; //index représente l'index de MetaBloc qui commence a partir de 1. (on la mise a jour dans la boucle)

while(i<=ceil( (NbrBlocs -1) / (FB*nbrMeta_enreg) ) && *trouver == false){
    
    fread(&MetaBloc,sizeof(Buffer),1,MemoireS.memoireS);

    *index += 1;

    while(*trouver == false && j<=MetaBloc.nbrE){

    fichierinfo_niveau1 = LireMetaD_Enreg(MetaBloc,1,j);

    fichierinfo_niveau2 = LireMetaD_Enreg(MetaBloc,2,j);

    if(strcmp(fichierinfo_niveau1.nom,nom) == 0){

        *trouver = true;

        *adress = j;

        *niveau = 1;

        return fichierinfo_niveau1;

    }else{

        if(strcmp(fichierinfo_niveau2.nom,nom)==0){

            *trouver = true;

        *adress = j;

        *niveau = 2;

            return fichierinfo_niveau2;
        }
    }

    j++;

}

i++;

}
    MetaD vide;
    
    memset(&vide, 0, sizeof(MetaD));

    return vide;
}

void Affichier_Fichiers_Meta(MS MemoireS){

    fseek(MemoireS.memoireS,sizeof(Bloc),SEEK_SET);

    int nbrMetaBlocs = ceil(( NbrBlocs - 1 )/ (FB * nbrMeta_enreg));

    Bloc MetaBloc;

    MetaD fichierinfo;

    int i,j,niveau;

    bool found =false;

    for(i=0; i< nbrMetaBlocs; i++){
    
    fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

    j=0,niveau = 1;

    while(j<= MetaBloc.nbrE ){

    fichierinfo = LireMetaD_Enreg(MetaBloc,niveau,j);

    if(fichierinfo.nom[0] != '\0'){

        found = true;

    printf("\nNom du fichier: %s ",fichierinfo.nom);

    printf("\nAdress de Premier Bloc: %d ",fichierinfo.adresse_PremierBloc);

    printf("\nNombres d'enregistrement occupe: %d",fichierinfo.taille_Enrgs);

    printf("\nNombres de Blocs occupe: %d",fichierinfo.taille_Blocs);

    printf("\nMode d'Organisation Externe : %s", (fichierinfo.OrganisationE == 1) ? "Contigue" : "Chainee");

    printf("\nMode d'Organisation Interne : %s", (fichierinfo.OrganisationI == 1) ? "Ordonne" : "Non Ordonne");

    printf("\n");

    }

    niveau++;

    if(niveau > 2){

        niveau = 1;

        j++;
    }

    }

    }

    if(found == false){

        printf("\nPas de fichiers metadonnees trouver dans la memoire seconadire!");
        
    }
}
