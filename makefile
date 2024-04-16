run: init client destroy
	./init
	./client
	./destroy

run_init: init
	./init

run_client: client
	./client

run_destroy: destroy
	./destroy

release: init client destroy


init: bank.h init.cpp bank.o bank_cell.o
	g++ init.cpp bank.o bank_cell.o -o init -lrt

client: bank.h client.cpp bank.o bank_cell.o
	g++ client.cpp bank.o bank_cell.o -o client -lrt

destroy: bank.h destroy.cpp bank.o bank_cell.o
	g++ destroy.cpp bank.o bank_cell.o -o destroy -lrt

init_d: init.cpp bank.o bank_cell.o
	g++ init.cpp bank.o bank_cell.o -g -o init_d -lrt

client_d: client.cpp bank.o bank_cell.o
	g++ client.cpp bank.o bank_cell.o -g -o client_d -lrt

destroy_d: destroy.cpp bank.o bank_cell.o
	g++ destroy.cpp bank.o bank_cell.o -g -o destroy_d -lrt

debug_init: init_d
	gdb init_d

debug_destroy: destroy_d
	gdb destroy_d

debug_testing: testing
	gdb testing

testing: bank.h client.cpp bank.o bank_cell.o
	g++ testing.cpp bank.o bank_cell.o -g -o testing -lrt

debug_valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy

bank_cell.o: bank_cell.cpp bank.h
	g++ bank_cell.cpp -c

bank.o: bank_cell.h bank.h bank.cpp
	g++ bank.cpp -c

clean:
	rm -f init client destroy *.o init_d client_d destroy_d
