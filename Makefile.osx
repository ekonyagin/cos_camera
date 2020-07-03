CXX=g++
CC=g++
CFLAGS+=`pkg-config --cflags opencv4`
LDFLAGS+=`pkg-config --libs opencv4`
SOURCES=xiSample.cpp
OBJECTS=$(SOURCES:.cpp=.o)
PROGRAM=xiSample

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -g $(OBJECTS) -O3 -o $@ -F /Library/Frameworks -framework m3api $(LDFLAGS)
	dsymutil $@

.cpp.o: $(patsubst %.cpp,%.o,$(wildcard *.cpp))
	$(CXX) -g -F /Library/Frameworks -c $< -O3 $(CFLAGS) -std=c++11 -o $@

clean:
	rm -Rf $(PROGRAM) $(PROGRAM).dSYM $(OBJECTS)
