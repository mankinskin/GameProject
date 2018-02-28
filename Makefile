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





build:
	mkdir build

all: build $(OBJ_TARGETS)
	$(GCC) -o game $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) $(INCLUDE_SOIL) $(OBJ_TARGETS) -L$(LIBGL_DIR) -L$(LIBGLFW_DIR) -L$(LIBFREETYPE_DIR) -L$(LIBASSIMP_DIR) -L$(LIBSOIL_DIR) -lassimp -lglfw -lGLEW -lGL -lSOIL -lfreetype -lncurses

build/gui.o:
	$(GCC) -c src/gui.cpp -o $@ $(INCLUDE_GL) 
build/input.o:
	$(GCC) -c src/input.cpp -o $@ $(INCLUDE_GL) 
build/widget.o:
	$(GCC) -c src/widget.cpp -o $@ $(INCLUDE_GL) 
build/colorings.o:
	$(GCC) -c src/colorings.cpp -o $@ $(INCLUDE_GL) 
build/texture.o: 
	$(GCC) -c src/texture.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_SOIL)
build/shader_loader.o:
	$(GCC) -c src/shader_loader.cpp -o $@ $(INCLUDE_GL)
build/neural.o:
	$(GCC) -c src/neural.cpp -o $@ 
build/material.o:
	$(GCC) -c src/material.cpp -o $@ $(INCLUDE_GL)
build/line.o:
	$(GCC) -c src/line.cpp -o $@ $(INCLUDE_GL)
build/lights.o:
	$(GCC) -c src/lights.cpp -o $@ $(INCLUDE_GL)
build/voxelization.o:
	$(GCC) -c src/voxelization.cpp -o $@ $(INCLUDE_GL) 
build/vao.o:
	$(GCC) -c src/vao.cpp -o $@ $(INCLUDE_GL) 
build/shader.o: 
	$(GCC) -c src/shader.cpp -o $@ $(INCLUDE_GL) 
build/physics.o:
	$(GCC) -c src/physics.cpp -o $@ $(INCLUDE_GL) 
build/debug.o:
	$(GCC) -c src/debug.cpp -o $@ $(INCLUDE_GL) 
build/entities.o:
	$(GCC) -c src/entities.cpp -o $@ $(INCLUDE_GL) 
build/gl.o:
	$(GCC) -c src/gl.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
build/gldebug.o:
	$(GCC) -c src/gldebug.cpp -o $@ $(INCLUDE_GL) 
build/mesh.o: 
	$(GCC) -c src/mesh.cpp -o $@ $(INCLUDE_GL) 
build/framebuffer.o:
	$(GCC) -c src/framebuffer.cpp -o $@ $(INCLUDE_GL) 
build/model_loader.o:
	$(GCC) -c src/model_loader.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_ASSIMP)
build/model.o:
	$(GCC) -c src/model.cpp -o $@ $(INCLUDE_GL) 
build/collision.o:
	$(GCC) -c src/collision.cpp -o $@ $(INCLUDE_GL) 
build/signal.o:
	$(GCC) -c src/signal.cpp -o $@ $(INCLUDE_GL) 
build/event.o:
	$(GCC) -c src/event.cpp -o $@ $(INCLUDE_GL) 
build/text.o:
	$(GCC) -c src/text.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
build/font_loader.o:
	$(GCC) -c src/font_loader.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
build/font.o:
	$(GCC) -c src/font.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
build/camera.o:
	$(GCC) -c src/camera.cpp -o $@ $(INCLUDE_GL) 
build/quad.o:
	$(GCC) -c src/quad.cpp -o $@ $(INCLUDE_GL) 
build/keys.o:
	$(GCC) -c src/keys.cpp -o $@ $(INCLUDE_GL) 
build/mouse.o:
	$(GCC) -c src/mouse.cpp -o $@ $(INCLUDE_GL) 
build/gates.o:
	$(GCC) -c src/gates.cpp -o $@ $(INCLUDE_GL) 
build/functor.o:
	$(GCC) -c src/functor.cpp -o $@ $(INCLUDE_GL) 
build/contextwindow.o: 
	$(GCC) -c src/contextwindow.cpp -o $@ $(INCLUDE_GL)
build/app.o:
	$(GCC) -c src/app.cpp -o $@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
build/main.o:	
	$(GCC) -c src/main.cpp -o $@ $(INCLUDE_GL)

cleanbuild: 
	rm -r build
cleanobj:
	rm src/*.o
clean: cleanbuild
