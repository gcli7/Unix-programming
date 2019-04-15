all: clean
	gcc -o test test.c
	gcc -fPIC -shared -o demo.so Lab2.c -ldl

run:
	export LD_LIBRARY_PATH=/home/cheng/Code/Unix-programming/lab2
	LD_PRELOAD=demo.so ./test

clean:
	rm -f test.o
	rm -f demo.so
	rm -f test
