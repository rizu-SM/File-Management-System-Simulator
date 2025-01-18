#ifndef FUNCTSFICHIERDONNES_H
#define FUNCTSFICHIERDONNES_H

#include "declarations.h"

#include "functsFichierMeta.h"

#include "functsmemoire.h"

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <string.h>

#include <stdbool.h>

#include <time.h>

void AllouerFichier(MS MemoireS,MetaD fichierinfo,bool nouveau,Buffer* fichier);

void SupprimerFichier(MS MemoireS, char nom[50]);

bool Recherche_Enregistrement(MS MemoireS,char nom[50],int id,int* adressBloc,int*adressEnreg);

void LireFichier(MS MemoireS,char nom[50]);

void RenommerFichier(MS MemoireS,char nom[50],char nouveauNom[50]);

void SuppressionLogique(MS MemoireS,char nom[50],int id);

void SuppressionPhysqiue(MS MemoireS,char nom[50],int id);

bool GES_insertion(MS MemoireS,enregistrement Enreg,char nom[50]);

void insererEnregistrement_Ordonne(Bloc fichierblocs[],int size,enregistrement enreg);

void Defragmenter(MS MemoireS,char nom[50]);

#endif
