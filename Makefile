CC = gcc-6
CFLAGS = -fopenmp -lcrypto
TARGET = crackpass

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)