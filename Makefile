CC = gcc
CFLAGS = -Wall -Wextra -g -O3 -pg
TARGET = GA
SRC = GA.c functions.c OF.c
OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
