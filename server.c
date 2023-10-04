#include "common.h"

#define BUFSZ 1024

 struct sockaddr_storage storage; //Instanciavel

void usage(int argc, char **argv){ //Função em caso de erro na chamada
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n");
    exit(EXIT_FAILURE);

}

int main(int argc, char **argv){
    if (argc < 3){          //Verifica se chamou o programa corretamente, senão retorna pro usage() acima. Primeiro verificador de erro
      usage(argc, **argv);    //Ver se os parâmetros tão ok
    }

    struct sockaddr_storage storage; //Instanciavel
     if(0 != server_sockaddr_init(argv[1], argv[2], &storage)){ //Recebe o tipo (v4 ou v6), o port, e o ponteiro pro storage. ERRO
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s = -1){ //Primeira verificação de erro
        logexit("Socket \n");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); //Servidor não tem send
    if (0 != bind(s, addr, sizeof(storage))) { //Inicializa com o socket e a struct, e o tamanho
        logexit("Bind \n");
    }

    if (0 != listen(s, 10)) { //Numero que vai no listen numero de conexoes que podem estar pendentes para tratamento
    logexit("listen");
    }

}