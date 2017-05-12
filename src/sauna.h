#ifndef SAUNA_H
#define  SAUNA_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include "constants.h"

typedef struct Sauna {
  int numLugaresOcupados;
  int numLugaresMax;
  char genero;
} Sauna;

void startSauna(int numLugares);


#endif
