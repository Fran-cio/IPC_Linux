CC=gcc
CFLAGS_gdb=-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11 -DTEST --debug
CFLAGS=-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11
PATHh=./include/
PATHrec=./src/
PATHrecser=./src/servidor/
PATHreccli=./src/client/
PATHout=./out/
PATHlib=./lib/
PATHbin=./bin/

all: cliente server

cliente: $(PATHbin)cliente

$(PATHbin)cliente: $(PATHout)cliente.o $(PATHlib)lib_cli.a 
	mkdir -p $(PATHbin)
	$(CC) $(CFLAGS) -o $(PATHbin)cliente $(PATHout)cliente.o -L$(PATHlib) -l_cli

cliente_gdb: $(PATHbin)cliente_gdb

$(PATHbin)cliente_gdb: $(PATHreccli)cliente.c $(PATHreccli)manejo_del_cliente.c $(PATHreccli)protocolos_cli.c
	mkdir -p $(PATHbin)
	$(CC) $(CFLAGS_gdb) -o $(PATHbin)cliente_gdb $(PATHreccli)cliente.c 
	
server_gdb: $(PATHbin)server_gdb

$(PATHbin)server_gdb: $(PATHrecser)server.c $(PATHrecser)manejo_del_server.c $(PATHrecser)protocolos.c
	mkdir -p $(PATHbin)
	$(CC) $(CFLAGS_gdb) -o $(PATHbin)server_gdb $(PATHrecser)server.c $(PATHrecser)protocolos.c $(PATHrec)signals.c 

server: $(PATHbin)server
	
$(PATHbin)server: $(PATHout)server.o $(PATHlib)lib_ser.a 
	mkdir -p $(PATHbin) ./log ./ipc 
	$(CC) $(CFLAGS) -o $(PATHbin)server $(PATHout)server.o -L$(PATHlib) -l_ser
	
$(PATHlib)lib_ser.a: $(PATHout)protocolos.o $(PATHout)signals.o 
	mkdir -p $(PATHlib) 
	ar cr $(PATHlib)lib_ser.a $(PATHout)protocolos.o $(PATHout)signals.o 

$(PATHlib)lib_cli.a: $(PATHout)protocolos_cli.o $(PATHout)signals.o 
	mkdir -p $(PATHlib) 
	ar cr $(PATHlib)lib_cli.a $(PATHout)protocolos_cli.o $(PATHout)signals.o 

$(PATHout)server.o: $(PATHrecser)server.c  
	mkdir -p $(PATHout)
	$(CC) $(CFLAGS) -c $(PATHrecser)server.c
	mv ./server.o $(PATHout)/server.o

$(PATHout)protocolos.o: $(PATHrecser)protocolos.c $(PATHrecser)manejo_del_server.c   
	mkdir -p $(PATHout)
	$(CC) $(CFLAGS) -c $(PATHrecser)protocolos.c 
	mv ./protocolos.o $(PATHout)

$(PATHout)cliente.o: $(PATHreccli)cliente.c
	mkdir -p $(PATHout)
	$(CC) $(CFLAGS) -c $(PATHreccli)cliente.c 
	mv ./cliente.o $(PATHout)

$(PATHout)protocolos_cli.o: $(PATHreccli)protocolos_cli.c $(PATHreccli)manejo_del_cliente.c   
	mkdir -p $(PATHout)
	$(CC) $(CFLAGS) -c $(PATHreccli)protocolos_cli.c 
	mv ./protocolos_cli.o $(PATHout)


$(PATHout)signals.o: $(PATHrec)signals.c  
	mkdir -p $(PATHout)
	$(CC) $(CFLAGS) -c $(PATHrec)signals.c 
	mv ./signals.o $(PATHout)

clean:
	rm -f -d $(PATHlib)* $(PATHlib) $(PATHout)* $(PATHout) $(PATHbin)* $(PATHbin) ./log/* ./ipc/* ./log ./ipc 
