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

/**
 * Struct representando o gerador de pedidos
 */
typedef struct Gerador {
  int numPedidos;
  int maxUtilizacao;
  int pedidosGeradosM;
  int pedidosGeradosF;
  int pedidosRecusadosM;
  int pedidosRecusadosF;
  int pedidosDescartadosM;
  int pedidosDescartadosF;
}Gerador;

/**
 * Retorna um caracter representando o sexo
 * @return 'M' ou 'F' com iguais chances
 */
char getRandomSex();

/**
 * Retorna um valor entre 0 e o maximo de milisegundos de utilizaçao
 * @param  maxUtilizacao Tempo de utilização maximo em milisegundos
 * @return               Valor entre 0 e o tempo máximo
 */
int getRandomDuracaoDeUtilizacao(int maxUtilizacao);

/**
 * Lida com as rejeições
 */
void *observarRejeitados(void *args);

/**
 * Gera pedidos
 */
void *gerarPedidos(void *args);

/**
 * Grava uma mensagem no ficheiro de registro
 * @param pedido        Pedido a ser gravado
 * @param status_pedido Status do pedido a ser gravado
 */
void gravarMensagemRegistro(Pedido pedido, char *status_pedido);

/**
 * Inicia o gerador
 */
void startGerador();

#endif //SAUNA_GERADOR_H
