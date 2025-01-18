#ifndef FUNCTSMEMOIRE_H
#define FUNCTSMEMOIRE_H

#include "declarations.h"

#include "functsFichierMeta.h"

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <math.h>

#include <string.h>


void InitialiseDisk(MS* MemoireS);

void vider_MS(MS *ms);

bool GES_Creation(MetaD *fichierinfo,Bloc *Bloc0);

Bloc LireBloc0(MS MemoireS);

#endif
