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

/**
   @param numPedidos é o número total de pedidos gerados ao longo da execução do programa
   @param maxUtilizacao  é o tempo máximo de duração de uma utilização da sauna, em milisegundos
*/
void startGerador(int numPedidos, int maxUtilizacao);

/**
 * Retorna um caracter representando o sexo
 * @return 'M' ou 'F' com iguais chances
 */
char getRandomSex();


int getRandomDuracaoDeUtilizacao(int maxUtilizacao);

#endif //SAUNA_GERADOR_H
