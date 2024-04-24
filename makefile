CXX = g++
CFLAGS = -Werror -Wall 
DBGFLAFS = -g -fsanitize=address,undefined
CLIB = -lrt -lpthread

.PHONY: run_client run

run: init client destroy
	./init
	./client
	./destroy

run_client: client
	./client

init: bank.h init.cpp bank.o bank_cell.o
	$(CXX) init.cpp bank.o bank_cell.o -o init $(CLIB) $(CFLAGS)

client: bank.h client.cpp bank.o bank_cell.o
	$(CXX) client.cpp bank.o bank_cell.o -o client $(CLIB) $(CFLAGS)

destroy: bank.h destroy.cpp bank.o bank_cell.o
	$(CXX) destroy.cpp bank.o bank_cell.o -o destroy $(CLIB) $(CFLAGS)

testing: bank.h client.cpp debug_bank.o debug_bank_cell.o
	$(CXX) testing.cpp bank.o bank_cell.o $(DBGFLAGS) $(CFLAGS) -o testing $(CLIB)

debug_valgrind: init testing destroy
	./init
	valgrind ./testing
	./destroy

bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(CFLAGS)

debug_bank_cell.o: bank_cell.cpp bank.h
	$(CXX) bank_cell.cpp -c $(DBGFLAGS) $(CFLAGS) -o debug_bank_cell.o

bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(CFLAGS)

debug_bank.o: bank_cell.h bank.h bank.cpp
	$(CXX) bank.cpp -c $(DBGFLAGS) $(CFLAGS) -o debug_bank.o

clean:
	rm -f init client testing destroy *.o 
