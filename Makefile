IDIR = ./headers
SRCDIR = ./src
CC = g++
CFLAGS = -I $(IDIR) -pthread

ODIR = obj
_OBJ = team.o big_integer.o solver.o main.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))


all: warnerscore

$(ODIR)/%.o: $(SRCDIR)/%.cpp | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR):
	mkdir $@

warnerscore: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

run: warnerscore
	./warnerscore

clean:
	rm -rf $(ODIR) 
	rm warnerscore