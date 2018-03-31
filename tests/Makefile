include ../Config

BINDIR = ../bin
TARGETS = $(BINDIR)/testzfp $(BINDIR)/testviews
CXXLIBS = -L../lib -lzfp

all: $(TARGETS)

$(BINDIR)/testzfp: testzfp.cpp fields.o ../lib/libzfp.a
	$(CXX) $(CXXFLAGS) -I../array testzfp.cpp fields.o $(CXXLIBS) -o $@

$(BINDIR)/testviews: testviews.cpp ../lib/libzfp.a
	$(CXX) $(CXXFLAGS) -I../array testviews.cpp $(CXXLIBS) -o $@

fields.o: fields.c fields.h
	$(CC) $(CFLAGS) -c fields.c

test: $(BINDIR)/testzfp
	$(BINDIR)/testzfp

clean:
	rm -f $(TARGETS) fields.o
