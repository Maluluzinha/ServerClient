#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include <inttypes.h> //Habilitar uint16_t
#include "sys/types.h"  //No terminal do Linux, dar: man socket
#include "sys/socket.h" //ja deve puxar junto struct do socket
#include <arpa/inet.h>

#pragma once

void logexit(const char *msg);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);

void strcatspace(char *dest, char *src);

