# Sistemas operativos 2 - Trabajo practico 2
## Informe
### Configuranciones iniciales
Se opto por ahcer un pool de conexiones con una base de datos.
Este pool se hizo con un arreglo de conexiones en memoria compartida con el parametro `SQLITE_OPEN_FULLMUTEX`, con el fin de evitar que la corrupcion de datos.
La base de datos se encuentra en la carpeta *db* y contiene una tabla hecha con juegos y su fecha de lanzamiento, y una tabla log donde el servidor va a guardar la las query que haga el tipo de cliente y los tiempos.
### Tablas de datos
#### Juegos
```sql
1|Assasins Creed|2007
2|Fallout 3|2008
3|The Elder Scrolls V: Skyrim|2011
4|Far Cry 3|2012
5|Grand Theft Auto V|2013
6|Red Dead Redemption|2010
7|Hollow Knight|2017
8|Cuphead|2017
9|The Witcher 3: Wild Hunt|2015
10|Super Mario Bros|1985
```
#### Log
```sql
B|Sun Apr  3 23:02:56 2022

C|Sun Apr  3 23:03:34 2022

B|Sun Apr  3 23:20:19 2022

B|Sun Apr  3 23:20:36 2022

C|Sun Apr  3 23:20:59 2022

C|Sun Apr  3 23:21:01 2022

C|Sun Apr  3 23:21:02 2022

C|Sun Apr  3 23:21:03 2022
```
### Asignacion de las funciones
A los clientes se les pasa por parametro el tipo de clientes que van a ser y acto seguido se les asigna una funcion, el fin de esto es abstraernos de la implementacion de cada cliente.
Ademas, de esta forma es posible no tocar la base que es el TP1, ya que los protocolos y su asignacion no se modifica.
### Avisos
La decision de que el archivo se envie byte a byte, es con el fin de evitar tener problemas con el tamanio de los buffers, desconozco las consecuencias de esta implementacion.
Se implemento una funcion hash que se obtuvo de la siguiente [pagina](http://www.cse.yorku.ca/~oz/hash.html "Hash").
### Uso:
El servidor y el cliente se pueden generar con el makefile haciendo uso de lo comandos `make server` o `make cliente` y para generar ambos `make all`.
Los binarios, objectos, base de datos y librerias se guardan en los respectivos directorios asociados a los mismos.

Tanto el server como el cliente va a imprimir el formato de ingreso de parametros al ser ejecutados sin parametros.
