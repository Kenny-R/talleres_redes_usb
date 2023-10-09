COMP = gcc -c
LINK = gcc
OBJDIR = etc
OBJECTS = $(OBJDIR)/main.o

PROGRAM = taller1.out

.PHONY: all clean

all: $(PROGRAM)

clean: 
	rm $(PROGRAM)
	rm $(OBJECTS)
	rm -r ./$(OBJDIR)

$(PROGRAM): $(OBJECTS)
	$(LINK) $(OBJECTS) -o $(PROGRAM) -lpthread

$(OBJDIR)/main.o: main.c
	$(COMP) main.c -o $(OBJDIR)/main.o
	
$(OBJDIR):
	mkdir $(OBJDIR)