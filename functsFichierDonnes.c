#include "functsFichierDonnes.h"

#include "declarations.h"

#include "functsFichierMeta.h"

#include "functsmemoire.h"

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdbool.h>

#include <time.h>




void insererEnregistrement_Ordonne(Bloc fichierblocs[],int size,enregistrement enreg){

int i=0,j=0;

bool trouver = false;


while(i<size && trouver == false){

    while(j<fichierblocs[i].nbrE && trouver == false){

    if(fichierblocs[i].enregistrements[j].id >= enreg.id){

        trouver = true;

    }else{
        j++;
    }


}


if(trouver == false){

        j=0;
        i++;
    }
    
}

    if(trouver == true){ //insertion au début ou au mileu

    int k = size -1;
    
    int m;

    while(k > i){

    m = fichierblocs[k].nbrE - 1;

    while(m>=0){

        fichierblocs[k].enregistrements[m + 1] = fichierblocs[k].enregistrements[m];

        m = m -1;
    }

    if(k - 1 >= 0){

    fichierblocs[k].enregistrements[0]  = fichierblocs[k-1].enregistrements[fichierblocs[k-1].nbrE - 1];

    }

    k--;

    }

    fichierblocs[size-1].nbrE++;



//on arrive a le bloc i-eme ou il faut faire l'insertion :

m = fichierblocs[i].nbrE - 1;

    while(m>=j){

        fichierblocs[i].enregistrements[m + 1] = fichierblocs[i].enregistrements[m];

        m--;
    }

    fichierblocs[i].enregistrements[j] = enreg; //on insere l'enregistrement!


    }else{ //insertion a la fin!

        i=i-1;

        printf("\ni = %d",i);

        if(fichierblocs[i].nbrE >0){

        fichierblocs[i].enregistrements[fichierblocs[i].nbrE - 1] = enreg;

        }else{

        fichierblocs[i].enregistrements[0] = enreg;

        }

        fichierblocs[i].nbrE ++;


    }

    printf("\nL'enregistrement avec ID %d a ete inserer avec succes! ",enreg.id);
}

