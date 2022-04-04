#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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
