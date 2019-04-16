#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
	DIR *dir;
	int fd;
	struct stat s;
	char buf[] = {"Hello World!"};
	FILE *f;

	printf("\n\n");

	dir = opendir("testcases");
	readdir(dir);
	closedir(dir);

	fd = creat("a.txt", O_CREAT);
	write(fd, buf, sizeof(buf));
	close(fd);
	
	fd = open("a.txt", O_CREAT | O_RDWR, S_IRWXU);
	read(fd, buf, sizeof(buf));
	close(dup(fd));
	close(dup2(fd, 10));
	
	lstat("a.txt", &s);
	stat("a.txt", &s);

	strcpy(buf, "Test pwrite!");
	pwrite(fd, buf, sizeof(buf), 50);

	f = fopen("b.txt", "wb+");
	strcpy(buf, "Test fwrite!");
	fwrite(buf, sizeof(char), 12, f);
	fread(buf, sizeof(char), 5, f);
	fclose(f);

	return 0;
}
