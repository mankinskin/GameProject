include deppaths

all:
	gcc -o game -I$(INCLUDE_GL_DIR) src/main.cpp
