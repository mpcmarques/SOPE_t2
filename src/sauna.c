#include "sauna.h"

int main(int argc, char const *argv[]) {
        // start sauna
        startSauna(20);
        return 0;
}

void startSauna(int numLugares){
        int fdSauna, fdRejeitados;
        Pedido pedido;
        Sauna sauna = {0, numLugares, 'N'};

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
                if(pedido.genero == sauna.genero) {
                        // adicionar pessoa a sauna
                        printf("Aceito: %d %c %d\n", pedido.numSerie, pedido.genero, pedido.tempo);
                }
                // genero da pessoa e da sauna nao sao iguais
                else{
                        // devolver atraves do canal de rejeitados
                        printf("Rejeitado: %d %c %d\n", pedido.numSerie, pedido.genero, pedido.tempo);
                }

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
        sleep(3);
}
