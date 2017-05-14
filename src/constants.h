#ifndef CONSTANTS_H
#define  CONSTANTS_H

#define PATH_FIFO_SAUNA_ENTRADA "tmp/entrada"
#define PATH_FIFO_REJEITADOS "tmp/rejeitados"


#include <time.h>

/**
 * Struct represta um pedido da sauna
 */
typedef struct Pedido {
        int numSerie;
        char genero;
        int tempo;
        int numRejeicao;
}Pedido;

/**
 * Calcula a diferença de tempo entre dois timespecs
 * @param start  Timespec tempo inicial
 * @param stop   Timespec tempo final
 * @param result Timespec resultado
 */
void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result);

/**
 * Sleep in milliseconds
 * @param milliseconds milliseconds to sleep
 */
void msleep(int *milliseconds);

#endif
