BUILDDIR=build
INCLUDE=-I../src/

tests: $(BUILDDIR)/functor $(BUILDDIR)/signal

functor: $(BUILDDIR)
	g++ -o $(BUILDDIR)/functor functor.cpp ../src/functor.cpp $(INCLUDE)

signal: $(BUILDDIR)
	g++ -o $(BUILDDIR)/signal signal.cpp ../src/signal.cpp $(INCLUDE)

$(BUILDDIR):
	mkdir $(BUILDDIR)
clean:
	rm -rf $(BUILDDIR)

run: tests
	$(BUILDDIR)/functor
	$(BUILDDIR)/signal

PHONY: tests