void AllouerFichier(MS MemoireS,MetaD fichierinfo,bool nouveau,Buffer* fichier){
    
    int i,nbrEReste=fichierinfo.taille_Enrgs,m,size;

    Buffer* buffer;

    if(nouveau == true){

    buffer = (Bloc *)malloc(fichierinfo.taille_Blocs * sizeof(Bloc));

    //initialisation des blocs de fichier.

    printf("\nChargement de l'exemplaire de fichier de PRODUITS..");

    char nomP[27]; //nom du produit

    int idP;// id du produit

    double prix;//prix du produit

    int j,p=1;

    srand((unsigned int)time(NULL));

    for(i=0; i<fichierinfo.taille_Blocs; i++){

        buffer[i].nextBloc=-1;

        buffer[i].nbrE = 0; //inittialisation

        for(m=0; m<FB; m++){

        buffer[i].enregistrements[m].isDeleted = false;

        memset(buffer[i].enregistrements[m].data, 0, sizeof(buffer[i].enregistrements[m].data));

        }

        if(nbrEReste/FB >0){

        size = FB;

    }else{

        size = nbrEReste;

    }


    j=0;

    while(j < size ){


    memset(nomP,0,sizeof(nomP));

    strcpy(nomP,"Produit ");

    sprintf(nomP + strlen(nomP), "%d", p);


    idP = p;

    prix = 150 + (double)rand() / RAND_MAX * (20000 - 150);

    buffer[i].enregistrements[j].id = idP;

    buffer[i].enregistrements[j].isDeleted = false;

    memcpy(buffer[i].enregistrements[j].data, nomP, sizeof(nomP));

    memcpy(buffer[i].enregistrements[j].data + sizeof(nomP), &prix, sizeof(prix));

    buffer[i].nbrE++;

    printf("\nProduit %d :",p);

    printf("\nID: %d ",idP);

    printf("\nPrix %.2lf ",prix);

    printf("\n*Produit de ID %d ajoute avec succes dans le fichier*\n",idP);

    j++;

    p++;


    }

    nbrEReste = nbrEReste - FB;
    
    }



    }else{

    Buffer* buffer = fichier;

    }
    

    Bloc Bloc0;

    rewind(MemoireS.memoireS);

    fread(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

    int k = fichierinfo.adresse_PremierBloc / data_size; //calculer dans quelle enregistrement l'état de PremierBloc dans la table d'allocation est situé.

    int j = fichierinfo.adresse_PremierBloc - k * data_size;

    if(fichierinfo.OrganisationE == 1){ //organisation contigue

    fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

    for(i=0; i<fichierinfo.taille_Blocs; i++){

            if(j>=data_size){
                k +=1;
                j =0;
            }

            Bloc0.enregistrements[k].data[j] = 1;


            fwrite(&buffer[i],sizeof(Bloc),1,MemoireS.memoireS);


            j +=1;

    }
    }else{
        if(fichierinfo.OrganisationE == 0){ //Organisation Chainé
        
        int i=0;


        while(i<fichierinfo.taille_Blocs){

            if(j>=data_size){
                k +=1;
                j =0;
            }
        if(Bloc0.enregistrements[k].data[j] == 0){
        
        Bloc0.enregistrements[k].data[j] = 1;

        i++;

        if(i>0 && i<fichierinfo.taille_Blocs){

            buffer[i-1].nextBloc = k*data_size + j + 1; //on remplie le variable de chainage des Blocs dans le Buffer.

        }

        }

        j +=1;



        }
        
        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

        i=0;

        while(i<fichierinfo.taille_Blocs){

        fwrite(&buffer[i],sizeof(Bloc),1,MemoireS.memoireS);

        fseek(MemoireS.memoireS,(buffer[i].nextBloc)*sizeof(Bloc),SEEK_SET);

        i++;

        }

        }
    }
    
    rewind(MemoireS.memoireS);

    Bloc0.nextBloc = Bloc0.nextBloc - fichierinfo.taille_Blocs;

    fwrite(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS); //mise a jour de table d'allocation.

    free(buffer);
}


void SupprimerFichier(MS MemoireS, char nom[50]){

    bool trouver;

    int index,adress,niveau;

    MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adress,&niveau);

    if(trouver == true){

        rewind(MemoireS.memoireS);

        Buffer bloc0,bloc;

        fread(&bloc0,sizeof(bloc0),1,MemoireS.memoireS);

        int k = fichierinfo.adresse_PremierBloc / data_size; //enregistrement[k] dans le Bloc0 de la table d'allocation

        int j = fichierinfo.adresse_PremierBloc % data_size; //enregistrement[k].data[j] dans le Bloc0 de la table d'allocation

        if(fichierinfo.OrganisationE == 1){

        //supression dans la memoire secondaire ET mise a jour la table d'allocation:
        
        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc * sizeof(Bloc),SEEK_SET);

        int i,m;

        for(i=0; i<fichierinfo.taille_Blocs; i++){

            fread(&bloc,sizeof(bloc),1,MemoireS.memoireS);

            for(m=0; m<FB; m++){

            memset(bloc.enregistrements[m].data, 0, sizeof(bloc.enregistrements[m].data)); //vider les enregistrements de bloc.

            }

            bloc.nbrE = 0;

            fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);
            
            fwrite(&bloc,sizeof(Bloc),1,MemoireS.memoireS);

            bloc0.enregistrements[k].data[j] = 0;

            j += 1;

            if(j>=data_size){
                k += 1;
                j = 0;
            }

        }

        rewind(MemoireS.memoireS);

        bloc0.nextBloc -= fichierinfo.taille_Blocs;

        fwrite(&bloc0,sizeof(Bloc),1,MemoireS.memoireS);
        
        }else{ //ORGANISATION CHAINEE!

        //supression dans la memoire secondaire ET mise a jour la table d'allocation:
        
        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc * sizeof(Bloc),SEEK_SET);

        int i,m,nextBloc;

        for(i=0; i<fichierinfo.taille_Blocs; i++){

        fread(&bloc,sizeof(bloc),1,MemoireS.memoireS);

        for(m=0; m<FB; m++){

            memset(bloc.enregistrements[m].data, 0, sizeof(bloc.enregistrements[m].data)); //vider les enregistrements de bloc.

        }
            bloc.nbrE = 0;

            nextBloc = bloc.nextBloc;

            bloc.nextBloc = -1;

            fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);
            
            fwrite(&bloc,sizeof(Bloc),1,MemoireS.memoireS);

            fseek(MemoireS.memoireS, nextBloc * sizeof(Bloc),SEEK_SET);

            bloc0.enregistrements[k].data[j] = 0;

            k = nextBloc / data_size;

            j = nextBloc % data_size;


        }

        rewind(MemoireS.memoireS);

        bloc0.nextBloc -= fichierinfo.taille_Blocs;

        fwrite(&bloc0,sizeof(Bloc),1,MemoireS.memoireS);

        }

        //supression de meta data de fichier!

        Bloc MetaBloc;

        fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

        fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

        SupprimerMetaD(&MetaBloc,niveau,adress);

        fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

        fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

        printf("\nLe fichier %s a ete supprimer avec succes.\n",fichierinfo.nom);


    }else{
        printf("\nErreur! fonction *SupprimerFichier*, fichier %s n'existe pas!",nom);
    }
}

