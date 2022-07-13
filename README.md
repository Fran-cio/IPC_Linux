# Sistemas operativos 2 - Trabajo practico 1
## Informe
### Configuranciones iniciales
Los protocolos elegidos fueron **ipv4**, **ipv6** y **unix**.
Cada uno se implemento de tanto en servidor como cliente, de tal manera que desde una clase principal se lanzan los procesos correspondiente a cada uno de estos, en el servidor se derivan procesos hijos por protocolos y en el cliente se ejecuta uno u otro en base a los parametros enviados.
En ambos se configura cada protocolo por separado (configuracion de entructura, bind(), socket(), etc).
Llegado el momento cada protocolo maneja los mensajes de la misma manera, esto lo implemente modulizando el comportamiento de cada etapa.
### Logger
Ademas de los procesos anteriores, el servidor cuenta con un logger que va guardando el ancho de banda de cada protocolo en Megabytes por segundo. 
### Memoria compartida
Para el logger tener la informacion de cada ancho de banda se genero una estructura:
```c
typedef struct Protocolos{
  long ipv4;
  long ipv6;
  long uni;
}Protocolos;
```
El cual se genera un segmento de memoria compartido y se comparte entre los procesos, a cada protocolo se le asigna un puntero de memoria. De esa forma cada protocolo acumula el numero de bytes recibidos en ella.
### Calculo del ancho de banda
El logger cada 1 segundo, muestrea los valores y los manipula para mostrarnos, posteriormente setea en 0 cada contador para volver esperar 1 segundo y asi obtener la carga recibida en bytes por segundo.
### Parametros
Se opto por la menor cantidad de parametros estaticos, de tal forma que el usuario es capaz de seleccionar los puertos de los protocolos y el archivo que se usa para el socket unix, ademas del tamanio del buffer, esto para el caso del servidor. Para el cliente se puede elegir el protocolo, el tamanio de buffer, el host y el puerto al cual conectarse en caso de internet, y para unix el nombre del archivo. Para el server se uso la etiqueta ANADDR_ANY, que en palabras del manual significa:

`When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces.`

Es decir, conecto a todas las interfaces locales, esto tambien se podria parametrizar facilmenente.

### Uso:
El servidor y el cliente se pueden generar con el makefile haciendo uso de lo comandos `make server` o `make cliente` y para generar ambos `make all`.
Los binarios, objectos y librerias se guardan en los respectivos directorios asociados a los mismos.

Tanto el server como el cliente va a imprimir el formato de ingreso de parametros al ser ejecutados sin parametros.
