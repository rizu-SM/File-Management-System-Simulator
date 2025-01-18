#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "declarations.h"

#include "functsmemoire.h"

#include "functsFichierMeta.h"

#include "functsFichierDonnes.h"

// Définir les couleurs pour les blocs
#define RESET "\033[0m"
#define ROUGE "\033[41m"
#define VERT "\033[42m"

// Définir la largeur d'affichage pour plus de lisibilité
#define BLOCS_PAR_LIGNE 8
#define BLOCS_PAR_COLONNE 8



//LES FUNCTIONS:


void AfficherEtatMemoire(MS* MemoireS) {
    Bloc buffer;
    int i, nom = 1;

    printf("\nEtat de la memoire secondaire :\n");

    // Parcourir tous les blocs
    for (i = 22; i < MemoireS->nbrBlocs; i++) {
        fseek(MemoireS->memoireS, i * sizeof(Bloc), SEEK_SET);
        fread(&buffer, sizeof(Bloc), 1, MemoireS->memoireS);

        // Afficher les blocs avec des couleurs et espaces
        if (buffer.nbrE > 0) {
            // printf(ROUGE " Fichier (%d Enreg) " RESET " ", buffer.nbrE);
            printf(ROUGE " Fichier '%d' (%d Enreg) " RESET " ", nom, buffer.nbrE);
            nom++;
        } else {
            printf(VERT " Libre " RESET " ");
        }

        // Ajouter une nouvelle ligne après un certain nombre de blocs
        if ((i - 21) % BLOCS_PAR_LIGNE == 0) {
            printf("\n\n");
        }
    }

    printf("\n");
}


void compactage(MS *ms, MetaD *metaData) {
    Bloc buffer; // Utilisé pour lire et écrire dans les blocs
    Bloc allocationTable; // Utilisé pour lire et écrire dans le bloc0
    Bloc blocVide = {0}; // Initialise un bloc vide
    int blocActuel = 1; // On commence après les blocs réservés
    int blocDestination = 1; // Destination pour décaler les blocs remplis

    // Parcours de tous les blocs
    while (blocActuel < ms->nbrBlocs) {
        fseek(ms->memoireS, blocActuel * sizeof(Bloc), SEEK_SET);
        fread(&buffer, sizeof(Bloc), 1, ms->memoireS);

        if (buffer.nbrE > 0) { // Si le bloc actuel contient des enregistrements
            if (blocActuel != blocDestination) {
                // Déplacer le bloc vers la destination
                fseek(ms->memoireS, blocDestination * sizeof(Bloc), SEEK_SET);
                fwrite(&buffer, sizeof(Bloc), 1, ms->memoireS);


                // Supprimer l'ancien bloc
                fseek(ms->memoireS, blocActuel * sizeof(Bloc), SEEK_SET);
                fwrite(&blocVide, sizeof(Bloc), 1, ms->memoireS);
                
                int i,j;

                // Mettre à jour les métadonnées pour refléter la nouvelle adresse....................................................
                for (i = 1; i <= 21; i++) { // Parcourir les metablocs
                    fseek(ms->memoireS, i * sizeof(Bloc), SEEK_SET);
                    fread(&buffer, sizeof(Bloc), 1, ms->memoireS);
                    
                    for (j = 0; j < FB; j++) { // Parcourir les enregistrements
                        if (!buffer.enregistrements[j].isDeleted) {
                            // Niveau 1
                            MetaD meta;
                            memcpy(&meta.adresse_PremierBloc, buffer.enregistrements[j].data + 58, sizeof(int));
                            if (meta.adresse_PremierBloc == blocActuel) {
                                // Mettre à jour l'adresse
                                meta.adresse_PremierBloc = blocDestination;
                                memcpy(buffer.enregistrements[j].data + 58, &meta.adresse_PremierBloc, sizeof(int));
                            }

                            // Niveau 2
                            memcpy(&meta.adresse_PremierBloc, buffer.enregistrements[j].data + 128, sizeof(int));
                            if (meta.adresse_PremierBloc == blocActuel) {
                                // Mettre à jour l'adresse
                                meta.adresse_PremierBloc = blocDestination;
                                memcpy(buffer.enregistrements[j].data + 128, &meta.adresse_PremierBloc, sizeof(int));
                            }
                        }
                    }
                    // Sauvegarder les modifications dans le metabloc
                    fseek(ms->memoireS, i * sizeof(Bloc), SEEK_SET);
                    fwrite(&buffer, sizeof(Bloc), 1, ms->memoireS);
                }


                // Marquer la destination comme utilisée................................................................
                rewind(ms->memoireS);
                fread(&allocationTable, sizeof(Bloc), 1, ms->memoireS);

                if (blocDestination < 140)
                {
                    allocationTable.enregistrements[0].data[blocDestination] = 1;
                    fwrite(&allocationTable, sizeof(Bloc), 1, ms->memoireS);
                }else
                {
                    allocationTable.enregistrements[1].data[blocDestination - 140] = 1;
                    fwrite(&allocationTable, sizeof(Bloc), 1, ms->memoireS);
                }

                // Marquer le Actuel comme libre.........................................................................

                if (blocActuel < 140)
                {
                    allocationTable.enregistrements[0].data[blocActuel] = 0;
                    fwrite(&allocationTable, sizeof(Bloc), 1, ms->memoireS);
                }else
                {
                    allocationTable.enregistrements[1].data[blocActuel - 140] = 0;
                    fwrite(&allocationTable, sizeof(Bloc), 1, ms->memoireS);
                }
            }

            blocDestination++;
        }

        blocActuel++;
    }

    printf("Compactage terminé avec succès.\n");
}