bool Recherche_Enregistrement(MS MemoireS,char nom[50],int id,int* adressBloc,int*adressEnreg){/*cette fonction recherche un enregistrement aux dépand de son id , si elle le trouve
                                                                                                elle affiche l'enregistrement et elle retourn true, au plus elle renvoie l'adresse du bloc et de l'enregistrement
                                                                                                qui on va l'utilise dans la fonction SuppressionPhysique*/
    
    if(adressBloc != NULL && adressEnreg != NULL){//en case en a saisie NULL (dans le cas ou en veut la recherche juste pour afficher l'enrergistremet)

    *adressBloc = -1;//initilaser adressBloc et adressEnreg a -1 , en cas on ne trouve pas l'enregistrement.

    *adressEnreg = -1;
    }

    int niveau,adress,index;

    bool trouver;

    MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adress,&niveau);

    if(trouver == false){

        printf("\nImpossible de trouver le fichier avec le nom : %s",nom);

        return false;

    }else{

        Bloc bloc;

        produit product;

        bool enreg_trouver=false;

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);


        if(fichierinfo.OrganisationE == 1){ //organisation contigue

            int i=0,j;

            while(i<fichierinfo.taille_Blocs && enreg_trouver == false){

                fread(&bloc,sizeof(Bloc),1,MemoireS.memoireS);

                j=0;

                while(j< bloc.nbrE && enreg_trouver == false){

                    if(bloc.enregistrements[j].id == id ){
                        
                        if(adressBloc != NULL && adressEnreg != NULL){ //en case en a saisie NULL (dans le cas ou en veut la recherche juste pour afficher l'enrergistremet)

                        *adressBloc = fichierinfo.adresse_PremierBloc + i; //cette opétation nous donne l'adresse du bloc dans le cas Contigue

                        *adressEnreg = j;

                        }

                        printf("\nEnregistrement avec id=%d a ete trouve!\n",id);

                        printf("\nEnregistrement ID = %d",id);
                        printf("\nContenu:");

                        memcpy(product.nom,bloc.enregistrements[j].data,sizeof(product.nom));

                        printf("\nNom du Produit : %s",product.nom);

                        memcpy(&product.prix,bloc.enregistrements[j].data + sizeof(product.nom),sizeof(product.prix));

                        printf("\nPrix: %.2lf",product.prix);

                        enreg_trouver = true;

                    }else{
                        j++;
                    }
                }

                i++;

            }
        }else{ //organisation chainnee

        int i=0,j;

        Bloc blocPrecedent;

        while(i<fichierinfo.taille_Blocs && enreg_trouver == false){

                fread(&bloc,sizeof(Bloc),1,MemoireS.memoireS);

                j=0;

                while(j< bloc.nbrE && enreg_trouver == false){

                    if(bloc.enregistrements[j].id == id ){

                        if(adressBloc != NULL && adressEnreg != NULL){//en case en a saisie NULL (dans le cas ou en veut la recherche juste pour afficher l'enrergistremet)

                        if(i == 0){

                            *adressBloc = fichierinfo.adresse_PremierBloc;

                        }else{
                            
                            *adressBloc = blocPrecedent.nextBloc;

                        }

                        *adressEnreg = j;

                        }

                        printf("\nEnregistrement avec id=%d a ete trouve!\n",id);

                        printf("\nEnregistrement ID = %d",id);
                        printf("\nContenu:");

                        memcpy(product.nom,bloc.enregistrements[j].data,sizeof(product.nom));

                        printf("\nNom du Produit : %s",product.nom);

                        memcpy(&product.prix,bloc.enregistrements[j].data + sizeof(product.nom),sizeof(product.prix));

                        printf("\nPrix: %.2lf",product.prix);

                        enreg_trouver = true;

                    }else{
                        j++;
                    }
                }

                if(bloc.nextBloc != -1){

                    blocPrecedent = bloc;

                fseek(MemoireS.memoireS,bloc.nextBloc*sizeof(Bloc),SEEK_SET);

                }

                i++;

            }


        }

        return enreg_trouver;
    }
}

void LireFichier(MS MemoireS,char nom[50]){ //fonction pour lire un fichier - REMPARQUE IMPORTANTE : ELLE EST UTILISEE POUR LIRE UN FICHIER DE PRODUITS!

int index,adress,niveau;

bool trouver;

MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adress,&niveau);

if(trouver == false){

    printf("\nErreur!, fonction *LireFichier* fichier avec nom %s n'existe pas!",nom);

}else{

    char nomP[27];

    int idP;

    double prix;

    if(fichierinfo.OrganisationE == 1){ //si l'organisation est contigue

    fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

    Buffer buffer;

    int i,j;

    for(i=0;i<fichierinfo.taille_Blocs; i++){

    fread(&buffer,sizeof(Buffer),1,MemoireS.memoireS);

    for(j=0; j<buffer.nbrE; j++){

        if(buffer.enregistrements[j].isDeleted == false){

        printf("\nProduit ID : %d",buffer.enregistrements[j].id);

        memcpy(nomP,buffer.enregistrements[j].data,sizeof(nomP));

        printf("\nNom : %s",nomP);

        memcpy(&prix,buffer.enregistrements[j].data + sizeof(nomP),sizeof(prix));

        printf("\nPrix : %.2f",prix);

        printf("\n");

        }
        
    }

    }

    }else{ //organisation CHAINE

    fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

    Buffer buffer;

    int i=0,j;

    while(i<fichierinfo.taille_Blocs && buffer.nextBloc != -1){

        fread(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

        for(j=0; j< buffer.nbrE; j++){

        if(buffer.enregistrements[j].isDeleted == false){


        printf("\nProduit ID : %d",buffer.enregistrements[j].id);

        memcpy(nomP,buffer.enregistrements[j].data,sizeof(nomP));

        printf("\nNom : %s",nomP);

        memcpy(&prix,buffer.enregistrements[j].data + sizeof(nomP),sizeof(prix));

        printf("\nPrix : %.2lf",prix);

        printf("\n");

        }

        }

        fseek(MemoireS.memoireS,buffer.nextBloc*sizeof(Bloc),SEEK_SET);

        i++;
    }


    }
}
}

void RenommerFichier(MS MemoireS,char nom[50],char nouveauNom[50]){

    bool trouver;

    int index,adress,niveau;

    MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adress,&niveau);

    if(trouver == false){

        printf("\nErreur! impossible de trouver le fichier %s dans la memoire secondaire.\n",nom);
        return;
    }else{

        Buffer buffer;

        memset(fichierinfo.nom,0,sizeof(fichierinfo.nom));

        strcpy(fichierinfo.nom,nouveauNom);

        fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

        fread(&buffer,sizeof(Buffer),1,MemoireS.memoireS); //lire la méta-bloc

        EcrireMetaD_Enreg(&buffer,fichierinfo,niveau,adress);

        fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

        fwrite(&buffer,sizeof(Buffer),1,MemoireS.memoireS);

        printf("\nLe Fichier a ete renommer avec sucess!");

        return;
    }
}

