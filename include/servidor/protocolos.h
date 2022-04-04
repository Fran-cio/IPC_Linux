#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void protocolo_ipv4(char*);
void protocolo_ipv6(char*);
void protocolo_unix(char * argv);
void iniciar_variables_globales(); 
void looger(void);
void func(int);
pid_t fork_con_errno(void);
