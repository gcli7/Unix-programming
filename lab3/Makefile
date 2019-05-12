all:
	yasm -f elf64 -DYASM -D__x86_64__ -DPIC libmini64.asm -o libmini64.o
	gcc -c -g -Wall -fno-stack-protector -fPIC -nostdlib libmini.c
	ld -shared -o libmini.so libmini64.o libmini.o
	yasm -f elf64 -DYASM -D__x86_64__ -DPIC start.asm -o start.o

testmini:
	gcc -c -g -Wall -fno-stack-protector -nostdlib -I. -I.. -DUSEMINI testmini.c
	ld -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -o testmini testmini.o start.o -L. -L.. -lmini

clean:
	rm -f *.o *.so alarm1 alarm2 alarm3 jmp1 testmini
