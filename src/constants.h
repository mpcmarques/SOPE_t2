#ifndef CONSTANTS_H
#define  CONSTANTS_H

#define PATH_FIFO_SAUNA_ENTRADA "tmp/entrada"
#define PATH_FIFO_REJEITADOS "tmp/rejeitados"


#define NANO_SECOND_MULTIPLIER  1000000

typedef struct Pedido {
  int numSerie;
  char genero;
  int tempo;
  int numRejeicao;
}Pedido;

#endif
