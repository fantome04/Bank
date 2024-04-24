CXX = g++
CFLAGS = -Werror -Wall
DBGFLAGS = -g -fsanitize=address,undefined
CLIB = -lrt -lpthread

compile: init client server destroy

init: bank.h init.cpp bank.o bank_cell.o
	$(CXX) init.cpp bank.o bank_cell.o -o init $(CLIB) $(CFLAGS)

client: client.cpp
	$(CXX) client.cpp -o client $(CLIB) $(CFLAGS)

server: bank.h server.cpp bank.o bank_cell.o
	$(CXX) server.cpp bank.o bank_cell.o -o server $(CLIB) $(CFLAGS)

destroy: bank.h destroy.cpp bank.o bank_cell.o
	$(CXX) destroy.cpp bank.o bank_cell.o -o destroy $(CLIB) $(CFLAGS)

testing: bank.h client.cpp debug_bank.o debug_bank_cell.o
	$(CXX) testing.cpp bank.o bank_cell.o $(DBGFLAGS) -o testing $(CLIB)

debug_valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy

bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c

debug_bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(DBGFLAGS) -o debug_bank_cell.o

bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c

debug_bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(DBGFLAGS) -o debug_bank.o

.PHONY: clean

clean:
	rm -f init client server testing destroy *.o
