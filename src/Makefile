CC=c++
CFLAGS=-I.
TARGET=router.exe
SOURCES=run-router.cpp my-router.cpp aodv_messages.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBS=-lpthread -lstdc++

all: $(TARGET) $(SOURCES)

$(TARGET): $(SOURCES) 
	$(CC) $(CFLAGS) -g -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	$(RM) $(TARGET) $(OBJECTS) 
