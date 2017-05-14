#ifndef SAUNA_H
#define  SAUNA_H

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

#define REGISTER_PATH "/tmp/bal"

/**
 * Struct representa uma sauna
 */
typedef struct Sauna {
        int numLugaresOcupados;
        int numLugaresMax;
        int pedidosRecebidosM;
        int pedidosRecebidosF;
        int pedidosRecusadosM;
        int pedidosRecusadosF;
        int pedidosServidosM;
        int pedidosServidosF;
        char genero;
} Sauna;

/**
 * Começa o serviço da sauna
 * @param numLugares numero de lugares maximo da sauna
 */
void startSauna(int numLugares);

/**
 * Rejeita o pedido
 * @param pedido               Pedido a ser rejeitado
 * @param rejeitadosFicheiroFD File Descriptor do FIFO rejeitados
 */
void rejeitarPedido(Pedido pedido, int rejeitadosFicheiroFD);

/**
 * Grava uma mensagem no ficheiro de registro
 * @param pedido        Pedido a ser gravado
 * @param status_pedido O status do pedido a ser gravado
 */
void gravarMensagemRegistro(Pedido pedido, char *status_pedido);

/**
 * Adiciona o pedido a sauna
 * @param  args Ponteiro do predido a ser adicionado
 */
void *adicionarASauna(void *args);

#endif
