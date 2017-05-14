#include "sauna.h"

// Global variables
Sauna sauna;
FILE *registroFile;
struct timespec startTimespec;

pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER; /* mutex para a sec.crít. */

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

        // escrever ultima linha da sauna
        fprintf(registroFile, "HOMENS : Recebidos: %d Servidos: %d Recusados: %d\n",
                sauna.pedidosRecebidosM,
                sauna.pedidosServidosM,
                sauna.pedidosRecusadosM);

        fprintf(registroFile, "MULHERES : Recebidos: %d Servidos: %d Recusados: %d\n",
                sauna.pedidosRecebidosF,
                sauna.pedidosServidosF,
                sauna.pedidosRecusadosF);

        fprintf(registroFile, "TOTAL : Recebidos: %d Servidos: %d Recusados: %d\n",
                sauna.pedidosRecebidosM + sauna.pedidosRecebidosF,
                sauna.pedidosServidosM + sauna.pedidosServidosF,
                sauna.pedidosRecusadosM + sauna.pedidosRecusadosF);

        // fechar file
        fclose(registroFile);

        printf("Sauna: terminou! \n");
        return 0;
}

void *adicionarASauna(void *args){
        pthread_mutex_lock(&mut); // lock thread

        Pedido *pedido = args;

        // registrar açao
        gravarMensagemRegistro(*pedido, "SERVIDO");

        pthread_mutex_unlock(&mut); // unlock mutex

        // segurar o thread por o tempo determinado
        msleep(&pedido->tempo);

        pthread_mutex_lock(&mut); // lock thread

        // remover da sauna
        sauna.numLugaresOcupados--;

        pthread_mutex_unlock(&mut); // unlock mutex

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
        sauna.pedidosRecebidosM = 0;
        sauna.pedidosRecebidosF = 0;
        sauna.pedidosRecusadosM = 0;
        sauna.pedidosServidosM = 0;
        sauna.pedidosServidosF = 0;

        pthread_t utilizadoresThreads[numLugares];

        // criar fifo rejeitados
        if(mkfifo(PATH_FIFO_REJEITADOS, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                perror("erro ao criar fifo rejeitados! ");
                return;
        }

        // criar fifosauna entrada
        if(mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
                perror("erro ao criar fifo sauna entrada! ");
                return;
        }

        printf("Sauna: sauna iniciada, esperando ligação com gerador..\n");

        // abrir sauna entrada para leitura
        while((fdSauna = open(PATH_FIFO_SAUNA_ENTRADA, O_RDONLY)) < 0) {
                printf("Sauna: aguardando lista de pedidos da sauna..\n");
                sleep(1);
        }

        // abrir rejeitados para escrita
        while((fdRejeitados = open(PATH_FIFO_REJEITADOS, O_WRONLY | O_NONBLOCK)) < 0) {
                printf("Sauna: aguardando lista de recusados..\n");
                sleep(1);
        }

        // ler entrada da sauna
        while(read(fdSauna, &pedido, sizeof(pedido)) > 0) {
                pthread_mutex_lock(&mut); // lock thread

                if(pedido.genero == 'M') {
                        sauna.pedidosRecebidosM++;
                } else {
                        sauna.pedidosRecebidosF++;
                }

                // Definir genero da sauna caso nao tenha sauna
                if (sauna.genero == 'N' || sauna.numLugaresOcupados == 0) {
                        // definir sauna como genero inicial
                        sauna.genero = pedido.genero;
                }

                // sauna possui genero
                // ver se o genero do pedido e da sauna sao iguais
                if(pedido.genero == sauna.genero && sauna.numLugaresOcupados < sauna.numLugaresMax) {
                        printf("Aceito: %d %c %d\n", pedido.numSerie, pedido.genero, pedido.tempo);
                        // criar thread da utilizacao da sauna
                        pthread_create(&utilizadoresThreads[sauna.numLugaresOcupados], NULL, adicionarASauna, &pedido);

                        // adicionar pessoa a sauna
                        if(pedido.genero == 'M') {
                                sauna.pedidosServidosM++;
                        } else {
                                sauna.pedidosServidosF++;
                        }
                        sauna.numLugaresOcupados++;
                }
                // genero da pessoa e da sauna nao sao iguais
                else{
                        printf("Rejeitado: %d %c %d %d\n", pedido.numSerie, pedido.genero, pedido.tempo, pedido.numRejeicao);
                        if(pedido.genero == 'M') {
                                sauna.pedidosRecusadosM++;
                        } else {
                                sauna.pedidosRecusadosF++;
                        }
                        // devolver atraves do canal de rejeitados
                        rejeitarPedido(pedido, fdRejeitados);
                }

                pthread_mutex_unlock(&mut); // unlock thread
        }

        // esperar pedidos serem completados
        int i = 0;
        for (i = 0; i < sauna.numLugaresOcupados; i++) {
                pthread_join(utilizadoresThreads[i], NULL);
        };

        // fechar descritor
        close(fdSauna);
        close(fdRejeitados);
        // destruir fifo rejeitados
        unlink(PATH_FIFO_REJEITADOS);
        unlink(PATH_FIFO_SAUNA_ENTRADA);
}

void rejeitarPedido(Pedido pedido, int fd){
        // mudar rejeicao do pedido
        pedido.numRejeicao += 1;

        // escreve pedido rejeitado em /tmp/rejeitados
        write(fd, &pedido, sizeof(pedido));

        // rejeitarPedid
        gravarMensagemRegistro(pedido, "REJEITADO");
}
