BUILDDIR=build
INCLUDE=-I../src/

tests: $(BUILDDIR)/id $(BUILDDIR)/tuple_utils $(BUILDDIR)/functor $(BUILDDIR)/signal

all: id functor signal tuple_utils

id: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/id id.cpp $(INCLUDE)

functor: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/functor functor.cpp ../src/functor.cpp $(INCLUDE)

signal: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/signal signal.cpp ../src/signal.cpp $(INCLUDE)

tuple_utils: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/tuple_utils tuple_utils.cpp $(INCLUDE)

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run: tests
	$(BUILDDIR)/id
	$(BUILDDIR)/tuple_utils
	$(BUILDDIR)/functor
	$(BUILDDIR)/signal

PHONY: tests
