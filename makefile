CXX = g++
CFLAGS = -Werror -Wall -g -fsanitize=address,undefined
CLIB = -lrt -lpthread

.PHONY: run_client run

run: init client destroy
	./init
	./client
	./destroy

run_client: client
	./client

init: bank.h init.cpp bank.o bank_cell.o
	$(CXX) init.cpp bank.o bank_cell.o -o init $(CLIB)

client: bank.h client.cpp bank.o bank_cell.o
	$(CXX) client.cpp bank.o bank_cell.o -o client $(CLIB)

destroy: bank.h destroy.cpp bank.o bank_cell.o
	$(CXX) destroy.cpp bank.o bank_cell.o -o destroy $(CLIB)

testing: bank.h client.cpp bank.o bank_cell.o
	$(CXX) testing.cpp bank.o bank_cell.o $(-CFLAGS) -o testing $(CLIB)

debug_valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy

bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c

bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c

clean:
	rm -f init client destroy *.o init_d client_d destroy_d
