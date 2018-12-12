CXX=clang++
CXXFLAGS=-std=c++2a -stdlib=libc++ -Iinclude -g
BIN=test
OBJS=src/Option.o src/test.o

all: $(BIN)

$(BIN): $(OBJS)
	  $(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	$(RM) $(OBJS) $(BIN)
