#ifndef SAUNA_H
#define  SAUNA_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"


typedef struct Sauna {
  int numLugaresOcupados;
  int numLugaresMax;
  char genero;
} Sauna;

void startSauna(int numLugares);

void rejeitarPedido(Pedido pedido, int rejeitadosFicheiroFD);


#endif
