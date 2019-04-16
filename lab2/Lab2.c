#include "Lab2.h"

int closedir(DIR *dir) {
	LOAD_FUNC(real_closedir, "closedir");
	int ret = real_closedir(dir);
	fprintf(stderr, "closedir(%p) = %d\n", (void *)dir, ret);
	return ret;
}

DIR * opendir(const char *s) {
	LOAD_FUNC(real_opendir, "opendir");
	DIR *ret = real_opendir(s);
	fprintf(stderr, "opendir(\"%s\") = %p\n", s, (void *)ret);
	return ret;
}

struct dirent * readdir(DIR *dir) {
	LOAD_FUNC(real_readdir, "readdir");
	struct dirent *ret = real_readdir(dir);
	fprintf(stderr, "readdir(%p) = %p\n", (void *)dir, (void *)ret);
	return ret;
}

int creat(const char *path, mode_t mode) {
	LOAD_FUNC(real_creat, "creat");
	int ret = real_creat(path, mode);
	fprintf(stderr, "creat(%p, %d) = %d\n", (void *)path, mode, ret);
	return ret;
}

// open

ssize_t read(int fd, void *buf, size_t count) {
	LOAD_FUNC(real_read, "read");
	int ret = real_read(fd, buf, count);
	fprintf(stderr, "read(%d, %p, %ld) = %d\n", fd, buf, count, ret);
	return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
	LOAD_FUNC(real_write, "write");
	int ret = real_write(fd, buf, count);
	fprintf(stderr, "write(%d, %p, %ld) = %d\n", fd, buf, count, ret);
	return ret;
}

int dup(int oldfd) {
	LOAD_FUNC(real_dup, "dup");
	int ret = real_dup(oldfd);
	fprintf(stderr, "dup(%d) = %d\n", oldfd, ret);
	return ret;
}

int dup2(int oldfd, int newfd) {
	LOAD_FUNC(real_dup2, "dup2");
	int ret = real_dup2(oldfd, newfd);
	fprintf(stderr, "dup2(%d, %d) = %d\n", oldfd, newfd, ret);
	return ret;
}

int close(int fd) {
	LOAD_FUNC(real_close, "close");
	int ret = real_close(fd);
	fprintf(stderr, "close(%d) = %d\n", fd, ret);
	return ret;
}

int lstat(const char *path, struct stat *buf) {
	LOAD_FUNC(real_lstat, "lstat");
	int ret = real_lstat(path, buf);
	fprintf(stderr, "lstat(%p, %p) = %d\n", (void *)path, (void *)buf, ret);
	return ret;
}

int stat(const char *path, struct stat *buf) {
	LOAD_FUNC(real_stat, "stat");
	int ret = real_stat(path, buf);
	fprintf(stderr, "stat(%p, %p) = %d\n", (void *)path, (void *)buf, ret);
	return ret;
}
