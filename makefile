
all: a

TARGET = -mmacosx-version-min=10.8
LIBS = -F/System/Library/Frameworks -framework OpenGL -framework GLUT

a: a4.c
	clang a4.c -o a4 $(LIBS) $(TARGET)

warna:
	clang a4.c -Wall -o a4 $(LIBS) $(TARGET)

ubuntu: a4.c
	gcc -o a4 a4.c -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm

run:
	./a4 test.txt