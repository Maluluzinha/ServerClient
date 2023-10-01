#include "common.h"

#define BUFSZ 1024

void usage(int argc, char **argv){ //Função em caso de erro na chamada
    printf("Usage %s <server_ip> <server_port>");
    //%s recebe dados de uma string fornecida
    printf("Ex: %s 127.0.0.1 51511");
    exit(EXIT_FAILURE); //Corresponde a um bool = 1; Falha

}

void logexit(const char *mensagem){ //Função de erro que imprime uma mensagem, a mensagem recebida em um ponteiro
    perror(*mensagem);  //man perror
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if (argc < 3){          //Verifica se chamou o programa corretamente, senão retorna pro usage() acima. Primeiro verificador de erro
      usage(argc, **argv);    //Ver se os parâmetros tão ok
    }
    //Lembrar: socket é uma função int -> int socket(int domain, int type, int protocol);
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s = -1){ //Primeira verificação de erro
        logexit("Socket /n");
    }

    if(0 != connect(s, addr, sizeof(addr))){ //Função que verifica a conexão. Se connect = 0, significa erro. Se !=, sem erro, ignora o if.
    //Variável addr é o endereço do servidor
    //No trabalho, provavelmente addr e o sizeof vão receber uma struct
        logexit("Connect /n"); 
    }

    char addrstring[BUFSZ];
    addrtostr(addr, addrstring, BUFSZ);
    printf("Connected succefully with %s", addrstring);

    char buf[BUFSZ]; //Armazenar os dados
    memset(buf, 0, BUFSZ); //Iniciar o ponteiro com 0
    unsigned total = 0; //

    //Cliente primeiro deve enviar o dado
    fgets(buf, BUFSZ+1, stdin); //Lê os dados inseridos no teclado
    int count = send(s ,buf, strlen(buf)+1, 0); //Envia o dado
    if(count != strlen(buf)+1){ //Erro envio
        logexit("Send");
    }

    while(1){
        count = recv(s, buf +total, BUFSZ -total, 0); //Receber
        if(count = 0){
            break; //Conexão fechada
        }
        total +=count; //APAGAR DEPOIS ESSE TOTAL
    }
    close(s); //Fecha o socket  


    exit(EXIT_SUCCESS);
}
