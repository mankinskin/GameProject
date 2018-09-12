BUILDDIR=build
INCLUDE=-I../src/

tests: $(BUILDDIR)/functor $(BUILDDIR)/signal $(BUILDDIR)/tuple_utils $(BUILDDIR)/id

all: id functor signal tuple_utils

functor: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/functor functor.cpp ../src/functor.cpp $(INCLUDE)

signal: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/signal signal.cpp ../src/signal.cpp $(INCLUDE)

tuple_utils: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/tuple_utils tuple_utils.cpp $(INCLUDE)

id: $(BUILDDIR)
	clang++ -o $(BUILDDIR)/id id.cpp $(INCLUDE)

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run: tests
	$(BUILDDIR)/functor
	$(BUILDDIR)/signal
	$(BUILDDIR)/tuple_utils
	$(BUILDDIR)/id

PHONY: tests
