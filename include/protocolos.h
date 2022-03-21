#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void protocolo_ipv4(char*);
void protocolo_ipv6(char*);
void protocolo_unix(char * argv);
void iniciar_variables_globales(int); 
void looger(void);
void func(int);
void set_func_sig(__sighandler_t);
void sigHandler(int);
