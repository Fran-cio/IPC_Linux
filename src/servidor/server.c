#include "../../include/servidor/protocolos.h"
#include "../../include/signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc, char *argv[] ) {
	int pid_prot;
	if ( argc < 5 ) {
		fprintf( stderr, "Ingrese: %s <archivo> <puerto ipv4> <puerto ipv6> <tamaño de buffer>\n", argv[0] );
		exit( EXIT_FAILURE );
	}

	iniciar_variables_globales(atoi(argv[4]));

	pid_prot = fork_con_errno();
	if (pid_prot == 0) {
		protocolo_ipv4(argv[2]);
		return 0;
	}

	pid_prot = fork_con_errno();
	if (pid_prot == 0) {
		protocolo_ipv6(argv[3]);
		return 0;
	}

	pid_prot = fork_con_errno();
	if (pid_prot == 0) {
		protocolo_unix(argv[1]);
		return 0;
	}

	set_func_sig(*func);
	looger();
	sigHandler(SIGCHLD);

	return 0; 
}






