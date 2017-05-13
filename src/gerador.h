//
// Created by mateus on 11/05/17.
//

#ifndef SAUNA_GERADOR_H
#define SAUNA_GERADOR_H

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

typedef struct Gerador {
  int numPedidos;
  int maxUtilizacao;
  int pedidosGerados;
  int pedidosRecusados;
  int pedidosDescartados;
}Gerador;

/**
 * Retorna um caracter representando o sexo
 * @return 'M' ou 'F' com iguais chances
 */
char getRandomSex();

int getRandomDuracaoDeUtilizacao(int maxUtilizacao);

void *observarRejeitados(void *args);

void *gerarPedidos(void *args);

void gravarMensagemRegistro(Pedido pedido, char *status_pedido);

void startGerador();

#endif //SAUNA_GERADOR_H
