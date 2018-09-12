GCC=clang++ -stdlib=libstdc++
include deppaths
C_INCLUDE_PATH=
CPLUS_INCLUDE_PATH=/usr/include/c++
INCLUDE_GL= -I$(INCLUDE_GL_DIR) -I$(INCLUDE_GLFW_DIR) -I$(INCLUDE_GLM_DIR)
INCLUDE_FREETYPE=-I$(INCLUDE_FREETYPE_DIR)
INCLUDE_ASSIMP=-I$(INCLUDE_ASSIMP_DIR)
INCLUDE_SOIL=-I$(INCLUDE_SOIL_DIR)
INCLUDE_LIBPNG=-I$(INCLUDE_LIBPNG_DIR)
INCLUDE_LIB_PATHS=-L$(LIBGL_DIR) -L$(LIBGLFW_DIR) -L$(LIBFREETYPE_DIR) -L$(LIBASSIMP_DIR) -L$(LIBSOIL_DIR) -L$(LIBGLUT_DIR) -L$(LIBPNG_DIR)
INCLUDE_LIBS=-lncurses -lassimp -lglfw -lGLEW -lGL -lGLU -lSOIL -lfreetype -lX11 -lm -lrt -lpng -lstdc++
DEP_PACKAGES=libglfw3 libglfw3-dev 
INCLUDES=$(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_ASSIMP) $(INCLUDE_SOIL)

SRC_FILES=$(shell ls src | grep -e '\.cpp')
OBJ_FILES=$(SRC_FILES:cpp=o)
OBJ_TARGETS=$(patsubst %, build/%, $(OBJ_FILES))
H_FILES=$(shell ls src | grep -e '\.h')

all: tests build tags $(OBJ_TARGETS)
	$(GCC) -o main $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) $(INCLUDE_SOIL) $(OBJ_TARGETS) $(INCLUDE_LIB_PATHS) $(INCLUDE_LIBS)

build/%.o: src/%.cpp 
	$(GCC) -c -o $@ $< $(INCLUDES)

build:
	mkdir build

run: all
	./main

tags: 
	ctags -R .

clean: 
	rm -rf build

fresh: clean all

freshrun: fresh run

tests: build_tests
	make run -C tests
build_tests:
	make all -C tests

.PHONY: clean fresh tags all tests build_tests
