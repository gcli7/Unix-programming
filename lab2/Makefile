all: test fsmon.so

test: test.o
	gcc -Wall -o test test.c

fsmon.so: Lab2.c Lab2.h
	gcc -Wall -fPIC -shared -o fsmon.so Lab2.c -ldl

runLOG:
	MONITOR_OUTPUT=fsmon.log LD_LIBRARY_PATH=/home/cheng/Code/Unix-programming/lab2 LD_PRELOAD=fsmon.so ./test

run:
	LD_LIBRARY_PATH=/home/cheng/Code/Unix-programming/lab2 LD_PRELOAD=fsmon.so ./test

clean:
	rm -f *.o
	rm -f *.so
	rm -f test
	rm -f *.txt
	rm -f *.log
