#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./protocolos_cli.c"

void salida_error(char*);

int main( int argc, char *argv[] ){
	
	if ( argc < 3 ) {
		salida_error(argv[0]);
	}
	
	iniciar_variables_globales((long unsigned int)atoll(argv[2]));

	if (!strcmp(argv[1], "ipv4") & (argc<6)) {
		ipv4_cli(argv[3], (short unsigned int)atoi(argv[4]));
	}
	else if (!strcmp(argv[1], "ipv6")&(argc<7)) {
		ipv6_cli(argv[3], (short unsigned int)atoi(argv[4]));
	}
	else if (!strcmp(argv[1], "unix")) {
		unix_cli(argv[3]);
	}
	else {
		salida_error(argv[0]);
	}
	return 0;
}

void salida_error(char *argv){
	fprintf( stderr, "\rIngrese: %s <protocolo> <tamaño de buffer> <puerto> Si es por internet\n\
			\r%s <protocolo> <tamaño de buffer> <archivo> si es por unix", argv,argv );
	exit( EXIT_FAILURE );

}
