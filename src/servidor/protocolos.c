#include <netinet/in.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <unistd.h>
#include "../../include/servidor/estructura_protocolo.h"
#include "./manejo_del_server.c"

int puerto
		,esta_corriendo;

Protocolos *estrucura_prot; 

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

void asignar_segmento(void){
	estrucura_prot = (Protocolos *)mmap(NULL, sizeof(Protocolos), 
								PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	/*
	 * Se opto por una estructura de memoria compartida donde cada protocolo
	 * va a estar modificando su espacio de memoria asociado
	 */
	if(estrucura_prot == MAP_FAILED){
		perror("No se asigno el segmento");
		exit(EXIT_FAILURE);
	}
}

void iniciar_semaforo()
{
	semaforo = &estrucura_prot->semaforo;
	sem_unlink("servidor");
	*semaforo = sem_open("servidor", O_CREAT, 0, 1);
}

void iniciar_variables_globales(long unsigned int tam_buffer){
	if (tam_buffer == 0) {
		fprintf(stderr, "Tamaño de buffer invalido: %lu", tam_buffer);
	}
	long_buffer = tam_buffer;
	prctl(PR_SET_PDEATHSIG, SIGTERM);
	asignar_segmento();
	iniciar_semaforo();
	generarbasededatos();
}

/*
 * En cada protocolo se llenan los valores correspondientes dentro de la 
 * estructura y se bindea, posteriormente se va al manejo de los mensajes
 */
void protocolo_ipv4(char* argv){
	struct sockaddr_in direccion_serv_ipv4, direccion_cli_ipv4;

	fd_socket = socket_perror(AF_INET, SOCK_STREAM, 0);

	protocolo = "Ipv4";
	ratio = &estrucura_prot->ipv4;
	
	memset( (char *) &direccion_serv_ipv4, 0, sizeof(direccion_serv_ipv4) );
	short unsigned int puerto = (short unsigned int)atoi( argv );
	direccion_serv_ipv4.sin_family = AF_INET;
	direccion_serv_ipv4.sin_addr.s_addr = INADDR_ANY;
	direccion_serv_ipv4.sin_port = htons( puerto );

	if ( bind(fd_socket, ( struct sockaddr *) &direccion_serv_ipv4, 
				sizeof( direccion_serv_ipv4 ) ) < 0 ) {
		perror( "ligadura ipv4" );
		exit( 1 );
	}


	printf( "%s: PROCESO:%d - socket disponible: %d\n",protocolo, getpid(),
			puerto );

	listen( fd_socket, 5 );
	long_cli = sizeof( direccion_cli_ipv4 );


	recibir_mensajes(( struct sockaddr * )& direccion_cli_ipv4);
}

void protocolo_ipv6(char* argv){
	struct sockaddr_in6 direccion_serv_ipv6, direccion_cli_ipv6;

	protocolo = "Ipv6";
	ratio = &estrucura_prot->ipv6;

	fd_socket = socket_perror(AF_INET6, SOCK_STREAM, 0);

	memset( (char *) &direccion_serv_ipv6, 0, sizeof(direccion_serv_ipv6) );
	short unsigned int puerto = (short unsigned int)atoi( argv );
	direccion_serv_ipv6.sin6_family = AF_INET6;
	direccion_serv_ipv6.sin6_addr = in6addr_any;
	direccion_serv_ipv6.sin6_port = htons( puerto );

	if ( bind(fd_socket, ( struct sockaddr *) &direccion_serv_ipv6, 
				sizeof( direccion_serv_ipv6 ) ) < 0 ) {
		perror( "ligadura ipv6" );
		exit( 1 );
	}


	printf( "%s: PROCESO:%d - socket disponible: %d\n",protocolo, getpid(),
			puerto);

	listen( fd_socket, 5 );
	long_cli = sizeof( direccion_cli_ipv6 );

	recibir_mensajes(( struct sockaddr * )& direccion_cli_ipv6);
}

void protocolo_unix(char * argv){
	struct sockaddr_un direccion_serv_unix, direccion_cli_unix;
	char* path;

	protocolo = "unix";
	ratio = &estrucura_prot->uni;

	fd_socket = socket_perror(AF_UNIX, SOCK_STREAM, 0);

	/*
	 * La unica diferencia con los otros protocolos es que aca especificamos
	 * el path en el cual se va a guardar el ipc asociado al socket
	 */
	path = malloc((strlen(argv)+strlen("./ipc/")+1)*sizeof(char));
	path = strcpy(path, "./ipc/");
	path = strcat(path, argv);

	unlink(path);
	memset( &direccion_serv_unix, 0, sizeof(direccion_serv_unix) );
	direccion_serv_unix.sun_family = AF_UNIX;
	strcpy( direccion_serv_unix.sun_path, path );
	long_serv = strlen(direccion_serv_unix.sun_path) 
		+	sizeof(direccion_serv_unix.sun_family);

	if( bind(fd_socket, (struct sockaddr *) &direccion_serv_unix, (unsigned int)long_serv)<0){
		perror("ligadura Unix");
		exit(1);
	}

	printf( "%s: PROCESO:%d - socket disponible: %s\n",protocolo, getpid(), path);
	free(path);

	recibir_mensajes((struct sockaddr*) &direccion_cli_unix);	
}

/*
 * Esta funcion muestrea cada valor de la estrutura y hace los calculos para
 * logearlos. Posteriormente se resetean los valores
 */
void looger(void){
	long ipv4,
			 ipv6,
			 uni,
			 total;

	esta_corriendo=1;
	FILE* log;
	log = fopen("./log/log.txt", "w");
	if(log == NULL){
		perror("Log mal abierto");
		exit(EXIT_FAILURE);
	}
	char* formato = "\r===========================================================\n\
									 \ripv4: %lu MB/s\n\
									 \ripv6: %lu MB/s\n\
									 \runix: %lu MB/s\n\
									 \rtotal:%lu MB/s\n\
									 \r===========================================================\n";

	while ( esta_corriendo ) {
		ipv4 = estrucura_prot->ipv4/1000000;
		ipv6 = estrucura_prot->ipv6/1000000;
		uni = estrucura_prot->uni/1000000;
		
		total = ipv4 + ipv6 +uni;
		fprintf(log,formato,ipv4, ipv6, uni, total);

		//si se quiere ver en consola
		/*printf(formato,ipv4, ipv6, uni, total);*/ 
		sem_wait(*semaforo);	
		estrucura_prot->ipv4 = 0;
		estrucura_prot->ipv6 = 0;
		estrucura_prot->uni = 0; 
		sem_post(*semaforo);

		fflush(log);
		
		sleep(1);
	}
	fclose(log);
}

void func(int n){
	n &= n;
	printf("Fin de programa \n");
	esta_corriendo = 0;
}
