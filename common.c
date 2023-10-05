#include "common.h"
#include <arpa/inet.h>

void logexit(const char *mensagem){ //Função de erro que imprime uma mensagem, a mensagem recebida em um ponteiro
    perror(*mensagem);  //man perror
    exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage) { //Assinatura?
        if(addrstr == NULL || portstr == NULL) {
            return -1; //Acaba
        }

    //Parse da port
    uint16_t port = (u_int16_t)atoi(portstr); //Unsigned short. 16bits, esta no padrão do protocolo TCP
    if(port == 0){
        return -1; //Passou port errada
    }
    port = htons(port); //O numero da port deve ser big endian. Isso serve para converter para a representação de rede que é em big endian
                        //Host to network short

    //Parse do endereço. Fazer para IPv4 e IPv6
    //Primeiro pro IPv4
    struct in_addr inaddr4; //32-bits IP address
    if(inet_pton(AF_INET, addrstr, &inaddr4)){ //Parser. Tentar fazer de IPv4, se der certo joga na variável, então não é ipv6
        struct sockaddr_in *addr4 = (struct sockaddr_in *) storage; //Casting
        addr4->sin_family = AF_INET; //Essa estrutura está inicializando o storage lá presente no cliente
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }
    //Agora para IPv6
    struct in6_addr inaddr6; //128-bits IP address
    if(inet_pton(AF_INET6, addrstr, &inaddr6)){ //Parser. Tentar fazer de IPv4, se der certo joga na variável, então não é ipv6
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *) storage; //Casting
        addr6->sin6_family = AF_INET6; //Essa estrutura está inicializando o storage lá presente no cliente
        addr6->sin6_port = port;
        //addr6->sin6_addr = inaddr6; //Dessa maneira, o arranjo não está correto
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6)); //Copiando o dado do inaddr6 para outro vetor. Deve usar o endereço do inaddr6
        return 0;
    }
    return -1;
}

//Addr to Str
void addrtostr(const struct sockaddr *addr, char *str, size_t strsize){
    //Agora printar o endereço
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port; //Para converter de novo p big endian
     
    if(addr->sa_family == AF_INET){  //Verificar o tipo de protocolo, família
    
    //Dentro do if, fazer o parcing
        version = 4; //IPv4
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr; //Muda o tipo do ponteiro
        if(!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr,
                        INET6_ADDRSTRLEN + 1)){                 //Network to Presentation
            logexit("Ntop"); //Deu erro
        }
        port = ntohs(addr4->sin_port); //Desfazer, passar da apresentação de rede para a de dispositivo //Network to host short

    } else if(addr->sa_family == AF_INET6){
        version = 6; //IPv6
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr; //Muda o tipo do ponteiro
        if(!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr,
                        INET6_ADDRSTRLEN + 1)){                 //Network to Presentation
            logexit("Ntop"); //Deu erro
        }
         port = ntohs(addr6->sin6_port);

    } else {
            logexit("Unknow Protocol Family.");
    }
    if(str){
    snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port); //Printa o endereço
    }
}

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage) {
  //Parse da port
    uint16_t port = (u_int16_t)(portstr); //Unsigned short. 16bits, esta no padrão do protocolo TCP
    if(port == 0){
        return -1; //Passou port errada
    }
    port = htons(port); //O numero da port deve ser big endian.
    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY; //Esse muda, para aceitar qualquer endereço IP //CUIDADO
        return (0);

    } else if (0 == strcmp(proto, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return(0);

    } else{
        return -1;
    }
}