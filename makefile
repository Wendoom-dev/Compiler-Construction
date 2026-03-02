
CC = gcc
CFLAGS = -Wall -g


TARGET = stage1exe


SRCS = driver.c lexer.c parser.c


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) cleanOutput.txt