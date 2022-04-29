#include "base_de_datos.c"
#include "../utils.c"

#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <unistd.h>
#include<time.h>

int	fd_socket
		,fd_socket_nuevo;

long unsigned int	long_serv,
							long_cli;

char *protocolo, *mensaje;
char tipo[1];	

long unsigned *ratio;

sem_t** semaforo;

struct timeval stop, start;

int callback(void *NotUsed, int argc, char **argv, char **azColName);
int rutina_sql(char* buffer);
void handshake_tipo_server(char[]);
void logear_en_db();
void para_Cliente_AB();
void para_Cliente_C();

/*
 * Los mensajes se almacenan en un buffer con el tamaño pasado como parametro
 * posteriorente se lee el mensaje, si todo salio bien, la cantidad de datos
 * guardados se suman a la de cada protocolo
 */
void gestion_de_los_mensajes()
{
	close( fd_socket );

	handshake_tipo_server(tipo);

	printf("El tipo de cliente conectado es: %s\n",tipo);

	if (tipo[0] == 'C') 
	{
		para_Cliente_C();
	}
	else
	{
		para_Cliente_AB();
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
			gestion_de_los_mensajes();
			break;
		}
		else {
			printf( "SERVIDOR: Nuevo cliente %s, que atiende el proceso hijo: %d\n", 
					protocolo , pid);
			close(fd_socket_nuevo);
		}
	}
}

/*
 * Esta funcion es llamada en cada lectura de la base de datos, simplemente
 * se genera un string que se concatena con uno mas grande el cual se va 
 * a devolver atravez de la base de datos.
 */

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	NotUsed = 0;
	NotUsed = NotUsed;

	char *columna;

	columna = malloc(512);

	for (unsigned int i = 0; (int)i < argc; i++) {
		sprintf(columna,"%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

		mensaje = realloc_char_perror(mensaje,strlen(mensaje)+strlen(columna));

		mensaje = strcat(mensaje, columna);	

	}

	free(columna);

	return 0;
}

/*
 * Parte de la consigan pedia logear en la base, las query que haga los cliente
 * de tipo C y B. Se guarda el tipo y la hora.
 */

void logear_en_db()
{
	char* err_msg;

	time_t t;   // not a primitive datatype
	time(&t);

	char sql[128]; 
	sprintf(sql,"INSERT INTO LOG  VALUES('%s','%s')",tipo,ctime(&t));
	int rc = sqlite3_exec(obtener_conexion(), sql, callback, 0, &err_msg);

	if (rc != SQLITE_OK ) {
		strcpy(mensaje,err_msg);
	}

}

int rutina_sql(char* buffer)
{
	char* err_msg;

	if(tipo[0] == 'B'){
		logear_en_db();
	}

	mensaje = malloc(128);
	memset(mensaje, '\0', 128);
        int rc;
       	do 
        {
          rc = sqlite3_exec(obtener_conexion(), buffer, callback, 0, &err_msg);
        }while(rc == SQLITE_BUSY || rc == SQLITE_LOCKED);


	if (rc != SQLITE_OK ) {
		strcpy(mensaje,err_msg);
	}
	if(strlen(mensaje)==0)
	{
		mensaje[0] = ' '; 
	}

	handshake_send_tam_buffer(strlen(mensaje),fd_socket_nuevo);

	long int cantidad_de_bits = send(fd_socket_nuevo, mensaje, strlen(mensaje), 0);

	if (cantidad_de_bits <= 0) {
		close(fd_socket_nuevo);
		return EXIT_FAILURE;
	}
	free(mensaje);
	return EXIT_SUCCESS;
}

/*
 * El server le dice el tamanio del buffer que van a usar y se entera del tipo
 * de cliente con quien esta tratando.
 */

void handshake_tipo_server(char tipo[])
{
	long int cantidad_de_bits;

	cantidad_de_bits = recv(fd_socket_nuevo, tipo, 1 ,0);

	if ( cantidad_de_bits < 0 ) {
		perror( "escritura de socket" );
		close(fd_socket_nuevo);
		exit( 1 );
	}
	else if (cantidad_de_bits == 0) {
		printf("Server out");
		close(fd_socket_nuevo);
		exit(0);
	}
}

/*
 * A los clientes A y B se los trata de igual manera
 */

void para_Cliente_AB()
{
	long int cantidad_de_bits;
	long unsigned int long_buffer;
	char *buffer = malloc(1);
	while ( 1 )
	{
		long_buffer = handshake_recv_tam_buffer(fd_socket_nuevo);

		buffer = realloc_char_perror(buffer, long_buffer);

		memset(buffer, '\0', long_buffer);

		cantidad_de_bits = recv( fd_socket_nuevo, buffer, long_buffer,0);

		if (cantidad_de_bits <= 0) {
			close(fd_socket_nuevo);
			printf( "PROCESO %d. termino la ejecución.\n\n", 
					getpid() );
			break;
		}

		rutina_sql(buffer);

		sem_wait(*semaforo);
		*ratio += (unsigned long) cantidad_de_bits;
		sem_post(*semaforo);
	}
}

/*
 * Al cliente C se le envia el checksum y el archivo a posterior
 */

void para_Cliente_C()
{
	logear_en_db();

	FILE *descarga;

	if((descarga = fopen("./db/base_de_datos.db", "rb")) == NULL)
	{
		perror("Error abriendo la base de datos:");
		exit(EXIT_FAILURE);
	}

	char buffer[256];

	unsigned long long hash;

	hash = generar_hash_djb2("./db/base_de_datos.db");

	memset(buffer, '\0', 256);
	sprintf(buffer, "%llu", hash);

	handshake_send_tam_buffer(strlen(buffer), fd_socket_nuevo);

        long int cantidad_de_bits = send( fd_socket_nuevo,
            buffer, strlen(buffer) ,0);
        if (cantidad_de_bits <= 0) {
          close(fd_socket_nuevo);
          printf( "PROCESO %d. termino la ejecución.\n\n", 
              getpid() );
          exit(EXIT_FAILURE);

          sem_wait(*semaforo);
          *ratio += (unsigned long)cantidad_de_bits;
          sem_post(*semaforo);
        }

        while ( (buffer[0] =(char) fgetc ( descarga )) != EOF ) {
          long int cantidad_de_bits = send( fd_socket_nuevo,
              buffer, 1 ,0);

          if (cantidad_de_bits <= 0) {
            close(fd_socket_nuevo);
            printf( "PROCESO %d. termino la ejecución.\n\n", 
                getpid() );
            exit(EXIT_FAILURE);

            sem_wait(*semaforo);
            *ratio += (unsigned long)cantidad_de_bits;
            sem_post(*semaforo);
          }
        }

        fclose(descarga);
}
