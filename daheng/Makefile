# Makefile for sample program
.PHONY			: all clean

# the program to build
NAME			:= daheng_sample

# Build tools and flags
CXX			:= g++
LD			:= g++
SRCS                    := $(wildcard *.cpp)
OBJS                    := $(patsubst %cpp, %o, $(SRCS))
CPPFLAGS		:= -w -I./ -L/usr/lib -L./ -Wl,-rpath=/usr/lib:./ 
CPPFLAGS+=`pkg-config --cflags opencv4`

# -DCHEAT_JSON

LDFLAGS			:=-lgxiapi 
LDFLAGS+= -fopenmp `pkg-config --libs opencv4`
all			: $(NAME)

$(NAME)			: $(OBJS)
	$(LD) -o $@ $^ $(CPPFLAGS) $(LDFLAGS)

%.o		: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

clean			:
	$(RM) *.o $(NAME)

