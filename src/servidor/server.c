#include "../../include/servidor/protocolos.h"
#include "../../include/signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main( int argc, char *argv[] ) {
  int pid_prot;
  /*
         * Nos aseguramos que los valores obtenidos sean el numero correcto
  */
  if ( argc < 4 ) {
    fprintf( stderr, 
            "Ingrese: %s <archivo> <puerto ipv4> <puerto ipv6>\n",
            argv[0] );
    exit( EXIT_FAILURE );
  }

  iniciar_variables_globales();
  /*
         * Cada protocolo se deriva a un proceso hijo que se va a encargar
         * individualmente de configurar cada parametro y posteriormente
         * gestionar los mensajes
  */

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
  /*
         * El proceso padre se queda logeando los datos
  */

  set_func_sig(*func);//El proceso padre gestiona las interrupciones
  looger();
  sigHandler(SIGCHLD);//Limpiamos los procesos zombies

  return 0; 
}






