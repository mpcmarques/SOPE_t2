//
// Created by mateus on 11/05/17.
//

#ifndef SAUNA_GERADOR_H
#define SAUNA_GERADOR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "constants.h"
#include <pthread.h>

typedef struct Gerador {
  int numPedidos;
  int maxUtilizacao;
}Gerador;

/**
 * Retorna um caracter representando o sexo
 * @return 'M' ou 'F' com iguais chances
 */
char getRandomSex();


int getRandomDuracaoDeUtilizacao(int maxUtilizacao);

void *gerarPedidos(void *args);

#endif //SAUNA_GERADOR_H
