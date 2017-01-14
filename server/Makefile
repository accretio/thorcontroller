IDIR =../include
CC=cc
CFLAGS=-I$(IDIR) -DMPU9250 -DRASPBERRY_PI

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
	gcc -o $@ $^ $(CFLAGS) -lmicrohttpd # -lm -lpigpio -lrt -pthread -lcurses

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ wingleveler


send:
	rsync -a . pi@10.0.0.138:/tmp/wingleveler


smooth: smoother.o
	gcc -o $@ $^ $(CFLAGS) -lm
