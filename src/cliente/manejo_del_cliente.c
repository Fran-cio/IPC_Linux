#include "sys/socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int sockfd, terminar;

long unsigned int tam_buffer,servlen;

void mensajes_cli(void){
	char buffer[tam_buffer];
	long int n;

	terminar=0;
	for (long unsigned int i=0; i<tam_buffer; i++) {
		buffer[i] = (char)rand();
	}
	while(1) {			 
		printf("%s\n",buffer);
		n = send( sockfd, buffer, strlen(buffer),0 );
		if ( n < 0 ) {
			perror( "escritura de socket" );
			close(sockfd);
			exit( 1 );
		}
		else if (n == 0) {
			printf("Server out");
			close(sockfd);
			exit(0);
		}
	} 
}


