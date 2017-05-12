#ifndef CONSTANTS_H
#define  CONSTANTS_H

#define PATH_FIFO_SAUNA_ENTRADA "tmp/entrada"
#define PATH_FIFO_REJEITADOS "tmp/rejeitados"

typedef struct Pedido {
  int numSerie;
  char genero;
  int tempo;
}Pedido;

#endif
