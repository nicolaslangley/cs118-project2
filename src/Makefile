CC=cc
CFLAGS=-I.
TARGET=router-exec
SOURCES=run-router.cpp my-router.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBS=-lpthread -lstdc++

all: $(TARGET) $(SOURCES)

$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(TARGET) $(OBJECTS) 
