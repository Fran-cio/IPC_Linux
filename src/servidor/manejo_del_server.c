#include "base_de_datos.c"

#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <unistd.h>

int	fd_socket
		,fd_socket_nuevo;

long unsigned int long_buffer,
		 					long_serv,
							long_cli;

char *protocolo, *mensaje;

long *ratio;

sem_t** semaforo;

struct timeval stop, start;

int callback(void *NotUsed, int argc, char **argv, char **azColName);
int rutina_sql(char* buffer);

pid_t fork_con_errno(void){
	pid_t fd = fork();
	if(fd == -1){
		perror("Fallo en el fork");
		exit(EXIT_FAILURE);
	}
	return fd;
}

/*
 * Los mensajes se almacenan en un buffer con el tamaño pasado como parametro
 * posteriorente se lee el mensaje, si todo salio bien, la cantidad de datos
 * guardados se suman a la de cada protocolo
 */
void gestion_de_los_mensajes(int fd_socket,int fd_socket_nuevo)
{
	long int cantidad_de_bits;
	close( fd_socket );

	char buffer[long_buffer];

	memset(buffer, '\0', long_buffer);
	sprintf(buffer, "%lu", long_buffer);

	cantidad_de_bits = send(fd_socket_nuevo, buffer, strlen(buffer), 0);

	if ( cantidad_de_bits < 0 ) {
		perror( "fallo en handshake" );
		return;	
	}
	else if (cantidad_de_bits == 0) {
		close(fd_socket_nuevo);
		printf( "PROCESO %d. termino la ejecución.\n\n", 
				getpid() );
		return;
	}
	while ( 1 )
	{
		memset(buffer, '\0', long_buffer);

		cantidad_de_bits = recv( fd_socket_nuevo, buffer, long_buffer,0);

		if (cantidad_de_bits <= 0) {
			close(fd_socket_nuevo);
			printf( "PROCESO %d. termino la ejecución.\n\n", 
					getpid() );
			break;
		}

		printf("%s\n",buffer);

		rutina_sql(buffer);

		sem_wait(*semaforo);
		*ratio += cantidad_de_bits;
		sem_post(*semaforo);
	}
}
/*
 * Cada vez que un cliente conecte, la funcion accept deriva este socket a un
 * proceso hijo
 */
void recibir_mensajes(struct sockaddr* direccion_cli)
{
	listen( fd_socket, 5 );
	long_cli = sizeof( *direccion_cli);
	while ( 1 ) {
		fd_socket_nuevo = accept( fd_socket, direccion_cli,
				(unsigned int*)&long_cli );

		int pid = fork_con_errno();
		if (pid == 0) {
			prctl(PR_SET_PDEATHSIG, SIGTERM);
			gestion_de_los_mensajes(fd_socket,fd_socket_nuevo);
			break;
		}
		else {
			printf( "SERVIDOR: Nuevo cliente %s, que atiende el proceso hijo: %d\n", 
					protocolo , pid);
			close(fd_socket_nuevo);
		}
	}
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	NotUsed = 0;
	NotUsed = NotUsed;

	char *columna;

	columna = malloc(long_buffer);

	for (int i = 0; i < argc; i++) {
		sprintf(columna,"%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		mensaje = strcat(mensaje, columna);	
	}

	free(columna);

	return 0;
}

int rutina_sql(char* buffer)
{
	char* err_msg;
	mensaje = malloc(long_buffer);
	int rc = sqlite3_exec(obtener_conexion(), buffer, callback, 0, &err_msg);

	printf("%s\n",mensaje);

	if (rc != SQLITE_OK ) {
		strcpy(mensaje,err_msg);
	}
	if(strlen(mensaje)==0)
	{
		mensaje[0] = ' '; 
	}

	long int cantidad_de_bits = send(fd_socket_nuevo, mensaje, long_buffer, 0);

	if (cantidad_de_bits <= 0) {
		close(fd_socket_nuevo);

		return EXIT_FAILURE;
	}
	free(mensaje);
	return EXIT_SUCCESS;
}
