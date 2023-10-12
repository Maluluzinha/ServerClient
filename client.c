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

void quebraString(const char *entrada, char *info[], int maxPedacos) {
    char copiaEntrada[strlen(entrada) + 1];
    strcpy(copiaEntrada, entrada);
	char espacoChar[] = " ";

    int contador = 0;
    char *token = strtok(copiaEntrada, espacoChar);

    while (token != NULL && contador < maxPedacos) {
        info[contador] = strdup(token); // Aloca memória para cada pedaço
        token = strtok(NULL, espacoChar);
        contador++;
    }
}

int quantosDados(const char *dados[]) {
    int tamanho = 0;
    while (dados[tamanho] != NULL) {
        tamanho++;
    }
    return tamanho;
}


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

//	while(1){
	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("connected to %s\n", addrstr);
	//
	//CODE INIT:
	while(1){ //Envia o 01

    char dadosDigitados[BUFSZ]; //Entrada
	memset(dadosDigitados, 0, BUFSZ); //Aloca memória
    printf("Digite uma string: ");
    fgets(dadosDigitados, sizeof(dadosDigitados), stdin);

    dadosDigitados[strcspn(dadosDigitados, "\n")] = '\0'; // Remover o caractere de nova linha

    char *dados[20]; //Limite de 20 palavras
    quebraString(dadosDigitados, dados, 20);
    

	if (dados[0] != NULL) {
		//INSTALAR SENSOR:
        if (strcmp(dados[0], "install") == 0) {
            //printf("Comando 'install' detectado.\n");
		if(quantosDados((const char **)dados) == 6){
		printf("We are Number %d\n",quantosDados((const char **)dados));
        char *msg_req = "INS_REQ";
      	memcpy(dados[1], msg_req, sizeof(&msg_req));
		    char buf_to_send[BUFSZ];	   //Uma unica string para enviar
      	    memset(buf_to_send, 0, BUFSZ); //Memoria pra string
		    for (int i = 1; i <= 5; i++) {
            strcat(buf_to_send, dados[i]);
            strcat(buf_to_send, " ");
            }

			size_t count = send(s, buf_to_send, strlen(buf_to_send) + 1, 0);

      		if (count != strlen(buf_to_send) + 1) {
        		logexit("send");
      		}
		    } else{
				(printf("ERROR 03 \n"));
				close(s);
                exit(EXIT_FAILURE);
			}
		
		} else if(strcmp(dados[0], "show") == 0){ //COMANDO SHOW
			char *msg_req = "SEN_REQ";
      		memcpy(dados[1], msg_req, sizeof(&msg_req));

			char buf_to_send[BUFSZ];	   //Uma unica string para enviar
      	    memset(buf_to_send, 0, BUFSZ); //Memoria pra string
			for (int i = 1; i < 3; i++) {
            strcat(buf_to_send, dados[i]);
            strcat(buf_to_send, " ");
            }
			size_t count = send(s, buf_to_send, strlen(buf_to_send) + 1, 0);
			if (count != strlen(buf_to_send) + 1) {
        		logexit("send");
      		}

		} else if(strcmp(dados[0], "kill") == 0){ //COMANDO KILL
			char buf_to_send[BUFSZ];	//Uma unica string
      	    memset(buf_to_send, 0, BUFSZ); //Aloca memória
		    strcat(buf_to_send, "kill");
		    send(s, buf_to_send, strlen(buf_to_send) + 1, 0);
		    printf("Encerrando cliente e servidor \n");
            close(s);
            exit(EXIT_FAILURE);

		} else {
		  printf("Invalid command \n");
		  
          close(s);
          exit(EXIT_FAILURE);
	 }
		
	}


	 



	char buf[BUFSZ];
	memset(buf, 0, BUFSZ);
	//printf("mensagem> ");
	//fgets(buf, BUFSZ-1, stdin);
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
	printf("received %u bytes\n", total);
	
	
	puts(buf);
	//}
	close(s);
	exit(EXIT_SUCCESS);
	}
}