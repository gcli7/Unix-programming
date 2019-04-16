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
	char buf[256] = {"Hello World!"};
	FILE *f;

	dir = opendir("testcases");
	readdir(dir);
	closedir(dir);

	fd = creat("a.txt", O_RDWR | S_IRUSR | S_IWUSR);
	write(fd, buf, strlen(buf));
	close(fd);
	
	fd = open("z.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	close(fd);
	fd = open("a.txt", O_CREAT | O_RDWR, S_IRWXU);
	read(fd, buf, sizeof(buf));
	close(dup(fd));
	close(dup2(fd, 10));
	strcpy(buf, "Test pwrite!");
	pwrite(fd, buf, strlen(buf), 6);
	close(fd);
	
	lstat("a.txt", &s);
	stat("a.txt", &s);
	//perror("stat");

	f = fopen("b.txt", "w+");
	strcpy(buf, "Test fwrite!");
	fwrite(buf, sizeof(char), 12, f);
	fclose(f);

	f = fopen("b.txt", "r");
	fgetc(f);
	fread(buf, sizeof(char), 5, f);
	fclose(f);

	chdir("testcases");
	chdir("..");
	chown("b.txt", -1, -1);
	chmod("b.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	fclose(fopen("test.txt", "wb+"));
	remove("test.txt");
	rename("a.txt", "c.txt");

	link("b.txt", "hard_link");
	unlink("hard_link");
	symlink("c.txt", "soft_link");
	readlink("t", buf, sizeof(buf));
	unlink("soft_link");

	mkdir("mk_dir", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	rmdir("mk_dir");

	return 0;
}
