#include <semaphore.h>

typedef struct Protocolos{
	long ipv4;
  long ipv6;
	long uni;
	sem_t *semaforo;
}Protocolos;


