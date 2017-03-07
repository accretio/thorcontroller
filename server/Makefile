IDIR =../include
CC=cc
CFLAGS=-I$(IDIR) -DRASPBERRY_PI

ODIR=obj
LDIR =../lib

_DEPS =
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

thor: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) -lmicrohttpd -lpigpio # -lrt -pthread -lcurses

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ thor


send:
	rsync -a . pi@pi1:/home/pi/thorcontroller


send2:
	rsync -a . pi@workshop.accret.io:/home/pi/thorcontroller/server
