#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

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
    //while (1){
        //Agora para receber o dado/ler a mensagem que o cliente enviou
        //Receiving message from client
        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(csock, buf, BUFSZ - 1, 0);  //Mensagem que chega do cliente
        printf("Recebido: %d bytes: %s\n", (int)count, buf); //Print a mensagem do cliente

        //Não mexer acima
        //CODE INIT HERE:
        /*---------------------------------RECEBE SENSOR E INSTALA --------------------------------------------*/
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
        // Verifica se já existe um sensor com o mesmo ID (numero)
        for (int i = 0; i < N_SENSORES; i++) {
            if (tabelaSensor[i][0] == numero) {
                sensorExiste = true; // Sensor já existe
                break;
            }
        }
        
        if (sensorExiste == true) {
            printf("ERROR 04\n");
            sprintf(statusMensagem, "sensor already exists\n");
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
            tabelaSensor[indiceVazio][0] = numero; //ID
            tabelaSensor[indiceVazio][1] = dadosSensor[1] * dadosSensor[2]; //Potencia
            tabelaSensor[indiceVazio][2] = dadosSensor[3]; //Eficiência
            printf("Sensor com o ID %d adicionado à tabela.\n", numero);
            sprintf(statusMensagem, "successful installation\n");
            send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
            
        }  else {
            printf("A tabela de sensores está cheia, não é possível adicionar mais sensores.\n");

            }
        }
       
        printf("Potencia: %d \n", potencia);
        printf("Números: %d, Corrente: %d, Tensão: %d, Eficiência: %d\n", dadosSensor[0], dadosSensor[1],dadosSensor[2],dadosSensor[3]);
        
    }   /*---------------------------------REMOVE O SENSOR --------------------------------------------*/
        if (0 == strncmp(buf, "REM_REQ", 7)) { //REMOVE SENSOR
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
        
            for (int i = 1; i < 5; i++) {
                dadosSensor[i - 1] = atoi(buffer[i]);
            }

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
                for (int i = 0; i < N_SENSORES; i++) {
                    if (tabelaSensor[i][0] == numero) { //Zera todos os valores dos dados armazenados na tabela
                    tabelaSensor[i][0] = 0;
                    tabelaSensor[i][1] = 0;
                    tabelaSensor[i][2] = 0;
                    printf("Sensor com ID %d foi removido da tabela.\n", numero); //EXCLUIR
                    sprintf(statusMensagem, "successful removal\n");
                    send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
        }
    }
            }
            else {
                sprintf(statusMensagem, "sensor not installed\n");
                send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
            }

        }
         /*---------------------------------MUDA INFO DO SENSOR --------------------------------------------*/
        if (0 == strncmp(buf, "CH_REQ", 7)) { 
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
        
            for (int i = 1; i < 5; i++) {
                dadosSensor[i - 1] = atoi(buffer[i]);
            }

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
                for (int i = 0; i < N_SENSORES; i++) {
                    if (tabelaSensor[i][0] == numero) { //Muda os valores do sensor em especifico
                    tabelaSensor[i][0] = numero;
                    tabelaSensor[i][1] = dadosSensor[1] * dadosSensor[2];
                    tabelaSensor[i][2] = dadosSensor[3];
                    printf("Sensor com ID %d foi modificado.\n", numero); //EXCLUIR
                    sprintf(statusMensagem, "successful removal\n");
                    send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
                    }
                }
            }
            else {
                sprintf(statusMensagem, "sensor not installed\n");
                send(csock, statusMensagem, strlen(statusMensagem) + 1, 0); // Manda o dado pro cliente
            }
            //printf("Potencia: %d \n", potencia);
            printf("Números: %d, Corrente: %d, Tensão: %d, Eficiência: %d\n", dadosSensor[0], dadosSensor[1],dadosSensor[2],dadosSensor[3]);

        }
        /*--------------------------------- CONSULTA SENSOR ESPECIFICO --------------------------------------------*/
        if (0 == strncmp(buf, "SEN_REQ", 7)) {  //EXIBE INFO
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
    /*--------------------------------- IMPRIME VALORES DE TODOS--------------------------------------------*/
    if (0 == strncmp(buf, "VAL_REQ", 7)) {
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
        
        // Inicializa a mensagem para enviar todos os sensores juntos
        char todos[BUFSZ];
        memset(todos, 0, BUFSZ);
        
    for (int i = 0; i < N_SENSORES; i++) {
        if (tabelaSensor[i][0] != 0) {
            sprintf(statusMensagem, "sensor %d: (%d %d)", tabelaSensor[i][0],tabelaSensor[i][1], tabelaSensor[i][2]);
            strcat(todos, statusMensagem);
        }
    }
            send(csock, todos, strlen(todos) + 1, 0);
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
    //}
        close(csock); //Fecha e volta pro inicio do loop
    }

    exit(EXIT_SUCCESS);
}

