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

	dir = opendir("testcases");
	printf("\t===== opendir =====\n");

	readdir(dir);
	printf("\t===== readdir =====\n");

	closedir(dir);
	printf("\t===== close =====\n");

	fd = creat("a.txt", O_CREAT);
	printf("\t===== creat =====\n");

	write(fd, buf, sizeof(buf));
	printf("\t===== write =====\n");

	close(fd);
	printf("\t===== close =====\n");
	
	fd = open("a.txt", O_RDONLY);
	printf("\t===== fopen =====\n");

	read(fd, buf, sizeof(buf));
	printf("\t===== read =====\n");

	close(dup(fd));
	printf("\t===== close and dup =====\n");

	close(dup2(fd, 10));
	printf("\t===== close and dup2 =====\n");

	lstat("testcases", &s);
	printf("\t===== lstat =====\n");

	stat("testcases", &s);
	printf("\t===== stat =====\n");

	return 0;
}
