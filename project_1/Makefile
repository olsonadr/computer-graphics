
linux: proj.cpp
	g++ -o proj proj.cpp -Linclude -Iinclude -lGL -lGLU -lglut /usr/lib64/libglut.so -lm

windows: proj.cpp
	g++ -o proj proj.cpp -Linclude -Iinclude -lopengl32 -lGLU32 -lfreeglut -lglew32 -lm

clean:
	rm proj proj.exe