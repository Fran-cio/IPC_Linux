#include <bits/time.h>
#include <bits/types/struct_timeval.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include "signals.c"
#include <sys/ipc.h>
#include <sys/shm.h>
#define TAM 256

void gestion_de_los_mensajes(int,int);
 
void protocolo_ipv4(char*);
void protocolo_ipv6(char*);
void protocolo_unix(char * argv);

int socket_perror(int __domain, int __type, int __protocol);
void recibir_mensajes(struct sockaddr*);
void setear_tiempo(void);
void func(int);
void looger(void);

typedef struct Protocolos{
	long ipv4;
  long ipv6;
	long uni;
}Protocolos;

int fd_socket
		,fd_socket_nuevo
		,puerto
		,esta_corriendo;

char *protocolo;
long *ratio;

unsigned int long_serv
							,long_cli;

Protocolos *estrucura_prot; 

struct timeval stop, start;

int main( int argc, char *argv[] ) {
	esta_corriendo=1;
	int memoria_compartida = shmget(ftok("../ipc", 'S'),sizeof(*estrucura_prot),
			(IPC_CREAT | 0660));
	if(memoria_compartida < 0){
		perror("memoria_compartida fallo");
		exit(EXIT_FAILURE);
	}

	estrucura_prot = shmat(memoria_compartida, 0, 0);
	if(estrucura_prot == (void* )-1){
		perror("No se asigno el segmento");
		exit(EXIT_FAILURE);
	}
	int pid_prot;
	if ( argc < 4 ) {
		fprintf( stderr, "Ingrese: %s <archivo> <puerto ipv4> <puerto ipv6>\n",
				argv[0] );
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
	pid_prot = fork();
	if (pid_prot == 0) {
		protocolo_unix(argv[1]);
		return 0;
	}

	/*set_func_sig(*func);*/
	looger();
	
	sigHandler(SIGCHLD);
	return 0; 
}

void protocolo_ipv4(char* argv){
	struct sockaddr_in direccion_serv_ipv4, direccion_cli_ipv4;

	fd_socket = socket_perror(AF_INET, SOCK_STREAM, 0);

	protocolo = "Ipv4";
	ratio = &estrucura_prot->ipv4;
	printf("ipv4%lu \n",*ratio);


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


	printf( "%s: %d - socket disponible: %d\n",protocolo, getpid(),
			direccion_serv_ipv4.sin_port );

	listen( fd_socket, 5 );
	long_cli = sizeof( direccion_cli_ipv4 );


	recibir_mensajes(( struct sockaddr * )& direccion_cli_ipv4);
}

void protocolo_ipv6(char* argv){
	struct sockaddr_in6 direccion_serv_ipv6, direccion_cli_ipv6;

	protocolo = "Ipv6";
	ratio = &estrucura_prot->ipv6;
	printf("ipv6%lu \n",*ratio);


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


	printf( "%s: %d - socket disponible: %d\n",protocolo, getpid(),
			direccion_serv_ipv6.sin6_port );

	listen( fd_socket, 5 );
	long_cli = sizeof( direccion_cli_ipv6 );

	recibir_mensajes(( struct sockaddr * )& direccion_cli_ipv6);
}

void protocolo_unix(char * argv){
	struct sockaddr_un direccion_serv_unix, direccion_cli_unix;

	protocolo = "unix";
	ratio = &estrucura_prot->uni;

	printf("unix %lu \n",*ratio);
	fd_socket = socket_perror(AF_UNIX, SOCK_STREAM, 0);

	unlink(argv);
	memset( &direccion_serv_unix, 0, sizeof(direccion_serv_unix) );
	direccion_serv_unix.sun_family = AF_UNIX;
	strcpy( direccion_serv_unix.sun_path, argv );
	long_serv = strlen(direccion_serv_unix.sun_path) 
		+	sizeof(direccion_serv_unix.sun_family);

	if( bind(fd_socket, (struct sockaddr *) &direccion_serv_unix, long_serv)<0){
		perror("ligadura Unix");
		exit(1);
	}

	printf( "%s: %d - socket disponible: %s\n",protocolo, getpid(), argv );

	recibir_mensajes((struct sockaddr*) &direccion_cli_unix);	
}

void recibir_mensajes(struct sockaddr* direccion_cli){
	listen( fd_socket, 5 );
	long_cli = sizeof( *direccion_cli);
	while ( 1 ) {
		fd_socket_nuevo = accept( fd_socket, 
				direccion_cli, &long_cli );

		int pid = fork();
		if (pid == 0) {
			gestion_de_los_mensajes(fd_socket,fd_socket_nuevo);
		}
		else {
			printf( "SERVIDOR: Nuevo cliente %s, que atiende el proceso hijo: %d\n", protocolo,pid);
			close(fd_socket_nuevo);
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


			gettimeofday(&start, NULL);
			int n = read( fd_socket_nuevo, buffer, TAM-1 );
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

void looger(void){
	FILE* log;
	log = fopen("../log/log.txt", "w");
	if(log == NULL){
		perror("Log mal abierto");
		exit(EXIT_FAILURE);
	}
	char * texto;

	while ( esta_corriendo ) {
		sprintf(texto, "\r===========================================================\n\
										\ripv4: %lu b/s\n\
										\ripv6; %lu b/s\n\
										\runix: %lu b/s\n\
										\rtotal:%lu b/s\n\
										\r===========================================================\n"
										,estrucura_prot->ipv4, estrucura_prot->ipv6, estrucura_prot->uni,
										estrucura_prot->ipv4 + estrucura_prot->ipv6 + estrucura_prot->uni);
		printf("%s",texto);	
		sleep(5);
	}
	fclose(log);
}

void func(int n){
	printf("Fin de programa \n");
	esta_corriendo = 0;
}
