#include "sauna.h"

// Global variables
Sauna sauna;
FILE *registroFile;
struct timespec startTimespec;

int main(int argc, char const *argv[]) {
        int numLugares;

        // get program starting time
        clock_gettime(CLOCK_MONOTONIC, &startTimespec);

        // start sauna
        if(argc < 2 || argc > 2) {
                printf("Usage: sauna <n.lugares>\n");
                return 1;
        }

        if((numLugares = atoi(argv[1])) <= 0) {
                printf("<n.lugares> precisa ser um valor inteiro!\n");
                return 1;
        }

        // start file IO
        // create file path
        char registro[100];
        sprintf(registro, "tmp/bal.%d", getpid());
        if ((registroFile = fopen(registro, "a")) == NULL) {
                perror("Erro ao abrir ficheiro de registro");
                return 1;
        }

        // start sauna
        startSauna(numLugares);

        // fechar file
        fclose(registroFile);
        return 0;
}

void timespec_diff(struct timespec *start, struct timespec *stop, struct timespec *result){
        if ((stop->tv_nsec - start->tv_nsec) < 0) {
                result->tv_sec = stop->tv_sec - start->tv_sec - 1;
                result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
        } else {
                result->tv_sec = stop->tv_sec - start->tv_sec;
                result->tv_nsec = stop->tv_nsec - start->tv_nsec;
        }

        return;
}

void *adicionarASauna(void *args){
        Pedido *pedido = args;

        // registrar açao
        gravarMensagemRegistro(*pedido, "SERVIDO");

        printf("Aceito: %d %c %d\n", pedido->numSerie, pedido->genero, pedido->tempo);

        // segurar o thread por o tempo
        struct timespec sleepValue = {0};
        sleepValue.tv_nsec = pedido->tempo * NANO_SECOND_MULTIPLIER;
        nanosleep(&sleepValue, NULL);

        // remover da sauna
        sauna.numLugaresOcupados--;
        return NULL;
}

void gravarMensagemRegistro(Pedido pedido, char *status_pedido){
        // tempo atual
        struct timespec tempoAtual, result;
        clock_gettime(CLOCK_MONOTONIC, &tempoAtual);
        // get time difference
        timespec_diff(&startTimespec, &tempoAtual, &result);

        double millisecondsPassed = result.tv_sec * 1000 + result.tv_nsec/1000000;

        fprintf(registroFile, "%.2f – %d – %lu – %d: %c – %d – %s\n", millisecondsPassed, getpid(), pthread_self(), pedido.numSerie, pedido.genero, pedido.tempo, status_pedido);
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
                        //pthread_join(utilizadoresThreads[sauna.numLugaresOcupados], NULL);

                        sauna.numLugaresOcupados++;
                }
                // genero da pessoa e da sauna nao sao iguais
                else{
                        // devolver atraves do canal de rejeitados
                        rejeitarPedido(pedido, fdRejeitados);
                }

        }

        // esperar pedidos serem completados
        int i = 0;
        for (i = 0; i < sauna.numLugaresOcupados; i++) {
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

        // rejeitarPedid
        gravarMensagemRegistro(pedido, "REJEITADO");
}
