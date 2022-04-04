#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * Es una funcion Hash que saque de internet
 */

long unsigned generar_hash_djb2(char* ruta)
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
		hash = ((hash << 5) + hash) +(unsigned long) buffer[0]; /* hash * 33 + c */
	}
	fclose(archivo);
	return hash;
}

pid_t fork_con_errno(void){
	pid_t fd = fork();
	if(fd == -1){
		perror("Fallo en el fork");
		exit(EXIT_FAILURE);
	}
	return fd;
}

int socket_perror(int __domain, int __type, int __protocol){
	int fd_s = socket( __domain, __type, __protocol);
	int flag = 1;

	if(fd_s <0){
		perror("Socket Error");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(fd_s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
		perror("Setsockopt error");
		exit(EXIT_FAILURE);
	}

	return fd_s;
}

long unsigned int handshake_recv_tam_buffer(int socketfd)
{
	long int cantidad_de_bits;
	char handshake[16];
	cantidad_de_bits = recv(socketfd, handshake, sizeof(handshake),0);

	if ( cantidad_de_bits < 0 ) {
		perror( "escritura de socket" );
		close(socketfd);
		exit( 1 );
	}
	else if (cantidad_de_bits == 0) {
		printf("Server out");
		close(socketfd);
		exit(0);
	}

	long unsigned int long_buffer = (long unsigned int)atol(handshake);
	printf("\nrecibe: %s, en numero: %lu\n",handshake,long_buffer);

	return long_buffer;
}

void handshake_send_tam_buffer(long unsigned int long_buffer,int socketfd)
{
	long int cantidad_de_bits;
	char handshake[16];

	memset(handshake, '\0', 16);
	sprintf(handshake, "%lu              ", long_buffer);

	cantidad_de_bits = send(socketfd, handshake, strlen(handshake), 0);

	printf("\nmanda: %s\n",handshake);

	if ( cantidad_de_bits < 0 ) {
		perror( "fallo en handshake" );
		return;	
	}
	else if (cantidad_de_bits == 0) {
		close(socketfd);
		printf( "PROCESO %d. termino la ejecución.\n\n", 
				getpid() );
		return;
	}
}
char* realloc_char_perror(char* mensaje, long unsigned int tam_nuevo)
{
	char *p = realloc(mensaje, tam_nuevo);
	if (!p) {
		perror("realloc fallo");
		exit(EXIT_FAILURE);
	} else {
		return p;
	}
}
