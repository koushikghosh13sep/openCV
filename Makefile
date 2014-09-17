CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

#% : %.cpp
#	g++ $(CFLAGS) $(LIBS) -o $@ $<

SRCS = $(wildcard *.cpp)
PROGS = $(patsubst %.cpp,%,$(SRCS))
all: $(PROGS)
%: %.cpp
	g++ $(CFLAGS) $(LIBS) -o $@ $<

