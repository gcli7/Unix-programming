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
	fprintf(stderr, "creat(%s, %d) = %d\n", path, mode, ret);
	return ret;
}

// open

ssize_t read(int fd, void *buf, size_t count) {
	LOAD_FUNC(real_read, "read");
	ssize_t ret = real_read(fd, buf, count);
	fprintf(stderr, "read(%d, %p, %ld) = %ld\n", fd, buf, count, ret);
	return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
	LOAD_FUNC(real_write, "write");
	ssize_t ret = real_write(fd, buf, count);
	fprintf(stderr, "write(%d, %p, %ld) = %ld\n", fd, buf, count, ret);
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

int __lxstat(int __var, const char *path, struct stat *buf) {
	LOAD_FUNC(real_lstat, "__lxstat");
	int ret = real_lstat(3, path, buf);
	fprintf(stderr, "lstat(%s, %p) = %d\n", path, (void *)buf, ret);
	return ret;
}

int __xstat(int __var, const char *path, struct stat *buf) {
	LOAD_FUNC(real_stat, "__xstat");
	int ret = real_stat(3, path, buf);
	fprintf(stderr, "stat(%s, %p) = %d\n", path, (void *)buf, ret);
	return ret;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
	LOAD_FUNC(real_pwrite, "pwrite");
	ssize_t ret = real_pwrite(fd, buf, count, offset);
	fprintf(stderr, "pwrite(%d, %p, %ld, %ld) = %ld\n", fd, (void *)buf, count, offset, ret);
	return ret;
}

FILE *fopen(const char *pathname, const char *mode) {
	LOAD_FUNC(real_fopen, "fopen");
	FILE *ret = real_fopen(pathname, mode);
	fprintf(stderr, "fopen(%s, %s) = %p\n", pathname, mode, (void *)ret);
	return ret;
}

int fclose(FILE *stream) {
	LOAD_FUNC(real_fclose, "fclose");
	int ret = real_fclose(stream);
	fprintf(stderr, "fclose(%p) = %d\n", (void *)stream, ret);
	return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	LOAD_FUNC(real_fread, "fread");
	int ret = real_fread(ptr, size, nmemb, stream);
	fprintf(stderr, "fread(%p, %ld, %ld, %p) = %d\n", ptr, size, nmemb, (void *)stream, ret);
	return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	LOAD_FUNC(real_fwrite, "fwrite");
	int ret = real_fwrite(ptr, size, nmemb, stream);
	fprintf(stderr, "fwrite(%p, %ld, %ld, %p) = %d\n", ptr, size, nmemb, (void *)stream, ret);
	return ret;
}
