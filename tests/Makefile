

BUILDDIR=build
INCLUDE=-I../src/

tests: $(BUILDDIR)
	g++ -o $(BUILDDIR)/test_functor test_functor.cpp ../src/functor.cpp $(INCLUDE)

$(BUILDDIR):
	mkdir $(BUILDDIR)
clean:
	rm -rf $(BUILDDIR)

run: tests
	$(BUILDDIR)/test_functor

PHONY: tests
