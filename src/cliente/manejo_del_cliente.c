#include "sys/socket.h"

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int sockfd;

long unsigned int servlen;

void (*mensajes_cli)() = NULL;

long unsigned int handshake_cliente(char);
long unsigned generar_hash_5381(char* ruta);

void cliente_C()
{
	long int cantidad_de_bits;
	long unsigned int long_buffer;
	long_buffer = handshake_cliente('C');

	char buffer[long_buffer];
	FILE *descarga;
	if((descarga = fopen("./db/base_de_datos_descargado.db", "wb")) == NULL)
	{
		perror("Error abriendo el punto de descarga:");
		exit(EXIT_FAILURE);
	}

	unsigned long hash,hash_recibido;
	
	cantidad_de_bits = recv( sockfd, buffer, long_buffer,0 );

	if ( cantidad_de_bits < 0 ) {
		perror( "escritura de socket" );
		close(sockfd);
		exit( 1 );
	}
	else if (cantidad_de_bits == 0) {
		printf("Server out");
		close(sockfd);
		exit(0);
	}
	
	hash_recibido = (unsigned long)atol(buffer);

	while((cantidad_de_bits = recv( sockfd, buffer, 1 ,0 )) > 0)
	{
		fputc ( buffer[0], descarga );
	}

	fclose(descarga);

	hash = generar_hash_5381("./db/base_de_datos_descargado.db");
	printf("Hash recibido: %lu\nHash generado: %lu\n",hash_recibido,hash);
	if (hash == hash_recibido) {
		printf("Archivo descargado con exito\n");
	}
	else
	{
		printf("Fallo en la descarga\n");
	}
}

void cliente_B()
{
	long int cantidad_de_bits;
	long unsigned int long_buffer;
	long_buffer = handshake_cliente('B');

	char buffer[long_buffer];

	while(1) {			 
		printf("sqlite3>");
		fgets(buffer,(int)long_buffer,stdin);

		cantidad_de_bits = send( sockfd, buffer, long_buffer,0 );

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
		memset(buffer,0,long_buffer);

		cantidad_de_bits = recv( sockfd, buffer, long_buffer,0 );
		if ( cantidad_de_bits < 0 ) {
			perror( "escritura de socket" );
			close(sockfd);
			exit( 1 );
		}
		else if (cantidad_de_bits == 0) {
			printf("Server out");
			close(sockfd);
			exit(0);
		}
		printf("%s\n",buffer);
	}
}

void cliente_A()
{
	long int cantidad_de_bits;
	long unsigned int long_buffer;
	long_buffer = handshake_cliente('A');

	char buffer[long_buffer];
	char query[long_buffer];

	memset(query,'\0',long_buffer);
	strcpy(query, "SELECT * FROM Log;");

	while(1) {			 
		cantidad_de_bits = send( sockfd, query, long_buffer,0 );

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
		memset(buffer,'\0',long_buffer);

		cantidad_de_bits = recv( sockfd, buffer, long_buffer,0 );
		if ( cantidad_de_bits < 0 ) {
			perror( "escritura de socket" );
			close(sockfd);
			exit( 1 );
		}
		else if (cantidad_de_bits == 0) {
			printf("Server out");
			close(sockfd);
			exit(0);
		}
		printf("%s\n",buffer);
		sleep(1);
	}
}

long unsigned int handshake_cliente(char tipo)
{
	long int cantidad_de_bits;
	char handshake[16];
	cantidad_de_bits = recv(sockfd, handshake, sizeof(handshake),0);

	if ( cantidad_de_bits < 0 ) {
		perror( "escritura de socket" );
		close(sockfd);
		exit( 1 );
	}
	else if (cantidad_de_bits == 0) {
		printf("Server out");
		close(sockfd);
		exit(0);
	}
	long unsigned int long_buffer = (long unsigned int)atol(handshake);

	handshake[0] = tipo;

	cantidad_de_bits = send(sockfd, handshake, 1 ,0);

	if ( cantidad_de_bits < 0 ) {
		perror( "escritura de socket" );
		close(sockfd);
		exit( 1 );
	}
	else if (cantidad_de_bits == 0) {
		printf("Server out");
		close(sockfd);
		exit(0);
	}

	return long_buffer;
}

long unsigned generar_hash_5381(char* ruta)
{
	FILE *archivo;

	if((archivo = fopen(ruta, "rb")) == NULL)
	{
		perror("Error abriendo la base de datos:");
		exit(EXIT_FAILURE);
	}


	char buffer[1];

	unsigned long hash = 5381;

	while ( (buffer[0] =(char) fgetc ( archivo )) != EOF ) {
			hash = ((hash << 5) + hash) + (unsigned long)buffer[0]; /* hash * 33 + c */
	}
	fclose(archivo);
	return hash;
}
