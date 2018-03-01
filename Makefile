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
INCLUDE_LIB_PATHS=-L$(LIBGL_DIR) -L$(LIBGLFW_DIR) -L$(LIBFREETYPE_DIR) -L$(LIBASSIMP_DIR) -L$(LIBSOIL_DIR) -L$(LIBGLUT_DIR)
INCLUDE_LIBS=-lassimp -lglfw -lGLEW -lGL -lGLU -lSOIL -lfreetype -lncurses -lglut -lX11 -lm -lrt  


.deps_installed:
	touch .deps_installed
depinstall: .deps_installed
	sudo apt-get install libglu1-mesa libglu1-mesa-dev libgl1-mesa-dev libgl1-mesa-glx libglapi-mesa libosmesa6-dev mesa-common-dev mesa-va-drivers mesa-vulkan-drivers mesa-vdpau-drivers mesa-utils libglm-dev libglfw3-dev libglew-dev libassimp-dev libsoil-dev freeglut3-dev libfreetype6-dev -y

dep: depinstall
	sudo cp $(LIBGLFW_DIR)/libglfw.so .
	sudo cp $(LIBASSIMP_DIR)/libassimp.so .
	sudo cp $(LIBGLEW_DIR)/libGLEW.so .
	sudo cp $(LIBGLEW_DIR)/libGLEW.so.2.0 .
	sudo cp $(LIBGLEW_DIR)/libGLEW.so.2.0.0 .
	sudo cp $(LIBGLUT_DIR)/libglut.so.3 .
	sudo cp $(LIBGLUT_DIR)/libglut.so.3.9.0 .

build/%.o: src/%.cpp 
	$(GCC) -o $@ -c $< $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_ASSIMP) $(INCLUDE_SOIL)

build:
	mkdir build

all: build dep $(OBJ_TARGETS) 
	$(GCC) -o game $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) $(INCLUDE_SOIL) $(OBJ_TARGETS) $(INCLUDE_LIB_PATHS)  $(INCLUDE_LIBS)

run: all
	./game

cleanbuild: 
	sudo rm -rf build
cleandep:
	rm .deps_installed
clean: cleanbuild cleandep