void SuppressionLogique(MS MemoireS,char nom[50],int id){

    bool trouver;

    int index,adress,niveau;

    MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adress,&niveau);

    if(trouver == false){

        printf("\nErreur!, fichier %s non trouve dans la memoire secondaire.\n",nom);
        return;

    }else{

        Buffer buffer;

        int i=0,j;

        char nomP[27];

        memset(nomP,0,sizeof(nomP));

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

        if(fichierinfo.OrganisationE == 1){ //si l'organisation est Contigue

        for(i=0; i<fichierinfo.taille_Blocs; i++){

        fread(&buffer,sizeof(Buffer),1,MemoireS.memoireS);

            j=0;

        while(j<buffer.nbrE){

            if(buffer.enregistrements[j].id == id){

                memcpy(nomP,buffer.enregistrements[j].data,sizeof(nomP));

                buffer.enregistrements[j].isDeleted = true;

                printf("\nProduit %s avec ID %d a ete marque come supprimer.",nomP,id);

            }

            j++;
        }

        fseek(MemoireS.memoireS,(long)(-1 * sizeof(Bloc)),SEEK_CUR);

        fwrite(&buffer,sizeof(Buffer),1,MemoireS.memoireS);
        
        }

        }else{ //organisation chainée

        for(i=0;i<fichierinfo.taille_Blocs; i++){

            fread(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

            while(j<buffer.nbrE){

            if(buffer.enregistrements[j].id == id){

                memcpy(nomP,buffer.enregistrements[j].data,sizeof(nomP));

                buffer.enregistrements[j].isDeleted = true;

                printf("\nProduit %s avec ID %d a ete marque come supprimer.",nomP,id);

            }

            j++;
        }
        fseek(MemoireS.memoireS,(long)(-1 * sizeof(Bloc)),SEEK_CUR);

        fwrite(&buffer,sizeof(buffer),1,MemoireS.memoireS);

        fseek(MemoireS.memoireS,buffer.nextBloc*sizeof(Bloc),SEEK_SET);

        }

            
        }
    }
}

