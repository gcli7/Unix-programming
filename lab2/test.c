#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>

int main() {
	DIR *dir;
	//dir = opendir(".");
	dir = opendir("testcases");
	printf("\t===== opendir =====\n");

	readdir(dir);
	printf("\t===== readdir =====\n");

	closedir(dir);
	printf("\t===== close =====\n");

	creat("a.txt", S_IRWXU);

	return 0;
}
