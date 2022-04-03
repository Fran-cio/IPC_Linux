#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./protocolos_cli.c"

void salida_error(char*);

int main( int argc, char *argv[] ){
	
	if ( argc < 2 ) {
		salida_error(argv[0]);
	}
	/*
	 * Segun los parametros de entrada se decide que protocolo usara el cliente
	 * con sus respectivos parametros asociados
	 */

	if (!strcmp(argv[1], "ipv4") & (argc == 4 )) {
		ipv4_cli(argv[2], (short unsigned int)atoi(argv[3]));
	}
	else if (!strcmp(argv[1], "ipv6")&(argc == 4)) {
		ipv6_cli(argv[2], (short unsigned int)atoi(argv[3]));
	}
	else if ((!strcmp(argv[1], "unix") & (argc == 3))) {
		unix_cli(argv[2]);
	}
	else {
		salida_error(argv[0]);
	}
	return 0;
}

void salida_error(char *argv){
	fprintf( stderr, "\n\
			\rIngrese:\n\
			\r%s <protocolo> <ip> <puerto> Si es por internet\n\
			\r%s <protocolo> <archivo> si es por unix", argv,argv );
	exit( EXIT_FAILURE );
}
