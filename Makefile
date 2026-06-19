CC      = gcc
CFLAGS  = -Wall -Wextra -std=c17
SRC     = main.c memoria.c
OBJ     = $(SRC:.c=.o)
TARGET  = memsim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c tipos.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
