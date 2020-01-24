EXECUTABLES = Editeur
SOURCES = buffer.c position.c requette.c affichage.c chargement.c parser.c undoredo.c
CHECK = check_buffer.c
OBJ = $(SOURCES:%.c=%.o)
TESTES = $(CHECK:%.c=%)
FLAGS = -Wall -g
LIBS = `pkg-config --libs check`
INCLUDES = -lncurses


all: bin $(EXECUTABLES) $(TESTES)

$(EXECUTABLES):$(OBJ)
	gcc $(FLAGS) $^ $@.c -o ./bin/$@ $(INCLUDES)

bin: mymkdir.sh
	./mymkdir.sh

$(TESTES): $(OBJ)
	gcc $(FLAGS) $^ $@.c -o ./test/$@ $(LIBS) $(INCLUDES)

%.o: %.c
	gcc -c $<

graph:
	dot -Tpng diagramme.dot > diagramme.png

clean:
	rm -f $(OBJ) bin/* test/*
	rmdir bin
	rmdir test