void afficherMenu() { //cette function est utilisé pour afficher le menu pour l'utilisateur.
    printf("\n======== MENU PRINCIPAL ========\n");
    printf("1. Initialiser la memoire secondaire (Attention ! , l'initialisation supprimera tous les fichiers existants !)\n");
    printf("2. Affichier la table d'allocation de la memoire secondaire\n");
    printf("3. Creer un fichier (il sera etre sauvgarder automatiquement dans la MS)\n");
    printf("4. Lire un fichier (il sera etre lu automatiquement de la MS)\n");
    printf("5. Afficher l'etat de la memoire\n");
    printf("6. Afficher les metadonnees des fichiers\n");
    printf("7. Rechercher un enregistrement\n");
    printf("8. Inserer un nouveau enregistrement\n");
    printf("9. Supprimer un enregistrement logique\n");
    printf("10. Supprimer un enregistrement physique\n");
    printf("11. Defragmenter un fichier\n");
    printf("12. Supprimer un fichier\n");
    printf("13. Renommer un fichier\n");
    printf("14. Compactage de la memoire secondaire\n");
    printf("15. Vider la memoire secondaire\n");
    printf("16. Quitter\n");
    printf("================================\n");
    printf("Entrez votre choix : ");

    return;
}


int main(){

    MS MemoireS;

    bool opened=false;

    int choix;

    Bloc MetaBloc,Bloc0;

    MemoireS.memoireS=fopen("memoire_secondaire.bin","rb+");

    if(MemoireS.memoireS != NULL){

        printf("\n**Detection d'une memoire secondaire deja existante**\n");

        opened = true;
    }
    
    bool on = true;

while(on == true){

afficherMenu();

scanf("%d",&choix);

getchar(); //consomer le '/n'

switch (choix){

case 1:

    if(opened == true){
        
        printf("\nDetection d'une memoire secondaire deja existante ! Etes-vous sur de vouloir l'initialiser a nouveau ?");

        printf("\nSaisissez n'importe quel caractere pour continuer, sinon fermez le programme !\n");

        getchar();

        getchar(); // consomer le 'n'
    }

    InitialiseDisk(&MemoireS);

    MemoireS.memoireS= fopen("memoire_secondaire.bin","rb+");

    if(MemoireS.memoireS == NULL){

        printf("\nErreur!, impossible d'acceder a la memoire_secondaire.bin!");

        opened = false;

    }else{

        opened = true;
    }

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();

    break;

case 2: if(opened == true){

    printf("\n ** NOTE ** : Bloc etat: 1 --> Occupe \\ Bloc etat:0 --> Libre\n\n");

Bloc bloc0 = LireBloc0(MemoireS);

int i=0, j=0, k=0, counter=0;

while(i < NbrBlocs){

    if(k == data_size){
        j++;
        k=0;
    }


    printf("Bloc %d etat: ", i);

    if(bloc0.enregistrements[j].data[k] == 0){
        printf(VERT "%d" RESET "             ", bloc0.enregistrements[j].data[k]); // vert pour 0
    } else {
        printf(ROUGE "%d" RESET "             ", bloc0.enregistrements[j].data[k]);   // rouge pour 1
    }

    counter++;

    if(counter == 3){
        printf("\n\n");
        counter = 0;
    }

    i++;
    k++;
}


}else{
    printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

    printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");
}

printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();

break;
case 3:

if(opened == true){

MetaD Fichier_Meta;

bool espace_libre;

Fichier_Meta = CreeFichier(MemoireS);

Bloc0 = LireBloc0(MemoireS);

espace_libre = GES_Creation(&Fichier_Meta,&Bloc0);

if(espace_libre == true){

    sauvgarder_MetaD(MemoireS,Fichier_Meta,MetaBloc);

    AllouerFichier(MemoireS,Fichier_Meta,true,NULL);

}else{

    printf("\nGestion d'espcace de stockage : Espace insuffisant pour la creation d'un nouveau fichier!");

}

}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}


    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();

    break;



case 4:
if(opened == true){

    char nom[50];

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour la Lecture: ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");

    fgets(nom,sizeof(nom),stdin);
    
    LireFichier(MemoireS,nom);

}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();


    break;

case 5: AfficherEtatMemoire(&MemoireS);

printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();
break;

case 6:

if(opened == true){

    Affichier_Fichiers_Meta(MemoireS);

}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");


}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();

    break;

case 7: if(opened == true){

    char nom[50];

    int id;

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour rechercher son enregistrement: ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");
    fgets(nom,sizeof(nom),stdin);

    printf("\nEntrer le ID de l'Enregistrement a rechercher: ");

    scanf("%d",&id);

    Recherche_Enregistrement(MemoireS,nom,id,NULL,NULL);


    getchar(); //consome le '\n'


}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();


    break;


case 8:

if(opened == true){

    char nom[50];

    enregistrement enreg;

    produit product;

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour rechercher son enregistrement: ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");

    fgets(nom,sizeof(nom),stdin);

    printf("\nEntrer le ID de l'Enregistrement a inserer (La meme chose que le ID de Produit): ");

    scanf("%d",&enreg.id);

    getchar();//consomer le '\n'

    printf("\nEntrer le nom du produit: ");

    memset(product.nom,0,sizeof(product.nom));

    fgets(product.nom,sizeof(product.nom),stdin);

    memcpy(enreg.data,product.nom,sizeof(product.nom));

    printf("\nEnter le prix du produit: ");

    scanf("%lf",&product.prix);

    memcpy(enreg.data + sizeof(product.nom),&product.prix,sizeof(product.prix));

    GES_insertion(MemoireS,enreg,nom);

    getchar(); //consome le '\n'


}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();


    break;


case 9:

if(opened == true){

    char nom[50];

    int id;

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour supprimer son enregistrement (logiquement): ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");
    fgets(nom,sizeof(nom),stdin);

    printf("\nEntrer le ID de l'Enregistrement a supprimer (logiquement): ");

    scanf("%d",&id);

    SuppressionLogique(MemoireS,nom,id);

    getchar(); //consome le '\n'


}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

printf("\nSaisissez n'importe quel caractere pour continuer...\n");


    getchar();

    break;


case 10:
if(opened == true){

    char nom[50];

    int id;

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour supprimer son enregistrement (physiquement): ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");
    fgets(nom,sizeof(nom),stdin);

    printf("\nEntrer le ID de l'Enregistrement a supprimer (pyhsiquement): ");

    scanf("%d",&id);

    getchar(); //consome le '\n'


    SuppressionPhysqiue(MemoireS,nom,id);


}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

printf("\nSaisissez n'importe quel caractere pour continuer...\n");


    getchar();

    break;

case 11: if(opened == true){

    char nom[50];

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour Defragmenter: ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");

    fgets(nom,sizeof(nom),stdin);

    Defragmenter(MemoireS,nom);

}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();


    break;

case 12:

if(opened == true){

    char nom[50];

    memset(nom,0,sizeof(nom));

    printf("\nEntrer le Nom du fichier pour Supprimer: ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");

    fgets(nom,sizeof(nom),stdin);

    SupprimerFichier(MemoireS,nom);

}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();


    break;


case 13:

if(opened == true){

    char nom[50],nouveauNom[50];

    memset(nom,0,sizeof(nom));

    memset(nouveauNom,0,sizeof(nouveauNom));

    printf("\nEntrez le nom du fichier a renommer: ");
    printf("\nAttention! , le Nom est sensible a la casse (CASE SENSITIVE)");
    printf("\nEntrer: ");

    fgets(nom,sizeof(nom),stdin);

    printf("\nEntrer le nouveau nom du fichier: ");

    fgets(nouveauNom,sizeof(nouveauNom),stdin);

    RenommerFichier(MemoireS,nom,nouveauNom);


}else{

printf("\nImpossible de continuer, l'acces a la memoire secondaire n'est pas disponible!\n");

printf("\nVerifier si vous avez initialiser la memoire secondaire avec la commande 1,Sinon un probleme d'acess a memoire_secondaire.bin!");

}

    printf("\nSaisissez n'importe quel caractere pour continuer...\n");

    getchar();


    break;



case 14: printf("\nmalheureusement mensieur pas de compactage...");

break;

case 15: printf("\nVoues etes sure vous voulez vider la memoire secondaire? (Oui/Non)\n");

        char reponse[3];

        scanf("%s",reponse);

        getchar(); // consomer le '\n'

        reponse[0] = tolower(reponse[0]);

        reponse[1] = tolower(reponse[1]);

        reponse[2] = tolower(reponse[2]);

        if(strcmp(reponse,"oui") == 0){

            vider_MS(&MemoireS);


        }else{

            printf("\nL'operation a ete annulee!\n");
        }

        
        printf("\nSaisissez n'importe quel caractere pour continuer...\n");

        getchar();
        



case 16: on = false;
break;

default:
    break;









}

}

}


























