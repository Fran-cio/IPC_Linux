#include "../../include/sqlite3.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define CONEXIONES 5

sqlite3 **db;

void cerra_conexiones(int);

/*
 * Se genenera un arreglo de conexiones a la base de datos, y genera un
 * espacio de memoria compartido, y se conectan cada uno al archivo en la
 * carpeta db.
 *
 * La base de configura con el parametro full mutex, de tal forma que no
 * se generen corrupcion en los datos que se escriban o lean.
 */

void generarbasededatos()
{
	db = ( sqlite3** )mmap(NULL, sizeof(db) * CONEXIONES, 
								PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(db == MAP_FAILED)
	{
		perror("Mapeo de memoria erroneo");
		exit(EXIT_FAILURE);
	}
	int rc;
	for (int i=0; i<CONEXIONES; i++) 
	{
		db[i] = NULL;
		rc = sqlite3_open_v2("./db/base_de_datos.db", &(db[i]),
				SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
		if (rc != SQLITE_OK) 
		{
			fprintf(stderr, "No se pudo abir la base de datos %d: %s\n",
					i,	sqlite3_errmsg(db[i]));
			cerra_conexiones(i);

			exit(EXIT_FAILURE);
		}
	}
}

void cerra_conexiones(int numero)
{
	for(int i=0; i<numero; i++)
	{
		sqlite3_close(db[i]);
	}
}

/*
 * La politica del pool de conexiones es un simple random que devuelve una
 * de las conexiones
 */

sqlite3* obtener_conexion(){
	return db[(int)rand()%CONEXIONES];
}

