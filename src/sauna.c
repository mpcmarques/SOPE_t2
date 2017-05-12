#include "sauna.h"

int main(int argc, char const *argv[]) {
        // start sauna
        startSauna(20);
        return 0;
}

void startSauna(int numLugares){
        int fd;
        char str[1024];

        // make fifos
        mkfifo(PATH_FIFO_SAUNA_ENTRADA, S_IRWXU | S_IRWXG | S_IRWXO);
        mkfifo(PATH_FIFO_REJEITADOS, S_IRWXU | S_IRWXG | S_IRWXO);

        fd = open(PATH_FIFO_SAUNA_ENTRADA, O_RDONLY);

        while(readline(fd,str)) printf("%s",str);
        close(fd);
}

int readline(int fd, char *str)
{
        int n;
        do
        {
                n = read(fd,str,1);
        }
        while (n>0 && *str++ != '\0');
        return (n>0);
}
