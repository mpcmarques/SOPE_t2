//
// Created by mateus on 11/05/17.
//
#include "gerador.h"

// Global variables
int saunaEntradaFD; //  File descriptor
Gerador gerador;
struct timespec startTimespec;
FILE *registroFile;

int main(int argc, char *argv[]) {
  int numGerar; int maxUtilizacao;
        if (argc < 3 || argc > 3 ) {
                printf("Usage:gerador <n. pedidos> <max. utilização> \n");
                return 1;
        }

        if((numGerar = atoi(argv[1])) <= 0) {
                printf("<n.pedidos> precisa ser um valor inteiro!\n");
                return 1;
        }

        if((maxUtilizacao = atoi(argv[2])) <= 0) {
                printf("<max.utilizacao> precisa ser um valor inteiro!\n");
                return 1;
        }

        // get program starting time
        clock_gettime(CLOCK_MONOTONIC, &startTimespec);

        // generate new random seed
        srand(time(NULL));

        // start file IO
        // create file path
        char registro[100];
        sprintf(registro, "tmp/ger.%d", getpid());
        if ((registroFile = fopen(registro, "a")) == NULL) {
                perror("Erro ao abrir ficheiro de registro");
                return 1;
        }

        // start gerador
        startGerador(numGerar, maxUtilizacao);

        // escrever ultima linha do gerador
        fprintf(registroFile, "Gerados: %d Recusados: %d Descartados: %d\n", gerador.pedidosGerados, gerador.pedidosRecusados, gerador.pedidosDescartados);

        // fechar file
        fclose(registroFile);

        return 0;
}

void startGerador(int numPedidos, int maxUtilizacao){
        // setup gerador
        gerador.maxUtilizacao = maxUtilizacao;
        gerador.numPedidos = numPedidos;
        gerador.pedidosGerados = 0;
        gerador.pedidosDescartados = 0;
        gerador.pedidosRecusados = 0;

        // criar threads
        pthread_t geradorThread, observadorRejeitadosThread;

        if(pthread_create(&geradorThread, NULL, gerarPedidos, NULL) != 0) {
                // error handler
                perror("erro criando thread geradorThread");
        }

        if(pthread_create(&observadorRejeitadosThread, NULL, observarRejeitados, NULL) != 0) {
                // error handler
                perror("erro criando thread observarRejeitados");
        }

        pthread_join(geradorThread, NULL);
        pthread_join(observadorRejeitadosThread, NULL);
}

void gravarMensagemRegistro(Pedido pedido, char *status_pedido){
        // tempo atual
        struct timespec tempoAtual, result;
        clock_gettime(CLOCK_MONOTONIC, &tempoAtual);
        // get time difference
        timespec_diff(&startTimespec, &tempoAtual, &result);

        double millisecondsPassed = result.tv_sec * 1000 + result.tv_nsec/1000000;

        fprintf(registroFile, "%.2f – %d – %d: %c – %d – %s\n", millisecondsPassed, getpid(), pedido.numSerie, pedido.genero, pedido.tempo, status_pedido);
}

char getRandomSex(){
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
        int pedidosCount = 0; // track numero pedidos

        // criar fifo
        if(mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                perror("erro criando fifo");
        }

        //  abrir fifo entrada da sauna
        if ((saunaEntradaFD = open(PATH_FIFO_SAUNA_ENTRADA, O_WRONLY)) < 0) {
                perror("error path fifo");
                return NULL;
        }

        // gerar pedidos
        while(pedidosCount < gerador.numPedidos) {
                // gerar pedido
                Pedido pedido = {pedidosCount, getRandomSex(), getRandomDuracaoDeUtilizacao(gerador.maxUtilizacao), 0};

                //  adicionar um pedido gerado
                gravarMensagemRegistro(pedido, "PEDIDO");
                gerador.pedidosGerados++;

                // contacta o programa que gere a sauna atraves de um canal com nome /tmp/entrada
                write(saunaEntradaFD, &pedido, sizeof(pedido));
                sleep(1);
                pedidosCount++;
        }

        // fechar ficheiro
        close(saunaEntradaFD);
        // remover fifo
        unlink(PATH_FIFO_SAUNA_ENTRADA);
        return 0;
}

void *observarRejeitados(void *args){
        Pedido pedido;

        //  abrir fifo entrada da sauna
        int fd;
        while((fd = open(PATH_FIFO_REJEITADOS, O_RDONLY)) < 0) {
                sleep(1);
        }

        while(read(fd, &pedido, sizeof(pedido)) > 0) {

                // re-aproveita se o pedido foi rejeitado menos de 3 vezes
                if (pedido.numRejeicao < 3) {
                        printf("Rejeitado: %d %c %d %d\n", pedido.numSerie, pedido.genero, pedido.tempo, pedido.numRejeicao);
                        gerador.pedidosRecusados++;
                        gravarMensagemRegistro(pedido, "RECUSADO");

                        // tenta enviar pedido novamente
                        write(saunaEntradaFD, &pedido, sizeof(pedido));
                        sleep(1);
                } else {
                        gerador.pedidosDescartados++;
                        gravarMensagemRegistro(pedido, "DESCARTADO");
                }
        }

        // fechar descritor
        close(fd);
        return 0;
}
