#include "../include/signal.h"
#include "../signals.c"
#include "./manejo_del_cliente.c"

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h> 

/*
 * Segun el tipo de cliente se le asigna un metodo u otro.
 */

int asignar_tipo_cliente(char* tipo)
{
  if(!strcmp(tipo, "A"))
  {
    mensajes_cli = &cliente_A;
  }
  else if(!strcmp(tipo, "B"))
  {
    mensajes_cli = &cliente_B;
  }
  else if(!strcmp(tipo, "C"))
  {
    mensajes_cli = &cliente_C;
  }
  else
  {
    fprintf(stderr, "Ingrese un tipo de cliente valido: %s",tipo);
    return 0;
  }
  printf("Tipo de cliente asignado: %s\n",tipo);
  return 1;
}

void unix_cli(char* archivo) 
{
  struct sockaddr_un serv_addr;
  char *path;

  path = malloc((strlen(archivo)+strlen("./ipc/")+1)*sizeof(char));

  path = strcpy(path, "./ipc/");
  path = strcat(path, archivo);

  memset( (char *)&serv_addr, '\0', sizeof(serv_addr) );
  serv_addr.sun_family = AF_UNIX;
  strcpy( serv_addr.sun_path, path );
  servlen = strlen( serv_addr.sun_path) + sizeof(serv_addr.sun_family);

  free(path);
  if ( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0) ) < 0) {
    perror( "creación de socket" );
    exit( 1 );
  }
  if ( connect( sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0 ) {
    perror( "conexión" );
    exit( EXIT_FAILURE );
  }
  mensajes_cli();
}

void ipv4_cli(char* host,short unsigned int puerto)
{
  struct sockaddr_in serv_addr;

  sockfd = socket_perror( AF_INET, SOCK_STREAM, 0 );

  memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
  serv_addr.sin_family = AF_INET;
  if(!inet_pton(AF_INET, host, &serv_addr.sin_addr)){
    perror("Conexion ipv4");
    exit(EXIT_FAILURE);
  }
  serv_addr.sin_port = htons( puerto );

  if ( connect( sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0 ) {
    perror( "conexión" );
    exit( EXIT_FAILURE );
  }	
  mensajes_cli();
}

void ipv6_cli(char* host,short unsigned int puerto)
{
  struct sockaddr_in6 serv_addr;

  sockfd = socket_perror( AF_INET6, SOCK_STREAM,0);

  memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
  serv_addr.sin6_family = AF_INET6;
  if(!inet_pton(AF_INET6, host, &serv_addr.sin6_addr)){
    perror("Conexion ipv6");
    exit(EXIT_FAILURE);
  }
  serv_addr.sin6_port = htons( puerto );

  if ( connect( sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) < 0 ) {
    perror( "conexión" );
    exit( EXIT_FAILURE );
  }

  mensajes_cli();
}

