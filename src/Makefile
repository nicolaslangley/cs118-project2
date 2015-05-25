CC=cc
CFLAGS=-I.
TARGET=router-exec
LIBS=-lpthread -lstdc++

all: $(TARGET)

$(TARGET): run-router.o my-router.o
	$(CC) $(CFLAGS) -o $(TARGET) my-router.o run-router.o $(LIBS)

clean:
	$(RM) $(TARGET) my-router.o run-router.o
