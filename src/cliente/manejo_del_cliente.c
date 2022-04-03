#include "sys/socket.h"

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int sockfd, terminar;

long unsigned int servlen;

void (*funcion)() = NULL;

void mensajes_cli(void){
	funcion();	
}

void cliente_A()
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

	char buffer[long_buffer];

	terminar=0;


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
		printf("%s\n",buffer);
	}
}