void SuppressionPhysqiue(MS MemoireS,char nom[50],int id){

    int niveau,adress,index;

    bool trouverFichier=false;

    MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouverFichier,&index,&adress,&niveau);//on cherche le fichier avec nom[50]

    if(trouverFichier == true){ //si on le trouve

    int adressBloc,adressEnreg;

    bool trouverEnreg=false;

    trouverEnreg = Recherche_Enregistrement(MemoireS,fichierinfo.nom,id,&adressBloc,&adressEnreg); //on cherche l'enregistrement avec id, si on le trouve on utilise adressBloc et adressEnrg

    if(trouverEnreg == true){


        Bloc buffer,buffer2;

        fseek(MemoireS.memoireS,adressBloc*sizeof(Bloc),SEEK_SET);

        if(fichierinfo.OrganisationE == 1){

        int blocsRestants = fichierinfo.taille_Blocs - (adressBloc - fichierinfo.adresse_PremierBloc); /*ce variable calcule le nombre de blocs restant de adress bloc vers adress de dernier bloc du fichier
                                                                                                    (valable juste dans l'organisation Contigue)*/
        int i=0,j;

        while(i<blocsRestants){

            fread(&buffer,sizeof(Buffer),1,MemoireS.memoireS);

            memset(&buffer.enregistrements[adressEnreg].id,0,sizeof(int));

            memset(buffer.enregistrements[adressEnreg].data,0,sizeof(buffer.enregistrements[adressEnreg].data)); // vider le data de l'enregistrement a supprimer

            buffer.enregistrements[adressEnreg].isDeleted = false; //re-initialiser le boolean isDeleted si l'enregistrement été supprimer logiquement.

            j=adressEnreg;

            while(j<buffer.nbrE - 1){ //décalage des enregistrements vers la gauche

                buffer.enregistrements[j] = buffer.enregistrements[j+1];

                j++;
            }

            if((i +  1) == blocsRestants){//si nous sommes dans le dernier bloc , on vide le dernier enregistrement été occupé aprés faire le décalage.

            memset(&buffer.enregistrements[buffer.nbrE - 1].id,0,sizeof(int));

            memset(buffer.enregistrements[buffer.nbrE - 1].data,0,sizeof(buffer.enregistrements[buffer.nbrE - 1].data));

            buffer.enregistrements[buffer.nbrE - 1].isDeleted = false;

            buffer.nbrE -= 1;

            fichierinfo.taille_Enrgs -= 1; //mise a jour la métadonné du fichier

            if(fichierinfo.taille_Blocs == 1){

            fseek(MemoireS.memoireS,(long)(-1 * sizeof(Bloc)),SEEK_CUR);

            }

            fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

            if(buffer.nbrE == 0){

                fichierinfo.taille_Blocs -= 1;

                Bloc MetaBloc;

                if(fichierinfo.taille_Blocs == 0){
                    

                    fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

                    fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                    SupprimerMetaD(&MetaBloc,niveau,adress);

                    fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                    fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                }else{

                fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

                fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adress);

                fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                }

                //mise a jour de la table d'allocation

                rewind(MemoireS.memoireS);

                fread(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

                buffer.enregistrements[adressBloc / data_size].data[adressBloc % data_size] = 0;

                fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

                printf("\nL'enregistrement avec ID %d a ete supprimer avec succes!",id);

                printf("\nLe fichier %s a ete supprimer car le dernier enregistrement de ce fichier a ete supprime!",fichierinfo.nom);


            }else{

                Bloc MetaBloc;

                fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

                fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adress);

                fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                printf("\nL'enregistrement avec ID %d a ete supprimer avec succes!",id);


            }



            }else{

            fread(&buffer2,sizeof(Bloc),1,MemoireS.memoireS);

            buffer.enregistrements[5] = buffer2.enregistrements[0]; /*toujours quand le bloc ou on fait le décalage n'est pas le dernier
                                                                    il est toujours plein nbrE = FB , et donc aprés en fait le décalage
                                                                    l'enregistrement dernier sera étre vide.*/

            fseek(MemoireS.memoireS,(long)(-2*sizeof(Bloc)),SEEK_CUR);

            fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

            buffer = buffer2;

            adressEnreg = 0;

            adressBloc += 1;




            }
            
            i++;


        }

        }else{//organisation chainne

        bool dernierbloc=false;

        int j;

        fread(&buffer,sizeof(Buffer),1,MemoireS.memoireS);

        while(buffer.nextBloc != -1 || dernierbloc == false){

        fseek(MemoireS.memoireS,adressBloc * sizeof(Bloc),SEEK_SET);

        if(buffer.nextBloc == -1){

            dernierbloc = true;

        }

            memset(&buffer.enregistrements[adressEnreg].id,0,sizeof(int));

            memset(buffer.enregistrements[adressEnreg].data,0,sizeof(buffer.enregistrements[adressEnreg].data)); // vider le data de l'enregistrement a supprimer

            buffer.enregistrements[adressEnreg].isDeleted = false; //re-initialiser le boolean isDeleted si l'enregistrement été supprimer logiquement.

            j=adressEnreg;

            while(j<buffer.nbrE - 1){ //décalage des enregistrements vers la gauche

                buffer.enregistrements[j] = buffer.enregistrements[j+1];

                j++;
            }

            if(dernierbloc == true){//si nous sommes dans le dernier bloc , on vide le dernier enregistrement été occupé aprés faire le décalage.

            memset(&buffer.enregistrements[buffer.nbrE - 1].id,0,sizeof(int));

            memset(buffer.enregistrements[buffer.nbrE - 1].data,0,sizeof(buffer.enregistrements[buffer.nbrE - 1].data));

            buffer.enregistrements[buffer.nbrE - 1].isDeleted = false;

            buffer.nbrE -= 1;

            fichierinfo.taille_Enrgs -= 1; //mise a jour de la métadonnée du fichier

            

            fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);


            if(buffer.nbrE == 0){

                fichierinfo.taille_Blocs -= 1;

                Bloc MetaBloc;

                if(fichierinfo.taille_Blocs == 0){
                    

                    fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

                    fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                    SupprimerMetaD(&MetaBloc,niveau,adress);

                    fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                    fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                }else{

                fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

                fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adress);

                fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                }

                //mise a jour de la table d'allocation

                rewind(MemoireS.memoireS);

                fread(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

                buffer.enregistrements[adressBloc / data_size].data[adressBloc % data_size] = 0;

                fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

                printf("\nL'enregistrement avec ID %d a ete supprimer avec succes!",id);

                printf("\nLe fichier %s a ete supprimer car le dernier enregistrement de ce fichier a ete supprime!",fichierinfo.nom);

            }else{

                Bloc MetaBloc;

                fseek(MemoireS.memoireS,index*sizeof(Bloc),SEEK_SET);

                fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adress);

                fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

                fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                printf("\nL'enregistrement avec ID %d a ete supprimer avec succes!",id);


            }

            
            }else{

            fseek(MemoireS.memoireS,buffer.nextBloc * sizeof(Bloc),SEEK_SET);

            fread(&buffer2,sizeof(Bloc),1,MemoireS.memoireS);

            buffer.enregistrements[5] = buffer2.enregistrements[0]; /*toujours quand le bloc ou on fait le décalage n'est pas le dernier
                                                                    il est toujours plein nbrE = FB , et donc aprés en fait le décalage
                                                                    l'enregistrement dernier sera étre vide.*/

            fseek(MemoireS.memoireS,adressBloc * sizeof(Bloc),SEEK_SET);

            fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

            adressBloc = buffer.nextBloc;


            buffer = buffer2;

            adressEnreg = 0;

            }

        }

        }

    }else{

        printf("\nImpossible de trouver Enregistrement avec id %d",id);

        return;
    }


    }else{

        printf("\nImpossible de trouver fichier avec nom %s, suppression annule",nom);
        return;
    }
    }

