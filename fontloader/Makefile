include ../deppaths
GCC=g++
CPLUS_INCLUDE_PATH=/usr/include/c++
INCLUDE_FREETYPE=-I$(INCLUDE_FREETYPE_DIR)
SRC_FILES=$(shell ls | grep -e '\.cpp')
OBJ_FILES=$(SRC_FILES:cpp=o)
OBJ_TARGETS=$(patsubst %, build/%, $(OBJ_FILES))

all: build $(OBJ_TARGETS) tags 
	$(GCC) -o fontldr $(OBJ_TARGETS) $(INCLUDE_FREETYPE) -L$(LIBFREETYPE_DIR) -lfreetype 

build:
	mkdir build

build/%.o: %.cpp 
	$(GCC) -o $@ -c $< -I$(INCLUDE_GLM_DIR) $(INCLUDE_FREETYPE) -L$(LIBFREETYPE_DIR) -lfreetype 

tags: 
	ctags -R .

run: all
	./fontldr

cleanbuild: 
	rm -rf build

clean: cleanbuild
	rm -f fontldr

fresh: clean all

.PHONY: clean fresh tags all
