#include "sys/socket.h"

#include "../utils.c"

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int sockfd;

long unsigned int servlen;

void (*mensajes_cli)() = NULL; // puntero a funcion que se vaya a usar

void handshake_tipo_cliente(char);

/*
 * El cliente recibe el checksum del archivo, recibe el archivo y posteriormente
 * verifica el checksum con el que genera el
 */

void cliente_C()
{
  long int cantidad_de_bits;
  long unsigned int long_buffer;
  handshake_tipo_cliente('C');

  char *buffer = malloc(1);
  FILE *descarga;
  if((descarga = fopen("./db/base_de_datos_descargado.db", "wb")) == NULL)
  {
    perror("Error abriendo el punto de descarga:");
    exit(EXIT_FAILURE);
  }

  unsigned long long hash,hash_recibido;

  long_buffer = handshake_recv_tam_buffer(sockfd);
  buffer = realloc_char_perror(buffer, long_buffer);

  memset(buffer, '\0', long_buffer);

  cantidad_de_bits = recv( sockfd, buffer, long_buffer,0 );

  if ( cantidad_de_bits < 0 ) 
  {
    perror( "escritura de socket" );
    close(sockfd);
    exit( 1 );
  }
  else if (cantidad_de_bits == 0) 
  {
    printf("Server out");
    close(sockfd);
    exit(0);
  }

  hash_recibido = strtoull(buffer, NULL, 10); 

  while((cantidad_de_bits = recv( sockfd, buffer, 1 ,0 )) > 0)
    {
      fputc ( buffer[0], descarga );
    }

  fclose(descarga);

  hash = generar_hash_djb2("./db/base_de_datos_descargado.db");
  printf("Hash recibido: %llu\nHash generado: %llu\n",hash_recibido,hash);
  if (hash == hash_recibido) {
    printf("Archivo descargado con exito\n");
  }
  else
  {
    printf("Fallo en la descarga\n");
  }
}

/*
 * El cliente B genera un promp y espera las entradas del usuario por teclado
 * y se las va a enviar a la db
 */

void cliente_B()
{
  long int cantidad_de_bits;
  long unsigned int long_buffer;
  handshake_tipo_cliente('B');

  char *buffer = malloc(1);

  while(1) {			 
    buffer = realloc_char_perror(buffer, 512);
    printf("sqlite3>");
    fgets(buffer,512,stdin);

    handshake_send_tam_buffer(strlen(buffer)+1,sockfd);

    cantidad_de_bits = send( sockfd, buffer, strlen(buffer),0 );

    if ( cantidad_de_bits < 0 ) {
      perror( "escritura de socket" );
      close(sockfd);
      exit( 1 );
    }
    else if (cantidad_de_bits == 0) {
      printf("No Mande mas nada");
      close(sockfd);
      exit(0);
    }

    long_buffer = handshake_recv_tam_buffer(sockfd);

    buffer = realloc_char_perror(buffer,long_buffer);

    memset(buffer,'\0',long_buffer);

    cantidad_de_bits = recv( sockfd, buffer, long_buffer,0 );
    if ( cantidad_de_bits < 0 ) {
      perror( "escritura de socket" );
      close(sockfd);
      exit( 1 );
    }else if (cantidad_de_bits == 0) {
      printf("Server out");
      close(sockfd);
      exit(0);
    }
    printf("%s\n",buffer);
  }
}

/*
 * El cliente A pide ver el log y lo muestra en pantalla
 */

void cliente_A()
{
  long int cantidad_de_bits;
  long unsigned int long_buffer;
  handshake_tipo_cliente('A');

  char *buffer = malloc(1);
  char *query = "SELECT * FROM Log";

  while(1) {			 
    handshake_send_tam_buffer(strlen(query),sockfd);

    cantidad_de_bits = send( sockfd, query, strlen(query),0);

    if ( cantidad_de_bits < 0 ) {
      perror( "escritura de socket" );
      close(sockfd);
      exit( 1 );
    }  else if (cantidad_de_bits == 0) {
      printf("No Mande mas nada");
      close(sockfd);
      exit(0);
    }

    long_buffer = handshake_recv_tam_buffer(sockfd);
    buffer = realloc_char_perror(buffer,long_buffer);
    memset(buffer,'\0',long_buffer);

    cantidad_de_bits = recv( sockfd, buffer, long_buffer+1,0 );
    if ( cantidad_de_bits < 0 ) {
      perror( "escritura de socket" );
      close(sockfd);
      exit( 1 );
    }  else if (cantidad_de_bits == 0) {
      printf("Server out");
      close(sockfd);
      exit(0);
    }
    printf("%s\n",buffer);
    sleep(1);
  }
}

/* 
 * El cliente se entera del tamanio del buffer a utilizar y le dice que tipo de
 * cliente es
 */

void handshake_tipo_cliente(char tipo)
{
  long int cantidad_de_bits;	

  char handshake[1];

  handshake[0] = tipo;

  cantidad_de_bits = send(sockfd, handshake, 1 ,0);

  if ( cantidad_de_bits < 0 ) {
    perror( "escritura de socket" );
    close(sockfd);
    exit( 1 );
  } else if (cantidad_de_bits == 0) {
    printf("Server out");
    close(sockfd);
    exit(0);
  }
}

