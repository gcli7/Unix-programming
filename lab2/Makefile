all: fsmon.so

fsmon.so: Lab2.c Lab2.h
	gcc -Wall -fPIC -shared -o fsmon.so Lab2.c -ldl

runLOG:
	MONITOR_OUTPUT=fsmon.log LD_PRELOAD=./fsmon.so ./test

run:
	LD_PRELOAD=./fsmon.so ./test

clean:
	rm -f *.o
	rm -f *.so
	rm -f test
	rm -f *.txt
	rm -f *.log