bool GES_insertion(MS MemoireS,enregistrement Enreg,char nom[50]){

    bool trouver;

    int niveau,adressM,index;

    MetaD fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adressM,&niveau);

    if(trouver == true){

    Buffer buffer,Bloc0,MetaBloc;

    if(fichierinfo.OrganisationE == 1){ //organisation contigue
    
    fseek(MemoireS.memoireS,( fichierinfo.adresse_PremierBloc + fichierinfo.taille_Blocs -1 ) * sizeof(Bloc),SEEK_SET); // on va vers le dernier Bloc du fichier.

    fread(&buffer,sizeof(buffer),1,MemoireS.memoireS); //on lire le dernier bloc

    if(buffer.nbrE >= FB){ //si le dernier bloc est plien --> Voire le next bloc si il est libre!

    rewind(MemoireS.memoireS);

    fread(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

    int adress = fichierinfo.adresse_PremierBloc + fichierinfo.taille_Blocs; //L'adress de Bloc situé aprés le dernier Bloc du fichier.

    int k = adress/data_size; //l'enregistrement[k] de table allocation ou l'état du Bloc[adress] est situé.

    int j = adress % data_size; //data[j] de l'enregistrement[k] ou l'état du Bloc[adress].

    if(Bloc0.enregistrements[k].data[j] == 0){ //le Bloc situé apres le dernier Bloc du fichier est libre

    //mise a jour la table d'allocation :

    Bloc0.enregistrements[k].data[j] = 1;

    Bloc0.nextBloc = Bloc0.nextBloc - 1;

    fseek(MemoireS.memoireS,(long)(-1 * sizeof(Bloc)),SEEK_CUR);

    fwrite(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

    if(fichierinfo.OrganisationI == 1){

        // le fichier est ordonné

        Buffer* fichierBlocs = (Bloc*)malloc(( fichierinfo.taille_Blocs + 1 )* sizeof(Bloc));

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc * sizeof(Bloc), SEEK_SET);

        fread(fichierBlocs,( fichierinfo.taille_Blocs )*sizeof(Bloc),1,MemoireS.memoireS);


        insererEnregistrement_Ordonne(fichierBlocs,fichierinfo.taille_Blocs + 1 ,Enreg);

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc * sizeof(Bloc), SEEK_SET);

        fwrite(fichierBlocs,(fichierinfo.taille_Blocs + 1)*sizeof(Bloc),1,MemoireS.memoireS);

    }else{

        fseek(MemoireS.memoireS,( fichierinfo.adresse_PremierBloc + fichierinfo.taille_Blocs ) * sizeof(Bloc),SEEK_SET);

        fread(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

                buffer.enregistrements[0].id = Enreg.id;

                strcpy(buffer.enregistrements[0].data, Enreg.data );

                buffer.nbrE++;
        
        fseek(MemoireS.memoireS,(long)(-1 * sizeof(Bloc)),SEEK_CUR);

        fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

    }

// mise a jour de meta-donne du fichier :

            fichierinfo.taille_Blocs += 1;
            
            fichierinfo.taille_Enrgs += 1;

            fseek(MemoireS.memoireS, index * sizeof(Bloc),SEEK_SET);

            fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

            EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adressM); /*NOTE : on n'a pas utiliser la fonction sauvgarder_MetaD() car elle est utilisé pour
            sauvgarder une nouvelle métadonné d'un nouveau fichier!*/

            fseek(MemoireS.memoireS, (long)(-1* sizeof(Bloc) ),SEEK_CUR);

            fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

            printf("\nyay");

            return true;

    }else{ //Le bloc situé apres le dernier Bloc du fichier n'est pas libre! --> recherche d'un nouvelle espace contigue de Taille_Blocs + 1.

            fichierinfo.taille_Blocs += 1;
            
            fichierinfo.taille_Enrgs += 1;

            bool etat;

            etat = GES_Creation(&fichierinfo,&Bloc0); //vérification l'existance d'un nouveau espace contigue.

            if(etat == true){

            fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc * sizeof(Bloc),SEEK_SET);

            Buffer* fichierBlocs = (Bloc*)malloc(fichierinfo.taille_Blocs * sizeof(Bloc));

            fread(fichierBlocs,( fichierinfo.taille_Blocs - 1 )*sizeof(Bloc),1,MemoireS.memoireS);

            SupprimerFichier(MemoireS,fichierinfo.nom); //on supprimer le fichier précédent.

            if(fichierinfo.OrganisationI == 1){ //si l'organisation est ordonné

            insererEnregistrement_Ordonne(fichierBlocs,fichierinfo.taille_Blocs,Enreg);

            }else{
                // si l'organisation n'est pas ordonné , on insére directement vers le dernier Bloc.

                fichierBlocs[fichierinfo.taille_Blocs - 1 ].enregistrements[0].id = Enreg.id;

                strcpy(fichierBlocs[fichierinfo.taille_Blocs - 1 ].enregistrements[0].data, Enreg.data );

                fichierBlocs[fichierinfo.taille_Blocs - 1 ].nbrE++;

            }

            AllouerFichier(MemoireS,fichierinfo,false,fichierBlocs);//on utilise allouer parceque on va inserer le fichier de maniére nouvelle avec un nouveau espace contigue.

            //mise-a-jour de la métadonné du fichier

            fseek(MemoireS.memoireS, index * sizeof(Bloc),SEEK_SET);

            fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

            EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adressM); /*NOTE : on n'a pas utiliser la fonction sauvgarder_MetaD() car elle est utilisé pour
            sauvgarder une nouvelle métadonné d'un nouveau fichier!*/

            fseek(MemoireS.memoireS, (long)(-1* sizeof(Bloc) ),SEEK_CUR);

            fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

            return true;

            }else{

                printf("\nErreur! , impossible d'inserer un noveau enregistrement dans le fichier %s (PAS D'ESPACE CONTIGUE)!",fichierinfo.nom);

                return false;
            }

    }

    }else{ //le dernier Bloc n'est pas plien !

    if(fichierinfo.OrganisationI == 1){ //si le fichier est Ordonné

    fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);//on va vers le premier bloc du fichier

    Buffer* fichierblocs = (Bloc*)malloc(fichierinfo.taille_Blocs*sizeof(Bloc));

    fread(fichierblocs,fichierinfo.taille_Blocs*sizeof(Bloc),1,MemoireS.memoireS);

    insererEnregistrement_Ordonne(fichierblocs,fichierinfo.taille_Blocs,Enreg);

    fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

    fwrite(fichierblocs,fichierinfo.taille_Blocs*sizeof(Bloc),1,MemoireS.memoireS);

    }else{// fichier n'est pas ordonné

    int size = buffer.nbrE;

    buffer.enregistrements[size].id = Enreg.id;

    strcpy(buffer.enregistrements[size].data, Enreg.data );

    buffer.nbrE++;

    fseek(MemoireS.memoireS,(long)(-1 * sizeof(Bloc)),SEEK_CUR);

    fwrite(&buffer,sizeof(Bloc),1,MemoireS.memoireS);

    }


    }

    //mise a jour la méta-donné :

    fichierinfo.taille_Enrgs += 1;

    fseek(MemoireS.memoireS, index * sizeof(Bloc),SEEK_SET);

        fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

            EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adressM); /*NOTE : on n'a pas utiliser la fonction sauvgarder_MetaD() car elle est utilisé pour
            sauvgarder une nouvelle métadonné d'un nouveau fichier!*/

            fseek(MemoireS.memoireS, (long)(-1* sizeof(Bloc) ),SEEK_CUR);

            fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

            return true;



    }else{ //Organisation chainé

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

        int adress;

        while(fread(&buffer,sizeof(buffer),1,MemoireS.memoireS) != 0 && buffer.nextBloc != -1){

        fseek(MemoireS.memoireS,buffer.nextBloc*sizeof(Bloc),SEEK_SET);

        adress = buffer.nextBloc; //on sauvgarder l'adress de dernier bloc pour faire le chainage aprés (dans le cas d'avoir un nouveau bloc).

        }

        if(buffer.nbrE >= FB){ // s'il n'ya pas un espace dans le dernier bloc pour ajouter l'enregistrement

        rewind(MemoireS.memoireS);

        fread(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

        if(Bloc0.nextBloc <= 0){

            printf("\nErreur! , pas d'espace libre pour inserer le nouvelle enregistrement dans fichier %s",fichierinfo.nom);

            return false;
        }else{

        if(fichierinfo.OrganisationI == 1){ //si le fichier est ordonné

        int pos=0;

        Buffer *fichierblocs = (Bloc*)malloc((fichierinfo.taille_Blocs + 1) * sizeof(Bloc));


        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);


        while(fread(&buffer,sizeof(buffer),1,MemoireS.memoireS) != 0 && buffer.nextBloc != -1){

        fichierblocs[pos]=buffer;

        fseek(MemoireS.memoireS,buffer.nextBloc*sizeof(Bloc),SEEK_SET);

        pos++;

        }

        insererEnregistrement_Ordonne(fichierblocs,fichierinfo.taille_Blocs + 1,Enreg);


        //il reste de trouver OU le dernier bloc sera étre et faire le chainage de avant dernier avec le dernier

                int k = adress / data_size;

                int j = adress % data_size + 1; //on cherche un nouveau bloc libre aprés le dernier bloc (c'est pour ça on fait + 1)

                int i = adress + 1;

                bool found = false;

                while(i<NbrBlocs && found == false){
                    
                    if(j>=data_size){
                        k++;
                        j=0;
                    }

                    if(Bloc0.enregistrements[k].data[j] == 0){

                        //mise ajour la table d'allocation

                        found = true;

                        Bloc0.enregistrements[k].data[j] = 1;

                        rewind(MemoireS.memoireS);

                        fwrite(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

                    }

                    j++;
                    i++;

                }

            fichierblocs[fichierinfo.taille_Blocs - 1].nextBloc = i - 1;
            
            fichierblocs[fichierinfo.taille_Blocs].nextBloc = -1;

            fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

            for(i=0; i<= fichierinfo.taille_Blocs; i++){

            fwrite(&fichierblocs[i],sizeof(Bloc),1,MemoireS.memoireS);

            fseek(MemoireS.memoireS,fichierblocs[i].nextBloc*sizeof(Bloc),SEEK_SET);

            }

        }else{ //fichier n'est pas ordonné




                int k = adress / data_size;

                int j = adress % data_size + 1; //on cherche un nouveau bloc libre aprés le dernier bloc (c'est pour ça on fait + 1)

                int i = adress+1;

                bool found = false;

                while(i<NbrBlocs && found == false){
                    
                    if(j>=data_size){
                        k++;
                        j=0;
                    }

                    if(Bloc0.enregistrements[k].data[j] == 0){

                        found = true;

                        Bloc bloc;

                        fseek(MemoireS.memoireS, adress * sizeof(Bloc), SEEK_SET);

                        fread(&bloc,sizeof(Bloc),1,MemoireS.memoireS);

                        bloc.nextBloc = i;

                        fseek(MemoireS.memoireS, (long)(-1*sizeof(Bloc)), SEEK_CUR);

                        fwrite(&bloc,sizeof(Bloc),1,MemoireS.memoireS);

                        Bloc nouveaubloc;
                        
                        fseek(MemoireS.memoireS, i * sizeof(Bloc),SEEK_SET);

                        fread(&nouveaubloc,sizeof(Bloc),1,MemoireS.memoireS);

                        strcpy(nouveaubloc.enregistrements[0].data, Enreg.data);

                        nouveaubloc.nbrE++;

                        fseek(MemoireS.memoireS, i * sizeof(Bloc),SEEK_SET);

                        fwrite(&nouveaubloc,sizeof(Bloc),1,MemoireS.memoireS);
                        
                        //mise ajour la table d'allocation

                        Bloc0.enregistrements[k].data[j] = 1;

                        rewind(MemoireS.memoireS);

                        fwrite(&Bloc0,sizeof(Bloc),1,MemoireS.memoireS);

                    }

                    j++;
                    i++;

                }


        }

                        //mise a jour de la métadonné :

                        fichierinfo.taille_Blocs += 1;

                        fichierinfo.taille_Enrgs += 1;

                        fseek(MemoireS.memoireS, index * sizeof(Bloc),SEEK_SET);

                        fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                        EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adressM); /*NOTE : on n'a pas utiliser la fonction sauvgarder_MetaD() car elle est utilisé pour
                        sauvgarder une nouvelle métadonné d'un nouveau fichier!*/

                        fseek(MemoireS.memoireS, (long)(-1* sizeof(Bloc) ),SEEK_CUR);

                        fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

                        return true;
        }

        }else{

            if(fichierinfo.OrganisationI == 1){


        Buffer *fichierblocs = (Bloc*)malloc((fichierinfo.taille_Blocs) * sizeof(Bloc));


        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);


        int i=0;


        while(fread(&buffer,sizeof(buffer),1,MemoireS.memoireS) != 0 && i<fichierinfo.taille_Blocs ){

        fichierblocs[i]=buffer;

        if(buffer.nextBloc != -1){

        fseek(MemoireS.memoireS,buffer.nextBloc*sizeof(Bloc),SEEK_SET);

        }
        i++;


        }


        insererEnregistrement_Ordonne(fichierblocs,fichierinfo.taille_Blocs,Enreg);
    
        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

            for(i=0; i< fichierinfo.taille_Blocs; i++){

            fwrite(&fichierblocs[i],sizeof(Bloc),1,MemoireS.memoireS);

            if(fichierblocs[i].nextBloc != -1){

            fseek(MemoireS.memoireS,fichierblocs[i].nextBloc*sizeof(Bloc),SEEK_SET);

            }
            }



            }else{ //fichier n'est pas ordonné ---> on insere dans le dernier Bloc

        strcpy(buffer.enregistrements[buffer.nbrE].data,Enreg.data); //inseré l'enregistrement.

        buffer.nbrE++;

        fseek(MemoireS.memoireS,(long)(-1*sizeof(Bloc)),SEEK_CUR);

        fwrite(&buffer,sizeof(Buffer),1,MemoireS.memoireS);


            }

        //mise a jour la méta-donné.
        
        fichierinfo.taille_Enrgs += 1;

        fseek(MemoireS.memoireS, index * sizeof(Bloc),SEEK_SET);

        fread(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

        EcrireMetaD_Enreg(&MetaBloc,fichierinfo,niveau,adressM); /*NOTE : on n'a pas utiliser la fonction sauvgarder_MetaD() car elle est utilisé pour
            sauvgarder une nouvelle métadonné d'un nouveau fichier!*/

        fseek(MemoireS.memoireS, (long)(-1* sizeof(Bloc) ),SEEK_CUR);

        fwrite(&MetaBloc,sizeof(Bloc),1,MemoireS.memoireS);

        return true;

        }


    }

    }else{

         printf("\nErreur *GES_insertion*, fichier n'existe pas.");

        return false;
    }
}



