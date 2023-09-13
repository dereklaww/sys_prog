EXC = pe_exchange 
TRADER = pe_trader
CC=gcc
CFLAGS=-Wall -Werror -Wvla -O0 -std=c11 -g -fsanitize=address,leak
LDFLAGS=-lm
EXC_SRC        = pe_exchange.c pe_market.c pe_mytrader.c pe_order.c
TRADER_SRC = pe_trader.c
OBJ        = $(SRC:.c=.o)

all:$(EXC) $(TRADER)

$(EXC):
	$(CC) $(CFLAGS) $(LDFLAGS) $(EXC_SRC) -o $@ $(EXC.o)

$(TRADER):
	$(CC) $(CFLAGS) $(LDFLAGS) $(TRADER_SRC) -o $@ $(TRADER.o)



.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(ASAN_FLAGS) $< 

run:
	./$(TARGET)


.PHONY: clean
clean:
	rm -f *.o *.obj $(EXC)
	rm -f *.o *.obj $(TRADER)
	
add: 
	git add pe_exchange.c pe_exchange.h pe_market.c pe_market.h pe_mytrader.c pe_mytrader.h 
	git add pe_order.c pe_common.h pe_trader.c pe_trader.h Makefile README.md
	git commit -m "pls just work i beg of you"
	git push



