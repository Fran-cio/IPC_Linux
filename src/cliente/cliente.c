#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./protocolos_cli.c"

void salida_error(char*);

int main( int argc, char *argv[] ){
	if ( argc < 3 ) {
		salida_error(argv[0]);
	}
	/*
	 * Segun los parametros de entrada se decide que protocolo usara el cliente
	 * con sus respectivos parametros asociados
	 */
	asignar_tipo_cliente(argv[2]);

	if (!strcmp(argv[1], "ipv4") & (argc == 5 )) {
		ipv4_cli(argv[3], (short unsigned int)atoi(argv[4]));
	}
	else if (!strcmp(argv[1], "ipv6")&(argc == 5)) {
		ipv6_cli(argv[3], (short unsigned int)atoi(argv[4]));
	}
	else if ((!strcmp(argv[1], "unix") & (argc == 4))) {
		unix_cli(argv[3]);
	}
	else {
		salida_error(argv[0]);
	}
	return 0;
}

void salida_error(char *argv){
	fprintf( stderr, "\n\
			\rIngrese:\n\
			\r%s <protocolo> <tipo de cliente> <ip> <puerto> Si es por internet\n\
			\r%s <protocolo> <tipo de cliente> <archivo> si es por unix", argv,argv );
	exit( EXIT_FAILURE );
}
