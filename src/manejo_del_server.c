#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int	fd_socket
		,fd_socket_nuevo;

long unsigned int long_buffer;

long unsigned int long_serv
							,long_cli;

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
		close( fd_socket );
		while ( 1 )
		{
			char buffer[long_buffer];
			memset( buffer, 0, long_buffer );

			gettimeofday(&start, NULL);
			long int n = read( fd_socket_nuevo, buffer, long_buffer-1 );
			if ( n < 0 ) {
				perror( "lectura de socket" );
				exit(1);
			}
			gettimeofday(&stop, NULL);
			long time_us=(stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
			*ratio = 1000000*(n-1)/time_us;
			n = write( fd_socket_nuevo, "Obtuve su mensaje", 18 );
			if ( n < 0 ) {
				perror( "escritura en socket" );
				exit( 1 );
			}
			// Verificación de si hay que terminar
			buffer[strlen(buffer)-1] = '\0';
			if( !strcmp( "fin", buffer ) ) {
				printf( "PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", 
						getpid() );
				exit(0);
			}
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
		}
		else {
			printf( "SERVIDOR: Nuevo cliente %s, que atiende el proceso hijo: %d\n", protocolo,pid);
			close(fd_socket_nuevo);
		}
	}
}

