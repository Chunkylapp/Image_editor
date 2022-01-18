CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

build: image_editor

image_editor: effects.o image_editor.o img.o utils.o worker.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -lm

clean:
	rm *.o image_editor
pack:
	zip -FSr 312CA_GheorgheAlexandru_Tema3.zip README Makefile *.c *.h

.PHONY: clean pack