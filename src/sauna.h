#ifndef SAUNA_H
#define  SAUNA_H

#define _POSIX_C_SOURCE 199309L

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include <unistd.h>
#include <syscall.h>

#define REGISTER_PATH "/tmp/bal"

typedef struct Sauna {
        int numLugaresOcupados;
        int numLugaresMax;
        char genero;
} Sauna;

void startSauna(int numLugares);

void rejeitarPedido(Pedido pedido, int rejeitadosFicheiroFD);

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result);

void gravarMensagemRegistro(Pedido pedido, char *status_pedido);

#endif
