#include "sauna.h"

// Global variables
Sauna sauna;

int main(int argc, char const *argv[]) {
        int numLugares;

        // start sauna
        if(argc < 2 || argc > 2) {
                printf("Usage: sauna <n.lugares>\n");
                return 1;
        }

        if((numLugares = atoi(argv[1])) <= 0) {
                printf("<n.lugares> precisa ser um valor inteiro!\n");
        }

        startSauna(numLugares);
        return 0;
}

void *adicionarASauna(void *args){
        Pedido *pedido = args;
        clock_t begin;
        double tempo_gasto;

        printf("Aceito: %d %c %d\n", pedido->numSerie, pedido->genero, pedido->tempo);

        // marca o começo do tempo
        begin = clock();
        // segurar o thread por o tempo
        while(((tempo_gasto = ((clock() - begin) / CLOCKS_PER_SEC)) * 1000 ) > pedido->tempo) {
                // seg
        }
        // remover da sauna
        sauna.numLugaresOcupados--;
        return NULL;
}

void startSauna(int numLugares){
        int fdSauna, fdRejeitados;
        Pedido pedido;

        // setup sauna
        sauna.genero = 'N';
        sauna.numLugaresMax = numLugares;
        sauna.numLugaresOcupados = 0;

        pthread_t utilizadoresThreads[numLugares];

        // criar fifo rejeitados
        if(mkfifo(PATH_FIFO_REJEITADOS, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                perror("erro criando fifo rejeitados");
        }

        // abrir sauna entrada para leitura
        while((fdSauna = open(PATH_FIFO_SAUNA_ENTRADA, O_RDONLY)) < 0) {
                sleep(1);
        }

        // abrir rejeitados para escrita
        if ((fdRejeitados = open(PATH_FIFO_REJEITADOS, O_WRONLY)) < 0) {
                perror("error path fifo");
                return;
        }

        // ler entrada da sauna
        while(read(fdSauna, &pedido, sizeof(pedido)) > 0) {

                // Definir genero da sauna caso nao tenha sauna
                if (sauna.genero == 'N') {
                        // definir sauna como genero inicial
                        sauna.genero = pedido.genero;
                }

                // sauna possui genero
                // ver se o genero do pedido e da sauna sao iguais
                if(pedido.genero == sauna.genero && sauna.numLugaresOcupados < sauna.numLugaresMax) {
                        // adicionar pessoa a sauna
                        // criar thread da utilizacao da sauna
                        pthread_create(&utilizadoresThreads[sauna.numLugaresOcupados], NULL, adicionarASauna, &pedido);

                        sauna.numLugaresOcupados++;
                }
                // genero da pessoa e da sauna nao sao iguais
                else{
                        // devolver atraves do canal de rejeitados
                        rejeitarPedido(pedido, fdRejeitados);
                }

        }

        // esperar processos de utilizacao terminar
        int i;
        for(i = 0; i < sauna.numLugaresMax; i++){
          pthread_join(utilizadoresThreads[i], NULL);
        }

        // fechar descritor
        close(fdSauna);
        close(fdRejeitados);
        // destruir fifo rejeitados
        unlink(PATH_FIFO_REJEITADOS);
}

void rejeitarPedido(Pedido pedido, int fd){
        // mudar rejeicao do pedido
        pedido.numRejeicao += 1;

        // escreve pedido rejeitado em /tmp/rejeitados
        write(fd, &pedido, sizeof(pedido));
}
