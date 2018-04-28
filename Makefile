include deppaths
INCLUDE_GL= -I$(INCLUDE_GL_DIR) -I$(INCLUDE_GLFW_DIR) -I$(INCLUDE_GLM_DIR)
GCC=g++
C_INCLUDE_PATH=
CPLUS_INCLUDE_PATH=/usr/include/c++
INCLUDE_FREETYPE=-I$(INCLUDE_FREETYPE_DIR)
INCLUDE_ASSIMP=-I$(INCLUDE_ASSIMP_DIR)
INCLUDE_SOIL=-I$(INCLUDE_SOIL_DIR)
SRC_FILES=$(shell ls src | grep -e '\.cpp')
OBJ_FILES=$(SRC_FILES:cpp=o)
OBJ_TARGETS=$(patsubst %, build/%, $(OBJ_FILES))
H_FILES=$(shell ls src | grep -e '\.h')
INCLUDE_LIB_PATHS=-L$(LIBGL_DIR) -L$(LIBGLFW_DIR) -L$(LIBFREETYPE_DIR) -L$(LIBASSIMP_DIR) -L$(LIBSOIL_DIR) -L$(LIBGLUT_DIR)
INCLUDE_LIBS=-lncurses -lassimp -lglfw -lGLEW -lGL -lGLU -lSOIL -lfreetype -lX11 -lm -lrt  
DEP_PACKAGES=libglfw3 libglfw3-dev 

.deps_installed:
	sudo apt-get install llvm-6.0-dev 
	sudo apt-get install libgl1-mesa-dev libgl1-mesa-dri libglw1-mesa libgl1-mesa-glx libglapi-mesa libglu1-mesa-dev mesa-common-dev libglu1-mesa mesa-utils-extra
	sudo apt-get install libglfw3-dev libassimp-dev libglew-dev freeglut3-dev libglm-dev libncurses-dev libfreetype6-dev libsoil-dev 
	sudo cp $(LIBGLFW_DIR)/libglfw.so .
	sudo cp $(LIBGLFW_DIR)/libglfw.so.3 .
	sudo cp $(LIBASSIMP_DIR)/libassimp.so .
	sudo cp $(LIBGLEW_DIR)/libGLEW.so .
	sudo cp $(LIBGLEW_DIR)/libGLEW.so.2.0 .
	sudo cp $(LIBGLEW_DIR)/libGLEW.so.2.0.0 .
	sudo cp $(LIBGLUT_DIR)/libglut.so.3 .
	sudo cp $(LIBGLUT_DIR)/libglut.so.3.9.0 .
	touch $@

build/%.o: src/%.cpp 
	$(GCC) -o $@ -c $< $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_ASSIMP) $(INCLUDE_SOIL) $(INCLUDE_LIB_PATHS) $(INCLUDE_LIBS)

build:
	mkdir build

all: build tags $(OBJ_TARGETS) 
	$(GCC) -o game $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) $(INCLUDE_SOIL) $(OBJ_TARGETS) $(INCLUDE_LIB_PATHS)  $(INCLUDE_LIBS)

run: all
	./game

tags: 
	ctags -R .

cleanbuild: 
	rm -rf build

cleandep:
	rm -f .deps_installed
	rm -f libglfw.so libglfw.so.3 libassimp.so libGLEW.so libGLEW.so.2.0 libGLEW.so.2.0 libGLEW.so.2.0.0 libglut.so.3 libglut.so.3.9.0
	rm -f game

clean: cleanbuild cleandep

fresh: clean all

.PHONY: clean fresh tags all
