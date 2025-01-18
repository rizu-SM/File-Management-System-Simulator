#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <stdio.h>

#include <stdbool.h>


//déclaration des constants nécessaire:

#define FB 6 //facteur de blocage

#define NbrBlocs 256 //nombre de blocs maximale pour la memoire secrondaire.

#define data_size 140 //chaque enregistrement contient un tableau de charachtéres pour stocker des infromations multiples,
                      //le data_size représente la taille maximale de ce tableau

#define nbrMeta_enreg 2 //nombre de fichiers on peut représenter ses métadonnées dans un seule enregistrement.
                        //ça veut dire on peut representer des métadonnées de 2 fichiers dans un seule enregistrement de bloc réservé pour les métadonnées.


//déclaration des structures nécessaires:


typedef struct enregistrement{
    
    int id;
    
    char data[data_size];

    bool isDeleted; //ce variable est utilisé pour la suppression logique des enregistrements.

}enregistrement;


typedef struct Bloc{
    
    enregistrement enregistrements[FB];

    int nbrE; //le nombre d'enregistrements actuellement occupé dans un Bloc

    int nextBloc; /*un entier qui conserve l'index de prochaine Bloc dans le cas
    d'organisation chainée*/

}Bloc, Buffer; //le buffer contient la meme structure que un Bloc.

typedef struct Memoire_Secondaire{

    FILE *memoireS;

    int nbrBlocs; //cette variable indique le nombre de Blocs maximale de la memoire secrondaire.

    int bloc_size; //nombre d'enregistrements maximale dans un bloc

    double TailleMemoire;

}MS;

typedef struct Meta_Donnes{

    char nom[50];

    int taille_Blocs; //il prend 4 charachtéres dans data[data_size] d'un l'enregistrement.

    int taille_Enrgs; //+4

    int adresse_PremierBloc; //+4

    int OrganisationE; //on représent L'Organisation Contigue par 1 et Chainée par 0.

    int OrganisationI; //on représente L'Organisation Ordonné par 1 et NonOrdonné par 0


}MetaD;


typedef struct produit{ //exemplaire a utilisé

    char nom[27];

    double prix;

}produit;


#endif
