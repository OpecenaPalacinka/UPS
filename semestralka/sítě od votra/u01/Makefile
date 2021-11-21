CC = g++
CFLAGS = -g -Wall -pedantic
LDFLAGS = -lpthread
OBJS = server.o
BIN = TCPserver

all: compile run clean

run: compile
	./$(BIN) 4242

compile: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) -r $(OBJS)