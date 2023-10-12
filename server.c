#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

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
int tabelaSensor[N_SENSORES][DADOS_SENSOR]; //Tabela
int dadosSensor[4]; //Dados

void adicionarSensor(int dadosSensor[]) {
    int numero = dadosSensor[0];

    // Verifica se já existe um sensor com o mesmo ID (numero)
    for (int i = 0; i < N_SENSORES; i++) {
        if (tabelaSensor[i][0] == numero) {
            printf("Um sensor com o ID %d já existe na tabela.\n", numero);
            return; // Não adiciona o sensor se já existe um com o mesmo ID
        }
    }

    // Encontra um índice vazio na tabelaSensor e armazena os dados
    int indiceVazio = -1;
    for (int i = 0; i < N_SENSORES; i++) {
        if (tabelaSensor[i][0] == 0) {
            indiceVazio = i;
            break;
        }
    }

    if (indiceVazio != -1) {
        tabelaSensor[indiceVazio][0] = numero;
        tabelaSensor[indiceVazio][1] = dadosSensor[1];
        tabelaSensor[indiceVazio][2] = dadosSensor[2];
        tabelaSensor[indiceVazio][3] = dadosSensor[3];
        printf("Sensor com o ID %d adicionado à tabela.\n", numero);
    } else {
        printf("A tabela de sensores está cheia, não é possível adicionar mais sensores.\n");
    }
    
}

// void imprimirSensores() {
//     for (int i = 0; i < N_SENSORES; i++) {
//         if (tabelaSensor[i][0] != 0) {
//             printf("Sensor %d:\n", tabelaSensor[i][0]);
//             printf("Corrente: %d\n", tabelaSensor[i][1]);
//             printf("Tensão: %d\n", tabelaSensor[i][2]);
//             printf("Eficiência: %d\n", tabelaSensor[i][3]);
//             printf("\n");
//         }
//     }
// }

// bool sensorExists(int sensorId) {
//      for (int i = 0; i < N_SENSORES; i++) {
//         if (tabelaSensor[i][0] == sensorId) {
//             return true; // Sensor encontrado na tabela.
//         }
//     }
//     return false; // Sensor não encontrado na tabela.
// }

// bool sensorExists(int dadosSensor[]) {
//     int numero = dadosSensor[0];
//     // Verifica se já existe um sensor com o mesmo ID (numero)
//     for (int i = 0; i < N_SENSORES; i++) {
//         if (tabelaSensor[i][0] == numero) {
//             printf("Um sensor com o ID %d já existe na tabela.\n", numero);
//             return true; // Sensor encontrado na tabela.
//         }
//     }
//     printf("Um sensor com o ID %d não existe na tabela.\n", numero);
//     return false; // Sensor não encontrado na tabela.
// }

// void statusSensor(int dadosSensor[]){
//  int numero = dadosSensor[0];

//     // Verifica se já existe um sensor com o mesmo ID (numero)
//     for (int i = 0; i < N_SENSORES; i++) {
//         if (tabelaSensor[i][0] == numero) {
//             printf("Um sensor com o ID %d já existe na tabela.\n", numero);
//             return; // Não adiciona o sensor se já existe um com o mesmo ID
//         }
//         else{
//             printf("Um sensor com o ID %d não existe.\n", numero);
//         }
//     }
// }
//Talvez seja util

void limparTabelaSensor() {
    for (int i = 0; i < N_SENSORES; i++) {
        for (int j = 0; j < DADOS_SENSOR; j++) {
            tabelaSensor[i][j] = 0; // Define todos os elementos da matriz para zero
        }
    }
}


