CXX=g++
CXXFLAGS = -g -Wall -Wextra -pedantic-errors -std=c++0x
TARGET=router.exe
SOURCES=run-router.cpp my-router.cpp aodv_messages.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBS=-lpthread -lstdc++ 

all: $(TARGET) $(SOURCES)

$(TARGET): $(OBJECTS) 
	$(CC) -g $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(TARGET) $(OBJECTS) 
