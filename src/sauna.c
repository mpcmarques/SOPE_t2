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

        // make fifos
        mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO);
        mkfifo(PATH_FIFO_REJEITADOS, S_IRWXU | S_IRWXG | S_IRWXO);

        if ((fd = open(PATH_FIFO_SAUNA_ENTRADA, O_RDONLY)) < 0) {
                perror("error reading fifo sauna entrada");
                return;
        }

        while(read(fd, &pedido, sizeof(pedido)) > 0) {

                // ver se a sauna possui genero
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
        // deletar fifos
        unlink(PATH_FIFO_REJEITADOS);
}
