IDIR = ./headers
SRCDIR = ./src
CC = g++
CFLAGS = -I $(IDIR)

ODIR = obj
_OBJ = team.o main.o
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