void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {

    //INICIALIZA TABELA
    for (int i = 0; i < N_SENSORES; i++) {
        for (int j = 0; j < DADOS_SENSOR; j++) {
            tabelaSensor[i][j] = 0;
        }
    }

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

    //    while (1){ 
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
        memset(statusMensagem, 0, BUFSZ); //Limpa o vetor de status
         //Separa em espaço
        char espacoChar[] = " ";
        char *token = strtok(buf, espacoChar);
        int i = 0;
        while (NULL != token) {
        strcpy(buffer[i], token);
        token = strtok(NULL, espacoChar);
        i++;
      }
        //int dadosSensor[4];
        for (int i = 1; i < 5; i++) {
        dadosSensor[i - 1] = atoi(buffer[i]);
        }
        //Calcula a potência:
        int potencia = dadosSensor[1] * dadosSensor[2];
        //CODE END:
        //CODE INIT:
        int numero = dadosSensor[0];
        bool sensorExiste = false; // Variável de controle
        //int indiceExistente = -1; // Armazena o índice do sensor existente, se houver
        // Verifica se já existe um sensor com o mesmo ID (numero)
        for (int i = 0; i < N_SENSORES; i++) {
            if (tabelaSensor[i][0] == numero) {
                sensorExiste = true; // Sensor já existe
                //indiceExistente = i; // Armazena o índice do sensor existente
                //sprintf(statusMensagem, "Sensor já existe. ERROR 01\n");
                //send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); //Manda o dado pro cliente
                break;
            }
        }
        
        if (sensorExiste == true) {
            printf("Sensor já existe. Não adicionado\n");
            sprintf(statusMensagem, "ERROR 04\n");
            send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
        
        } else {
        // Encontra um índice vazio na tabelaSensor e armazena os dados
        int indiceVazio = -1;
        for (int i = 0; i < N_SENSORES; i++) {
            if (tabelaSensor[i][0] == 0) {
                indiceVazio = i;
                break;
            }
        }

        if (indiceVazio != -1) {
            tabelaSensor[indiceVazio][0] = numero;
            tabelaSensor[indiceVazio][1] = dadosSensor[1] *dadosSensor[2];
            //tabelaSensor[indiceVazio][2] = dadosSensor[2];
            tabelaSensor[indiceVazio][3] = dadosSensor[3];
            printf("Sensor com o ID %d adicionado à tabela.\n", numero);
            sprintf(statusMensagem, "successful installation\n");
            send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
        }  else {
            printf("A tabela de sensores está cheia, não é possível adicionar mais sensores.\n");
            }
        }
       
        printf("Potencia: %d \n", potencia);
        printf("Números: %d, Corrente: %d, Tensão: %d, Eficiência: %d\n", dadosSensor[0], dadosSensor[1],dadosSensor[2],dadosSensor[3]);
        //printf("Potencia: %d /n", potencia);
        //sprintf(statusMensagem, "OK 01\n");
        //send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); //Manda o dado pro cliente
        
    } //EXIBE INFO
    if (0 == strncmp(buf, "SEN_REQ", 7)) {
        char buffer[5][BUFSZ];
        memset(statusMensagem, 0, BUFSZ); //Limpa o vetor de status
         //Separa em espaço
        char espacoChar[] = " ";
        char *token = strtok(buf, espacoChar);
        int i = 0;
        while (NULL != token) {
        strcpy(buffer[i], token);
        token = strtok(NULL, espacoChar);
        i++;
      }
        //int dadosSensor[4];
        for (int i = 1; i < 5; i++) {
        dadosSensor[i - 1] = atoi(buffer[i]);
        }
        //Calcula a potência:
        int potencia = dadosSensor[1] * dadosSensor[2];
        
        int numero = dadosSensor[0];
        bool sensorExiste = false; // Variável de controle
        // Verifica se já existe um sensor com o mesmo ID (numero)
        for (int i = 0; i < N_SENSORES; i++) {
            if (tabelaSensor[i][0] == numero) {
                sensorExiste = true; // Sensor já existe
                break;
            }
        }
        if (sensorExiste == true){
            sprintf(statusMensagem, "sensor %d: Potencia: %d, Eficiência: %d\n", dadosSensor[0], potencia, dadosSensor[3]);
            send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
        }
        else {
            sprintf(statusMensagem, "sensor not installed\n");
            send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
        }

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

