#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int	fd_socket
		,fd_socket_nuevo;

long unsigned int long_buffer,
		 					long_serv,
							long_cli;

char *protocolo;

long *ratio;

struct timeval stop, start;

pid_t fork_con_errno(void){
	pid_t fd = fork();
	if(fd == -1){
		perror("Fallo en el fork");
		exit(EXIT_FAILURE);
	}
	return fd;
}

void gestion_de_los_mensajes(int fd_socket,int fd_socket_nuevo){
	long int n;
	close( fd_socket );

	char buffer[long_buffer];
	while ( 1 )
	{
		gettimeofday(&start, NULL);
		n = recv( fd_socket_nuevo, buffer, strlen(buffer),MSG_DONTWAIT);
		if ( n < 0 ) {
			perror( "lectura de socket" );
			break;
		}
		else if (n == 0) {
			close(fd_socket_nuevo);
			printf( "PROCESO %d. termino la ejecución.\n\n", 
					getpid() );
			break;
		}
		gettimeofday(&stop, NULL);
		long time_us=(stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
		*ratio = 1000000*(n-1)/time_us;
	}
}

void recibir_mensajes(struct sockaddr* direccion_cli){
	listen( fd_socket, 5 );
	long_cli = sizeof( *direccion_cli);
	while ( 1 ) {
		fd_socket_nuevo = accept( fd_socket, 
				direccion_cli, (unsigned int*)&long_cli );

		int pid = fork_con_errno();
		if (pid == 0) {
			gestion_de_los_mensajes(fd_socket,fd_socket_nuevo);
			break;
		}
		else {
			printf( "SERVIDOR: Nuevo cliente %s, que atiende el proceso hijo: %d\n", protocolo,pid);
			close(fd_socket_nuevo);
		}
	}
}

