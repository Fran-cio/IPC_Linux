#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#define TAM 256

void gestion_de_los_mensajes(int,int);
void protocolo_ipv4(char*);
void protocolo_ipv6(char*);
void protocolo_unix(char * argv);
int socket_perror(int __domain, int __type, int __protocol);
void recibir_mensajes(struct sockaddr*);

int fd_socket
			,fd_sockect_nuevo
			,puerto;
unsigned int long_serv
							,long_cli;

int main( int argc, char *argv[] ) {
	int pid_prot;

	if ( argc < 4 ) {
		fprintf( stderr, "Ingrese: %s <archivo> <puerto>\n", argv[0] );
		exit( 1 );
	}

	pid_prot = fork();
	if (pid_prot == 0) {
		protocolo_ipv4(argv[2]);
		return 0;
	}
	pid_prot = fork();
	if (pid_prot == 0) {
		protocolo_ipv6(argv[3]);
		return 0;
	}
	protocolo_unix(argv[1]);

	return 0; 
}

void protocolo_ipv4(char* argv){
	struct sockaddr_in direccion_serv_ipv4, direccion_cli_ipv4;

	fd_socket = socket_perror(AF_INET, SOCK_STREAM, 0);

	memset( (char *) &direccion_serv_ipv4, 0, sizeof(direccion_serv_ipv4) );
	int puerto = atoi( argv );
	direccion_serv_ipv4.sin_family = AF_INET;
	direccion_serv_ipv4.sin_addr.s_addr = INADDR_ANY;
	direccion_serv_ipv4.sin_port = htons( puerto );

	if ( bind(fd_socket, ( struct sockaddr *) &direccion_serv_ipv4, 
				sizeof( direccion_serv_ipv4 ) ) < 0 ) {
		perror( "ligadura ipv4" );
		exit( 1 );
	}


	printf( "Proceso: %d - socket disponible: %d\n", getpid(),
			direccion_serv_ipv4.sin_port );

	listen( fd_socket, 5 );
	long_cli = sizeof( direccion_cli_ipv4 );


	recibir_mensajes(( struct sockaddr * )& direccion_cli_ipv4);
}

void protocolo_ipv6(char* argv){
	struct sockaddr_in6 direccion_serv_ipv6, direccion_cli_ipv6;

	fd_socket = socket_perror(AF_INET6, SOCK_STREAM, 0);

	memset( (char *) &direccion_serv_ipv6, 0, sizeof(direccion_serv_ipv6) );
	int puerto = atoi( argv );
	direccion_serv_ipv6.sin6_family = AF_INET6;
	direccion_serv_ipv6.sin6_addr = in6addr_any;
	direccion_serv_ipv6.sin6_port = htons( puerto );

	if ( bind(fd_socket, ( struct sockaddr *) &direccion_serv_ipv6, 
				sizeof( direccion_serv_ipv6 ) ) < 0 ) {
		perror( "ligadura ipv6" );
		exit( 1 );
	}


	printf( "Proceso: %d - socket disponible: %d\n", getpid(),
			direccion_serv_ipv6.sin6_port );

	listen( fd_socket, 5 );
	long_cli = sizeof( long_cli );

	recibir_mensajes(( struct sockaddr * )& direccion_cli_ipv6);
}

void protocolo_unix(char * argv){
	struct sockaddr_un direccion_serv_unix, direccion_cli_unix;

	fd_socket = socket_perror(AF_UNIX, SOCK_STREAM, 0);


	unlink(argv);
	memset( &direccion_serv_unix, 0, sizeof(direccion_serv_unix) );
	direccion_serv_unix.sun_family = AF_UNIX;
	strcpy( direccion_serv_unix.sun_path, argv );
	long_serv = strlen(direccion_serv_unix.sun_path) + sizeof(direccion_serv_unix.sun_family);

	if( bind(fd_socket, (struct sockaddr *) &direccion_serv_unix, long_serv) < 0 ){
		perror("ligadura Unix");
		exit(1);
	}

	printf( "Proceso: %d - socket disponible: %s\n", getpid(), argv );

	recibir_mensajes((struct sockaddr*) &direccion_cli_unix);	
}

void recibir_mensajes(struct sockaddr* direccion_cli){
	listen( fd_socket, 5 );
	long_cli = sizeof( *direccion_cli);
	while (1) {
		fd_sockect_nuevo = accept( fd_socket, 
				direccion_cli, &long_cli );

		int pid = fork();
		if (pid == 0) {
			gestion_de_los_mensajes(fd_socket,fd_sockect_nuevo);
		}
		else {

			printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid );
			close(fd_sockect_nuevo);

		}
	}
}
void gestion_de_los_mensajes(int fd_socket,int fd_socket_nuevo){

	int pid = fork(); 

	if ( pid == 0 )
	{  // Proceso hijo
		close( fd_socket );

		while ( 1 )
		{

			char buffer[TAM];
			memset( buffer, 0, TAM );

			int n = read( fd_socket_nuevo, buffer, TAM-1 );
			if ( n < 0 ) {
				perror( "lectura de socket" );
				exit(1);
			}

			printf( "PROCESO %d. ", getpid() );
			printf( "Recibí: %s", buffer );

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
	else {
		printf( "SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", 
				pid );
		close( fd_socket_nuevo );
	}
}

int socket_perror(int __domain, int __type, int __protocol){
	int fd_s = socket( __domain, __type, __protocol);

	if(fd_s <0){
		perror("Socket Error");
		exit(1);
	}

	return fd_s;
}

