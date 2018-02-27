include deppaths # include paths to dependencies (Libraries, Include Directories)

INCLUDE_GL= -I$(INCLUDE_GL_DIR) -I$(INCLUDE_GLFW_DIR) -I$(INCLUDE_GLM_DIR)
GCC=g++ -std=c++17
C_INCLUDE_PATH=
CPLUS_INCLUDE_PATH=/usr/include/c++
INCLUDE_FREETYPE=-I$(INCLUDE_FREETYPE_DIR)
INCLUDE_ASSIMP=-I$(INCLUDE_ASSIMP_DIR)
SRC_FILES=$(shell ls src | grep -e '\.cpp')
OBJ_FILES=$(SRC_FILES:cpp=o)
H_FILES=$(shell ls src | grep -e '\.h')
build:
	mkdir build


all: build $(OBJ_FILES)
	cd src && $(GCC) -o ../build/game $(INCLUDE_GL) $(INCLUDE_FREETYPE) $(INCLUDE_GLFW) -ldl main.cpp $(OBJ_FILES)
	
	

#dependfiles=$(SRC_FILES:cpp=d)


#
#.depend: SHELL:=/bin/bash
#.depend: 
#	@for i in "${!SRC_FILES[@]}"; do echo 'cd src && $(GCC) $(INCLUDE_GL) -c ${SRC_FILES[$i]}'; done	#	> ${dependfiles[$i]}
#
#include $(dependfiles)


gui.o:
#$(GCC) -c src/gui.cpp -o build/$@ $(INCLUDE_GL) 
input.o:
#$(GCC) -c src/input.cpp -o build/$@ $(INCLUDE_GL) 
widget.o:
#$(GCC) -c src/widget.cpp -o build/$@ $(INCLUDE_GL) 
colorings.o:
#$(GCC) -c src/colorings.cpp -o build/$@ $(INCLUDE_GL) 
material.o:
	$(GCC) -c src/material.cpp -o build/$@ $(INCLUDE_GL)
line.o:
	$(GCC) -c src/line.cpp -o build/$@ $(INCLUDE_GL)
lights.o:
	$(GCC) -c src/lights.cpp -o build/$@ $(INCLUDE_GL)
voxelization.o:
	$(GCC) -c src/voxelization.cpp -o build/$@ $(INCLUDE_GL) 
vao.o:
	$(GCC) -c src/vao.cpp -o build/$@ $(INCLUDE_GL) 
shader.o:
	$(GCC) -c src/shader.cpp -o build/$@ $(INCLUDE_GL) 
physics.o:
	$(GCC) -c src/physics.cpp -o build/$@ $(INCLUDE_GL) 
debug.o:
	$(GCC) -c src/debug.cpp -o build/$@ $(INCLUDE_GL) 
entities.o:
	$(GCC) -c src/entities.cpp -o build/$@ $(INCLUDE_GL) 
gl.o:
	$(GCC) -c src/gl.cpp -o build/$@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
gldebug.o:
	$(GCC) -c src/gldebug.cpp -o build/$@ $(INCLUDE_GL) 
mesh.o: shader.o
	$(GCC) -c src/mesh.cpp -o build/$@ $(INCLUDE_GL) 
framebuffer.o:
	$(GCC) -c src/framebuffer.cpp -o build/$@ $(INCLUDE_GL) 
model_loader.o:
	$(GCC) -c src/model_loader.cpp -o build/$@ $(INCLUDE_GL) $(INCLUDE_ASSIMP)
model.o:
	$(GCC) -c src/model.cpp -o build/$@ $(INCLUDE_GL) 
collision.o:
	$(GCC) -c src/collision.cpp -o build/$@ $(INCLUDE_GL) 
signal.o:
	$(GCC) -c src/signal.cpp -o build/$@ $(INCLUDE_GL) 
event.o:
	$(GCC) -c src/event.cpp -o build/$@ $(INCLUDE_GL) 
text.o:
	$(GCC) -c src/text.cpp -o build/$@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
font_loader.o:
	$(GCC) -c src/font_loader.cpp -o build/$@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
font.o:
	$(GCC) -c src/font.cpp -o build/$@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
camera.o:
	$(GCC) -c src/camera.cpp -o build/$@ $(INCLUDE_GL) 
quad.o:
	$(GCC) -c src/quad.cpp -o build/$@ $(INCLUDE_GL) 
keys.o:
	$(GCC) -c src/keys.cpp -o build/$@ $(INCLUDE_GL) 
mouse.o:
	$(GCC) -c src/mouse.cpp -o build/$@ $(INCLUDE_GL) 
gates.o:
	$(GCC) -c src/gates.cpp -o build/$@ $(INCLUDE_GL) 
functor.o:
	$(GCC) -c src/functor.cpp -o build/$@ $(INCLUDE_GL) 
contextwindow.o: 
	$(GCC) -c src/contextwindow.cpp -o build/$@ $(INCLUDE_GL)
app.o:
	$(GCC) -c src/app.cpp -o src/$@ $(INCLUDE_GL) $(INCLUDE_FREETYPE)
main.o:	
	$(GCC) -c src/main.cpp -o src/$@ $(INCLUDE_GL)

cleanbuild: 
	rm -r build
cleanobj:
	rm src/*.o
clean: cleanbuild
