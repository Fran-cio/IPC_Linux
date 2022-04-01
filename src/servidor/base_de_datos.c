#include "../../include/sqlite3.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define CONEXIONES 5

sqlite3 **db;

void cerra_conexiones(int);

void generarbasededatos()
{
	db = ( sqlite3** )mmap(NULL, sizeof(db) * CONEXIONES, 
								PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	printf("%lu \n",sizeof(db));

	if(db == MAP_FAILED)
	{
		perror("Mapeo de memoria erroneo");
		exit(EXIT_FAILURE);
	}
	int rc;
	for (int i=0; i<CONEXIONES; i++) 
	{
		db[i] = NULL;
		rc = sqlite3_open_v2("./db/base_de_datos.db", &(db[i]), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
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

sqlite3* obtener_conexion(){
	return db[(int)rand()%5];
}

