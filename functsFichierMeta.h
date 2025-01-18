#ifndef FUNCTSFICHIERMETA_H
#define FUNCTSFICHIERMETA_H

#include "declarations.h"

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <string.h>

#include <math.h>

MetaD CreeFichier(MS MemoireS);

void EcrireMetaD_Enreg(Bloc *MetaBloc, MetaD fichierinfo, int niveau,int adress);

void SupprimerMetaD(Bloc *MetaBloc, int niveau, int adress);

MetaD LireMetaD_Enreg(Buffer MetaBloc,int niveau , int adress);

void sauvgarder_MetaD(MS MemoireS, MetaD fichierinfo,Buffer MetaBloc);

MetaD RechercheMetaD(MS MemoireS,char nom[50],bool* trouver,int* index,int *adress, int* niveau);

void Affichier_Fichiers_Meta(MS MemoireS);


#endif
