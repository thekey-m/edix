CC = clang
CFLAGS = -g -Wall -Wextra
LDFLAGS = 

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJ = $(patsubst src/%.c, bin/%.o, $(SRC))

BIN = bin

.PHONY: all clean

all: clean edix

run: edix
	$(BIN)/edix.exe $(ARGS)

edix: $(OBJ)
	$(CC) -o $(BIN)/edix.exe $^ $(LDFLAGS)

bin/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	del /Q /S $(BIN)\*