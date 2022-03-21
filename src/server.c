#include "../include/protocolos.h"
#include <stdio.h>

int main( int argc, char *argv[] ) {

	int pid_prot;
	if ( argc < 5 ) {
		fprintf( stderr, "Ingrese: %s <archivo> <puerto ipv4> <puerto ipv6>\n",
				argv[0] );
		exit( 1 );
	}

	iniciar_variables_globales(atoi(argv[4]));

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

	set_func_sig(*func);
	looger();
	sigHandler(SIGCHLD);

	return 0; 
}






