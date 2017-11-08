mainCPlus: main.cc
	g++ -L. main.cc -lmyfunc -o $@
mainC: main.c
	gcc -L. main.c -lmyfunc -o $@
libmyfunc.so: func1.o
	gcc -shared -fPIC -o libmyfunc.so func1.o
func1.o: func1.c
	gcc -c -fPIC func1.c
