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

void instalarArquivo(const char *nome_arquivo, char *sensor[20]) {
    FILE *arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo especificado: %s\n", nome_arquivo);
        return;
    }

    char linha[BUFSZ];
    int linha_atual = 0;
    while (fgets(linha, BUFSZ, arquivo) != NULL && linha_atual < 5) {
        linha[strcspn(linha, "\n")] = '\0';
        strncpy(sensor[linha_atual], linha, BUFSZ - 1);
        linha_atual++;
    }
	printf("successful installation: %s\n", nome_arquivo);

    fclose(arquivo);
}

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

//Pra testar: ./server v4 90900 ./client 127.0.0.1 90900

int main(int argc, char **argv) {

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

	while(1){
	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);
	printf("connected to %s\n", addrstr);
	//
	//CODE INIT:
	//while(1){ //Envia o 01

    char dadosDigitados[BUFSZ]; //Entrada
	memset(dadosDigitados, 0, BUFSZ); //Aloca memória
    printf("Digite uma string: ");
    fgets(dadosDigitados, sizeof(dadosDigitados), stdin);

    dadosDigitados[strcspn(dadosDigitados, "\n")] = '\0'; // Remover o caractere de nova linha

    char *dados[20]; //Limite de 20 palavras
	char *dadosDoServer[20]; //Limite de 20 palavras
	memset(dados, 0, BUFSZ); //Limpa os dados para validar o tamanho deles
	memset(dadosDoServer, 0, BUFSZ);
    quebraString(dadosDigitados, dados, 20); //Chama a função para quebrar os dados da string	

	if (dados[0] != NULL) {
		/*--------------------------------- COMANDO PARA INSTALAR SENSOR E ARQUIVO --------------------------------------------*/
        if (strcmp(dados[0], "install") == 0) {
			 if (strcmp(dados[1], "file") == 0 && dados[2] != NULL) {
            const char *nome_arquivo = dados[2]; // Obtém o nome do arquivo a partir dos argumentos

            instalarArquivo(nome_arquivo, dados);
			}
			
			else if(quantosDados((const char **)dados) == 6){ //Verifica se está no formato adequado
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

		    } else {

				printf("invalid sensor\n");
				char buf_to_send[BUFSZ];	//Uma unica string
      	    	memset(buf_to_send, 0, BUFSZ); //Aloca memória
		    	strcat(buf_to_send, "ERROR 03");
		    	send(s, buf_to_send, strlen(buf_to_send) + 1, 0);

			}
		
		} 
		/*--------------------------------- COMANDO PARA REMOVER SENSOR --------------------------------------------*/
		else if(strcmp(dados[0], "remove") == 0){
			char *msg_req = "REM_REQ";
      		memcpy(dados[0], msg_req, sizeof(&msg_req));

			char buf_to_send[BUFSZ];	   //Uma unica string para enviar
      	    memset(buf_to_send, 0, BUFSZ); //Memoria pra string
			for (int i = 0; i < 2; i++) {
            strcat(buf_to_send, dados[i]);
            strcat(buf_to_send, " ");
            }
			size_t count = send(s, buf_to_send, strlen(buf_to_send) + 1, 0);
			if (count != strlen(buf_to_send) + 1) {
        		logexit("send");
      		}

		}
		/*--------------------------------- COMANDO PARA MOSTRAR UM SENSOR --------------------------------------------*/ 
		else if(strcmp(dados[0], "show") == 0){
			if(strcmp(dados[1], "value") == 0){
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
			}
		/*--------------------------------- COMANDO PARA PARA MOSTRAR TODOS OS SENSORES --------------------------------*/
			else if(strcmp(dados[1], "values") == 0){

			char buf_to_send[BUFSZ];	   //Uma unica string para enviar
      	    memset(buf_to_send, 0, BUFSZ); //Memoria pra string
            strcat(buf_to_send, "VAL_REQ");
			size_t count = send(s, buf_to_send, strlen(buf_to_send) + 1, 0);
			if (count != strlen(buf_to_send) + 1) {
        		logexit("send");
      		}
			
		}
		}
		/*--------------------------------- COMANDO PARA MODIFICAR DADOS --------------------------------------------*/
		else if(strcmp(dados[0], "change") == 0){

			if(quantosDados((const char **)dados) == 6){ //Verifica se está no formato adequado
        		char *msg_req = "CH_REQ";
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

		    } else {

				printf("invalid sensor\n");
				char buf_to_send[BUFSZ];	//Uma unica string
      	    	memset(buf_to_send, 0, BUFSZ); //Aloca memória
		    	strcat(buf_to_send, "ERROR_03");
		    	send(s, buf_to_send, strlen(buf_to_send) + 1, 0);

			}

		}
		/*----------------------------------------- COMANDO KILL --------------------------------------------------*/
		else if(strcmp(dados[0], "kill") == 0){ //COMANDO KILL

			char buf_to_send[BUFSZ];	//Uma unica string
      	    memset(buf_to_send, 0, BUFSZ); //Aloca memória
		    strcat(buf_to_send, "kill");
		    send(s, buf_to_send, strlen(buf_to_send) + 1, 0);
		    printf("Encerrando cliente e servidor \n");
            close(s);
            exit(EXIT_FAILURE);

		} 
		/*----------------------------------------- COMANDO INVÁLIDO --------------------------------------------------*/
		else {

		  printf("Invalid command \n");
          close(s);
          exit(EXIT_FAILURE);

	 	}
		
	}

	char buf[BUFSZ];
	memset(buf, 0, BUFSZ);
	recv(s, buf, BUFSZ - 1, 0);
	buf[strcspn(buf, "\n")] = '\0'; // Remover o caractere de nova linha
	quebraString(buf, dadosDoServer, 20);

	//Recebe a string e imprime a mensagem
	if (dadosDoServer[0] != NULL) {
    	if (strcmp(dadosDoServer[0], "OK_01") == 0) {
        	printf("successful installation\n");
		}
		if (strcmp(dadosDoServer[0], "OK_02") == 0) {
        	printf("successful removal\n");
		}
		if (strcmp(dadosDoServer[0], "OK_03") == 0) {
        	printf("successful change\n");
		}
		if (strcmp(dadosDoServer[0], "ERROR_01") == 0) {
        	printf("sensor not installed\n");
		}
		if (strcmp(dadosDoServer[0], "ERROR_02") == 0) {
        	printf("no sensors\n");
		}
		if (strcmp(dadosDoServer[0], "ERROR_03") == 0) {
        	printf("invalid sensor\n");
		}
		if (strcmp(dadosDoServer[0], "ERROR_04") == 0) {
        	printf("sensor already exists\n");
		}
	}
	
	puts(buf);
	//}
	//close(s);
	exit(EXIT_SUCCESS);
	}
}