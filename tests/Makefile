BUILDDIR=build
INCLUDE=-I../src/

tests: $(BUILDDIR)/functor $(BUILDDIR)/signal $(BUILDDIR)/tuple_utils

all: functor signal tuple_utils

functor: $(BUILDDIR)
	g++ -o $(BUILDDIR)/functor functor.cpp ../src/functor.cpp $(INCLUDE)

signal: $(BUILDDIR)
	g++ -o $(BUILDDIR)/signal signal.cpp ../src/signal.cpp $(INCLUDE)

tuple_utils: $(BUILDDIR)
	g++ -o $(BUILDDIR)/tuple_utils tuple_utils.cpp $(INCLUDE)

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run: tests
	$(BUILDDIR)/functor
	$(BUILDDIR)/signal
	$(BUILDDIR)/tuple_utils

PHONY: tests
