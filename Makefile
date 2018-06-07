include deppaths
INCLUDE_GL= -I$(INCLUDE_GL_DIR) -I$(INCLUDE_GLFW_DIR) -I$(INCLUDE_GLM_DIR)
GCC=g++
C_INCLUDE_PATH=
CPLUS_INCLUDE_PATH=/usr/include/c++
INCLUDE_FREETYPE=-I$(INCLUDE_FREETYPE_DIR)
INCLUDE_ASSIMP=-I$(INCLUDE_ASSIMP_DIR)
INCLUDE_SOIL=-I$(INCLUDE_SOIL_DIR)
INCLUDE_LIBPNG=-I$(INCLUDE_LIBPNG_DIR)
SRC_FILES=$(shell ls src | grep -e '\.cpp')
OBJ_FILES=$(SRC_FILES:cpp=o)
OBJ_TARGETS=$(patsubst %, build/%, $(OBJ_FILES))
H_FILES=$(shell ls src | grep -e '\.h')
INCLUDES=$(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_ASSIMP) $(INCLUDE_SOIL)
INCLUDE_LIB_PATHS=-L$(LIBGL_DIR) -L$(LIBGLFW_DIR) -L$(LIBFREETYPE_DIR) -L$(LIBASSIMP_DIR) -L$(LIBSOIL_DIR) -L$(LIBGLUT_DIR) -L$(LIBPNG_DIR)
INCLUDE_LIBS=-lncurses -lassimp -lglfw -lGLEW -lGL -lGLU -lSOIL -lfreetype -lX11 -lm -lrt -lpng 
DEP_PACKAGES=libglfw3 libglfw3-dev 


build/%.o: src/%.cpp 
	$(GCC) -c -o $@ $< $(INCLUDES)

build:
	mkdir build

all: build tags $(OBJ_TARGETS) 
	$(GCC) -o main $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) $(INCLUDE_SOIL) $(OBJ_TARGETS) $(INCLUDE_LIB_PATHS) $(INCLUDE_LIBS)

run: all
	./main

tags: 
	ctags -R .

clean: 
	rm -rf build

fresh: clean all

freshrun: fresh run

.PHONY: clean fresh tags all
