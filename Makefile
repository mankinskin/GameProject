include deppaths # include paths to dependencies (Libraries, Include Directories)

INCLUDE_GL= -I$(INCLUDE_GL_DIR) -I$(INCLUDE_GLFW_DIR) -I$(INCLUDE_GLM_DIR)
GCC=g++ -std=c++17
C_INCLUDE_PATH=
CPLUS_INCLUDE_PATH=/usr/include/c++
INCLUDE_FREETYPE=-I$(INCLUDE_FREETYPE_DIR)
INCLUDE_ASSIMP=-I$(INCLUDE_ASSIMP_DIR)
INCLUDE_SOIL=-I$(INCLUDE_SOIL_DIR)
SRC_FILES=$(shell ls src | grep -e '\.cpp')
OBJ_FILES=$(SRC_FILES:cpp=o)
OBJ_TARGETS=$(patsubst %, build/%, $(OBJ_FILES))
H_FILES=$(shell ls src | grep -e '\.h')

INCLUDE_LIBS=-lassimp -lglfw -lGLEW -lGL -lGLU -lm -lSOIL -lfreetype -lncurses
depinstall:
	apt-get install libglm-dev libglfw3-dev libglew-dev libassimp-dev libsoil-dev freeglut3  -y
dep: depinstall
	cp $(LIBGLFW_DIR)/libglfw.so .
	cp $(LIBASSIMP_DIR)/libassimp.so .
	cp $(LIBGLEW_DIR)/libGLEW.so .

build/%.o: src/%.cpp 
	$(GCC) -o $@ -c $< $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_ASSIMP) $(INCLUDE_SOIL)

build:
	mkdir build

all: build dep $(OBJ_TARGETS) 
	$(GCC) -o game $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) $(INCLUDE_SOIL) $(OBJ_TARGETS) -L$(LIBGL_DIR) -L$(LIBGLFW_DIR) -L$(LIBFREETYPE_DIR) -L$(LIBASSIMP_DIR) -L$(LIBSOIL_DIR) $(INCLUDE_LIBS)

run: all
	./game

cleanbuild: 
	rm -r build
cleandep:
	rm .depend
clean: cleanbuild cleandep
