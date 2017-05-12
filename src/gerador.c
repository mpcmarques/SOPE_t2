//
// Created by mateus on 11/05/17.
//
#include "gerador.h"

#define TIMEOUT 1000

int main(int argc, char *argv[]) {

        Gerador *gerador = (Gerador*)malloc(sizeof(Gerador));
        gerador->maxUtilizacao = 1000;
        gerador->numPedidos = 10;

        // criar thread que vai gerar pedidos
        pthread_t geradorThread;

        printf("criando thread..\n");
        if(pthread_create(&geradorThread, NULL, gerarPedidos, gerador) != 0) {
                // error handler
                perror("erro criando thread");
                free(gerador);
        }

        pthread_exit(NULL);
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

char getRandomSex(){
        // generate new random seed
        srand(time(NULL));

        int random = rand()%2;
        if (random == 0) {
                return 'M';
        } else {
                return 'F';
        }
}

int getRandomDuracaoDeUtilizacao(int maxUtilizacao){
        int random = rand()%maxUtilizacao+1;
        return random;
}

void *gerarPedidos(void *args){
        printf("gerando pedidos..\n");
        Gerador *gerador = args;
        int pedidosCount = 0; // track numero pedidos

        // criar fifo
        if(mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                perror("erro criando fifo");
        }

        //  abrir fifo entrada da sauna
        int fd;
        if ((fd = open(PATH_FIFO_SAUNA_ENTRADA, O_WRONLY)) < 0) {
                perror("error path fifo");
                return NULL;
        }

        // gerar pedidos
        while(pedidosCount < gerador->numPedidos) {
                // gerar pedido
                Pedido pedido = {pedidosCount, getRandomSex(), getRandomDuracaoDeUtilizacao(gerador->maxUtilizacao), 0};

                // contacta o programa que gere a sauna atraves de um canal com nome /tmp/entrada
                write(fd, &pedido, sizeof(pedido));
                sleep(1);
                pedidosCount++;
        }

        // fechar ficheiro
        close(fd);
        free(gerador);
        // remover fifo
        unlink(PATH_FIFO_SAUNA_ENTRADA);
        return 0;
}

void observarRejeitados(){
        Pedido pedido;

        //  abrir fifo entrada da sauna
        int fd;
        if ((fd = open(PATH_FIFO_REJEITADOS, O_RDONLY)) < 0) {
                perror("observarRejeitados: erro ao abrir fifo");
                return;
        }

        while(read(fd, &pedido, sizeof(pedido)) > 0) {
                // descarta se o numero de rejeicao e 3
                if (pedido.numRejeicao == 3) {
                        // descarta pedido
                }
                // tenta novamente se e menor que isso
                else {
                        // tenta enviar o pedido novamente
                }
        }

        // fechar descritor
        close(fd);

}
