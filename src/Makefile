CC=cc
CFLAGS=-I.
TARGET=router.exe
SOURCES=run-router.cpp my-router.cpp aodv_messages.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBS=-lpthread -lstdc++

all: $(TARGET) $(SOURCES)

$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) -o -g $(TARGET) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(TARGET) $(OBJECTS) 
