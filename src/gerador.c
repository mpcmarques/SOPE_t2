//
// Created by mateus on 11/05/17.
//
#include "gerador.h"

#define TIMEOUT 1000

int main(int argc, char *argv[]) {

        startGerador(5, 1000);
        return 0;
}

/*
 * E um programa multithread em que um thread efectua a geraçao aleatoria de pedidos e os apresenta a sauna
 * Outro thread escuta os pedidos rejeitados e os recoloca na fila de pedidos, mas so caso o numero de um dado
 * pedido nao exceder 3, caso exceda, descarta o pedido!
 * */

/*
 * Durante toda a operação, o programa gerador emite mensagens de registo, para um ficheiro com o
 * nome /tmp/ger.pid
 */

void startGerador(int numPedidos, int maxUtilizacao){

        int pedidosCount = 0; // track numero pedidos

        // make fifos
        mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO);
        mkfifo(PATH_FIFO_REJEITADOS, S_IRWXU | S_IRWXG | S_IRWXO);

        //  abrir fifo entrada da sauna
        int fd;
        if ((fd = open(PATH_FIFO_SAUNA_ENTRADA, O_WRONLY)) < 0) {
                perror("error path fifo");
                return;
        }

        // gerar pedidos
        char message[1024];
        while(pedidosCount < numPedidos) {
                // gerar pedido
                sprintf(message, "%d %c %d\n", pedidosCount, 'M', 12);

                // contacta o programa que gere a sauna atraves de um canal com nome /tmp/entrada
                write(fd, message, strlen(message)+1);
                sleep(3);
                pedidosCount++;
        }
        // fechar ficheiro
        close(fd);
        // deletar fifos
        unlink(PATH_FIFO_SAUNA_ENTRADA);
        unlink(PATH_FIFO_REJEITADOS);
}
