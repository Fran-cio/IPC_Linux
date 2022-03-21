CC=gcc
CFLAGS=-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11
PATHh=./include/
PATHrec=./src/
PATHout=./out/
PATHlib=./lib/
PATHbin=./bin/

server: $(PATHbin)server

main_gdb: $(PATHrec)server.c $(PATHrec)protocolos.c 
	mkdir -p $(PATHbin)
	$(CC) $(CFLAGS) -o $(PATHbin)main_gdb main.c $(PATHrec)funciones.c -g
	
$(PATHbin)server: $(PATHout)server.o $(PATHlib)lib_tp1.a 
	mkdir -p $(PATHbin) ./log ./ipc 
	$(CC) $(CFLAGS) -o $(PATHbin)server $(PATHout)server.o -L$(PATHlib) -l_tp1
	
$(PATHlib)lib_tp1.a: $(PATHout)protocolos.o $(PATHout)signals.o 
	mkdir -p $(PATHlib) 
	ar cr $(PATHlib)lib_tp1.a $(PATHout)protocolos.o $(PATHout)signals.o 

$(PATHout)server.o: $(PATHrec)server.c  
	mkdir -p $(PATHout)
	$(CC) $(CFLAGS) -c $(PATHrec)server.c
	mv ./server.o $(PATHout)/server.o

$(PATHout)protocolos.o: $(PATHrec)protocolos.c $(PATHrec)manejo_del_server.c   
	$(CC) $(CFLAGS) -c $(PATHrec)protocolos.c 
	mv ./protocolos.o $(PATHout)

$(PATHout)signals.o: $(PATHrec)signals.c  
	$(CC) $(CFLAGS) -c $(PATHrec)signals.c 
	mv ./signals.o $(PATHout)

clean:
	rm -f -d $(PATHlib)* $(PATHlib) $(PATHout)* $(PATHout) $(PATHbin)* $(PATHbin) ./log/* ./ipc/* ./log ./ipc 
