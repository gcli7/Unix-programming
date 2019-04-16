all: test demo.so

test: test.o
	gcc -Wall -o test test.c

demo.so: Lab2.c Lab2.h
	gcc -Wall -fPIC -shared -o demo.so Lab2.c -ldl

run:
	LD_LIBRARY_PATH=/home/cheng/Code/Unix-programming/lab2 LD_PRELOAD=demo.so ./test
	echo $$$$
	echo $$$$

clean:
	rm -f *.o
	rm -f demo.so
	rm -f test
