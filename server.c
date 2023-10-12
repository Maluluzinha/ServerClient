#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void decodeString(const char *input, char *command, int *nSensor, int *current, int *voltage, int *energyEficence) {
    sscanf(input, "%s %d %d %d %d", command, nSensor, current, voltage, energyEficence);
}
#define tamanhoPadrao 10
//Mensagens vindas do RTU
char installRequisition[tamanhoPadrao] = "INS_REQ";
char removeRequisition[tamanhoPadrao] = "REM_REQ";
char changeRequisition[tamanhoPadrao] = "CH_REQ";
char dataRequisition[tamanhoPadrao] = "SEN_REQ";
char allDataRequisition[tamanhoPadrao] = "VAL_REQ";

//Status pro RTU
char statusMensagem[BUFSZ];
//Vetor de sensores
#define N_SENSORES 100 // Número máximo de sensores
#define DADOS_SENSOR 100 // Tamanho máximo das informações
char tabelaSensor[N_SENSORES][DADOS_SENSOR];

//memset(statusMensagem, 0, BUFSZ); 

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        //Agora para receber o dado/ler a mensagem que o cliente enviou
        //Receiving message from client
        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ - 1, 0);  //Mensagem que chega do cliente
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf); //Print a mensagem do cliente

        //Não mexer acima
        //CODE INIT HERE:
        if (0 == strncmp(buf, "INS_REQ", 7)) {
        char buffer[5][BUFSZ];
         //Separa em espaço
        char espacoChar[] = " ";
        char *token = strtok(buf, espacoChar);
        int i = 0;
        while (NULL != token) {
        strcpy(buffer[i], token);
        token = strtok(NULL, espacoChar);
        i++;
      }
        int dadosSensor[4];
        for (int i = 1; i < 5; i++) {
        dadosSensor[i - 1] = atoi(buffer[i]);
        }
        //Calcula a potência:
        int potencia = dadosSensor[1] * dadosSensor[2];
       
        printf("Potencia: %d \n", potencia);
        printf("Números: %d, Corrente: %d, Tensão: %d, Eficiência: %d\n", dadosSensor[0], dadosSensor[1],dadosSensor[2],dadosSensor[3]);
        //printf("Potencia: %d /n", potencia);
        sprintf(statusMensagem, "OK 01\n");
        send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); //Manda o dado pro cliente
    }
    else if (0 == strncmp(buf, "kill", 7)){
        close(csock);
        exit(EXIT_FAILURE);
    }
        //CODE END HERE

        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, buf, strlen(buf) + 1, 0); //Manda o dado pro cliente
        if (count != strlen(buf) + 1) {
            logexit("send");
        }
        memset(buf, 0, BUFSZ);
        close(csock); //Fecha e volta pro inicio do loop
    }

    exit(EXIT_SUCCESS);
}

