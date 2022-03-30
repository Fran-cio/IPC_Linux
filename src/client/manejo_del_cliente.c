#include "sys/socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int sockfd, terminar;

long unsigned int servlen;

void mensajes_cli(void){

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
	for (long unsigned int i=0; i<long_buffer; i++) {
		buffer[i] = (char)rand();
	}
	while(1) {			 
		cantidad_de_bits = send( sockfd, buffer, long_buffer,0 );
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
	} 
}


