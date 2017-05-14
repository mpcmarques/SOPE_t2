//
// Created by mateus on 11/05/17.
//
#include "gerador.h"

// Global variables
int saunaEntradaFD, rejeitadosFD; //  File descriptor
Gerador gerador;
struct timespec startTimespec;
FILE *registroFile;

pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER; /* mutex para a sec.crít. */

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
        fprintf(registroFile, "HOMENS: Gerados: %d Recusados: %d Descartados: %d\n",
                gerador.pedidosGeradosM,
                gerador.pedidosRecusadosM,
                gerador.pedidosDescartadosM);

        fprintf(registroFile, "MULHERES: Gerados: %d Recusados: %d Descartados: %d\n",
                gerador.pedidosGeradosF,
                gerador.pedidosRecusadosF,
                gerador.pedidosDescartadosF);

        fprintf(registroFile, "TOTAL: Gerados: %d Recusados: %d Descartados: %d\n",
                gerador.pedidosGeradosF + gerador.pedidosGeradosM,
                gerador.pedidosRecusadosF + gerador.pedidosRecusadosM,
                gerador.pedidosDescartadosF + gerador.pedidosDescartadosM);

        // fechar file
        fclose(registroFile);

        return 0;
}

void startGerador(int numPedidos, int maxUtilizacao){
        // setup gerador
        gerador.maxUtilizacao = maxUtilizacao;
        gerador.numPedidos = numPedidos;
        gerador.pedidosGeradosM = 0;
        gerador.pedidosDescartadosM = 0;
        gerador.pedidosRecusadosM = 0;
        gerador.pedidosGeradosF = 0;
        gerador.pedidosDescartadosF = 0;
        gerador.pedidosRecusadosF = 0;


        //  abrir fifo entrada da sauna
        while((saunaEntradaFD = open(PATH_FIFO_SAUNA_ENTRADA, O_WRONLY | O_NONBLOCK)) < 0) {
                printf("Gerador: Aguardando ligação com a lista de pedidos da sauna..\n");
                sleep(1);
        }

        //  abrir fifo rejeitados
        while((rejeitadosFD = open(PATH_FIFO_REJEITADOS, O_RDONLY)) < 0) {
                printf("Gerador: Aguardando ligação com a lista de rejeitados da sauna..\n");
                sleep(1);
        }

        // criar threads
        pthread_t geradorThread, observadorRejeitadosThread;

        if(pthread_create(&geradorThread, NULL, gerarPedidos, NULL) != 0) {
                // error handler
                perror("Gerador: erro criando thread geradorThread");
                return;
        }

        if(pthread_create(&observadorRejeitadosThread, NULL, observarRejeitados, NULL) != 0) {
                // error handler
                perror("Gerador: erro criando thread observarRejeitados");
                return;
        }

        pthread_join(geradorThread, NULL);
        pthread_join(observadorRejeitadosThread, NULL);

        // fechar fifo
        close(rejeitadosFD);
        printf("Gerador: terminou!\n");
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

        // gerar pedidos
        while(pedidosCount < gerador.numPedidos) {
                pthread_mutex_lock(&mut); // lock thread

                // gerar pedido
                Pedido pedido = {pedidosCount, getRandomSex(), getRandomDuracaoDeUtilizacao(gerador.maxUtilizacao), 0};

                //  adicionar um pedido gerado
                gravarMensagemRegistro(pedido, "PEDIDO");

                if (pedido.genero == 'M') {
                        gerador.pedidosGeradosM++;
                }else {
                        gerador.pedidosGeradosF++;
                }

                // contacta o programa que gere a sauna atraves de um canal com nome /tmp/entrada
                write(saunaEntradaFD, &pedido, sizeof(pedido));
                pedidosCount++;

                pthread_mutex_unlock(&mut); // unlock thread

                sleep(2);
        }

        // fechar FIFOS
        close(saunaEntradaFD);

        return NULL;
}

void *observarRejeitados(void *args){
        Pedido pedido;

        while(read(rejeitadosFD, &pedido, sizeof(pedido)) > 0) {
                pthread_mutex_lock(&mut); // lock thread
                // re-aproveita se o pedido foi rejeitado menos de 3 vezes
                if (pedido.numRejeicao < 3) {

                        if (pedido.genero == 'M') {
                                gerador.pedidosRecusadosM++;
                        }else {
                                gerador.pedidosRecusadosF++;
                        }
                        gravarMensagemRegistro(pedido, "RECUSADO");

                        // tenta enviar pedido novamente
                        write(saunaEntradaFD, &pedido, sizeof(pedido));
                } else {
                        if (pedido.genero == 'M') {
                                gerador.pedidosDescartadosM++;
                        }else {
                                gerador.pedidosDescartadosF++;
                        }
                        gravarMensagemRegistro(pedido, "DESCARTADO");
                }
                pthread_mutex_unlock(&mut); // unlock thread

                sleep(1);
        }

        return NULL;
}
