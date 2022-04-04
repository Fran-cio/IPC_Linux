#include <semaphore.h>

typedef struct Protocolos{
	long unsigned ipv4;
  long unsigned ipv6;
	long unsigned uni;
	sem_t *semaforo;
}Protocolos;