void Defragmenter(MS MemoireS,char nom[50]){

    bool trouver;

    int index , adress, niveau;
    
    MetaD fichierinfo;


    fichierinfo = RechercheMetaD(MemoireS,nom,&trouver,&index,&adress,&niveau);

    if(trouver == true){

        Bloc bloc;

    if(fichierinfo.OrganisationE == 1){

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

        int i=0,j;

        while(i<fichierinfo.taille_Blocs){

            fread(&bloc,sizeof(Bloc),1,MemoireS.memoireS);
            
            j=0;

            while(j<bloc.nbrE){

                
            if(bloc.enregistrements[j].isDeleted == true){

                printf("\nEnregistrement avec id %d a ete trouver logiquement supprimer --> suppression physique.",bloc.enregistrements[j].id);

                SuppressionPhysqiue(MemoireS,fichierinfo.nom,bloc.enregistrements[j].id);
            }

            j++;

            }

            i++;
            
        }


    }else{

        fseek(MemoireS.memoireS,fichierinfo.adresse_PremierBloc*sizeof(Bloc),SEEK_SET);

        int i=0,j;


        while(i<fichierinfo.taille_Blocs){

            fread(&bloc,sizeof(Bloc),1,MemoireS.memoireS);
            
            j=0;

            while(j<bloc.nbrE){

            if(bloc.enregistrements[j].isDeleted == true){

                printf("\nEnregistrement avec id %d a ete trouver logiquement supprimer --> suppression physique.",bloc.enregistrements[j].id);

                SuppressionPhysqiue(MemoireS,fichierinfo.nom,bloc.enregistrements[j].id);

            }

            j++;

            }

            if(bloc.nextBloc != -1){

                fseek(MemoireS.memoireS,bloc.nextBloc*sizeof(Bloc),SEEK_SET);

            }

            i++;
            
        }


    }

    printf("\nLe fichier %s a ete Defragmenter avec succes!",fichierinfo.nom);

    }else{

        printf("\nImpossible de trouver le fichier avec le nom %s",nom);
    }


}
