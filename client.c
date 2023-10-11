#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 1024

char sensor[5][BUFSZ];
char buf[BUFSZ];
char bufInfo[BUFSZ];

//struct sensor {
//    char comand[10];
//    int nSensor;
//    int current;
//    int voltage;
//    int energyEficence;
//};


//Lista de comandos de solicitação:
char comandInstall[BUFSZ] = "install";
char removeSensor[BUFSZ] = "remove";
char changeData[BUFSZ] = "change";
char displayData[BUFSZ] = "show";
char endAll[BUFSZ] = "kill";

//Lista de mensagens:
 char initSensor[BUFSZ] = "INS_REQ";

//Pra testar: ./server v4 90900 ./client 127.0.0.1 90900
//Arg_insreq é argmentos pro insreq, ´5 pq ~sao 5 argumentos na string;

int main(int argc, char **argv) {
    
    //struct sensor dataSensor;

    //NÃO MEXER
	if (argc < 3) {
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage); //Inicializa com o ponteiro e faz um casting, joga um ponteiro no storage //Ponteiro da struct
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("connected to %s\n", addrstr);
	//
	//CODE INIT:
	//char command[BUFSZ];
	//fgets(command, BUFSZ, stdin);
	//Recebe o comando
	//char mensageComand[BUFSZ];
	//scanf("%s", mensageComand);
	//fgets(mensageComand, BUFSZ, stdin);
	//size_t len = strlen(mensageComand);
	char mensageComand[BUFSZ];
	scanf("%s", mensageComand);
	char buffer[10][BUFSZ];
    //Separa a string por espaço
    char espacoChar[] = " ";
    char *token = strtok(mensageComand, espacoChar);
	char *dados[100]; // Um array de ponteiros de strings para armazenar as palavras
    //int num_tokens = 0;
    int i = 0;
        while (NULL != token) {
			dados[0] = token;
            strcpy(buffer[i], token);
            token = strtok(NULL, espacoChar);
            i++;
		}
	for(int i = 0; i<BUFSZ; i++){
		//printf("String %s", buffer[i]);
	}
	//If que verifica o comando
	if(strcmp(buffer[0], "install") == 0){
		char buf[10][BUFSZ]; //5 valores na string, de valor x
     	char *msg_req = "INS_REQ";
      	memcpy(buf[0], msg_req, sizeof(&msg_req));
		memcpy(buffer[0], msg_req, sizeof(&msg_req));
		for (int i = 0; i < 10; i++) {
		printf("%s", buffer[i]);
		}
		// char dadosSensor[4];
        // for (int i = 1; i < 5; i++) {
        // printf("%s", buffer[i]);
        // }
		//printf("%s", dadosSensor);

		//for (int i = 0; i < 10; i++) {
            //strncpy(buf[i + 1], dados[i], BUFSZ - 1);

        //}
        //for (int i = 0; i < 11; i++) {
		//	strcpy(buf[i], dados[i]);
        //}

		char buf_to_send[BUFSZ];	   //Uma unica string para enviar
      	memset(buf_to_send, 0, BUFSZ); //Memoria pra string
		for (int i = 0; i <= 4; i++) {
           strcat(buf_to_send, buf[i]);
           strcat(buf_to_send, " ");
        }
		
      	//strcpy(buf_to_send, buf[0]);	//Copia o buffer
      	size_t count = send(s, buf_to_send, strlen(buf_to_send) + 1, 0);


      		if (count != strlen(buf_to_send) + 1) {
        		logexit("send");
      		}
	 }
	 else if(strcmp(buffer[0], "kill") == 0){
		//char buf[1][BUFSZ]; //1 valor na string - char kill
     	//char *msg_req = "kill";
      	//memcpy(buf[0], msg_req, sizeof(&msg_req));

		char buf_to_send[BUFSZ];	//Uma unica string
      	memset(buf_to_send, 0, BUFSZ); //Aloca memória
		strcat(buf_to_send, "kill");
		send(s, buf_to_send, strlen(buf_to_send) + 1, 0);
		printf("Killed \n");
        close(s);
        exit(EXIT_FAILURE);
	 }
	 else {
		printf("Invalid command \n");
        close(s);
        exit(EXIT_FAILURE);
	 }

	 



	char buf[BUFSZ];
	memset(buf, 0, BUFSZ);
	printf("mensagem> ");
	fgets(buf, BUFSZ-1, stdin);
	size_t count = send(s, buf, strlen(buf)+1, 0);
	if (count != strlen(buf)+1) {
		logexit("send");
	}

	memset(buf, 0, BUFSZ);
	unsigned total = 0;
	while(1) {
		count = recv(s, buf + total, BUFSZ - total, 0);
		if (count == 0) {
			// Connection terminated.
			break;
		}
		total += count;
	}
	close(s);

	printf("received %u bytes\n", total);
	puts(buf);

	exit(EXIT_SUCCESS);
}