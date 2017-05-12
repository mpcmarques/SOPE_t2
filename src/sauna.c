#include "sauna.h"

int main(int argc, char const *argv[]) {
        // start sauna
        startSauna(20);
        return 0;
}

void startSauna(int numLugares){
        int fd;
        Pedido pedido;
        Sauna sauna = {0, numLugares, 'N'};

        if ((fd = open(PATH_FIFO_SAUNA_ENTRADA, O_RDONLY)) < 0) {
                perror("error reading fifo sauna entrada");
                return;
        }

        while(read(fd, &pedido, sizeof(pedido)) > 0) {

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
        close(fd);
}

void rejeitarPedido(Pedido pedido){
        int pedidosCount = 0; // track numero pedidos

        // criar fifo
        mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO);

        //  abrir fifo entrada da sauna
        int fd;
        if ((fd = open(PATH_FIFO_SAUNA_ENTRADA, O_WRONLY)) < 0) {
                perror("error path fifo");
                return;
        }

        // mudar rejeicao do pedido
        pedido.numRejeicao += 1;

        // escreve pedido rejeitado em /tmp/rejeitados
        write(fd, &pedido, sizeof(pedido));
        sleep(3);
        pedidosCount++;

        // fechar ficheiro
        close(fd);
        unlink(PATH_FIFO_SAUNA_ENTRADA);
}
