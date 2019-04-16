#include "Lab2.h"

int closedir(DIR *dir) {
	fprintf(stderr, "closedir(%p) = ", (void *)dir);
	LOAD_FUNC(real_closedir, "closedir");
	int ret = real_closedir(dir);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

DIR * opendir(const char *s) {
	fprintf(stderr, "opendir(\"%s\") = ", s);
	LOAD_FUNC(real_opendir, "opendir");
	DIR *ret = real_opendir(s);
	fprintf(stderr, "%p\n", (void *)ret);
	return ret;
}

struct dirent * readdir(DIR *dir) {
	fprintf(stderr, "readdir(%p) = ", (void *)dir);
	LOAD_FUNC(real_readdir, "readdir");
	struct dirent *ret = real_readdir(dir);
	fprintf(stderr, "%p\n", (void *)ret);
	return ret;
}

int creat(const char *path, mode_t mode) {
	fprintf(stderr, "creat(\"%s\", %d) = ", path, mode);
	LOAD_FUNC(real_creat, "creat");
	int ret = real_creat(path, mode);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

// open

ssize_t read(int fd, void *buf, size_t count) {
	fprintf(stderr, "read(%d, %p, %ld) = ", fd, buf, count);
	LOAD_FUNC(real_read, "read");
	ssize_t ret = real_read(fd, buf, count);
	fprintf(stderr, "%ld\n", ret);
	return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
	fprintf(stderr, "write(%d, %p, %ld) = ", fd, buf, count);
	LOAD_FUNC(real_write, "write");
	ssize_t ret = real_write(fd, buf, count);
	fprintf(stderr, "%ld\n", ret);
	return ret;
}

int dup(int oldfd) {
	fprintf(stderr, "dup(%d) = ", oldfd);
	LOAD_FUNC(real_dup, "dup");
	int ret = real_dup(oldfd);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int dup2(int oldfd, int newfd) {
	fprintf(stderr, "dup2(%d, %d) = ", oldfd, newfd);
	LOAD_FUNC(real_dup2, "dup2");
	int ret = real_dup2(oldfd, newfd);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int close(int fd) {
	fprintf(stderr, "close(%d) = ", fd);
	LOAD_FUNC(real_close, "close");
	int ret = real_close(fd);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int __lxstat(int __var, const char *path, struct stat *buf) {
	fprintf(stderr, "lstat(\"%s\", %p) = ", path, (void *)buf);
	LOAD_FUNC(real_lstat, "__lxstat");
	int ret = real_lstat(3, path, buf);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int __xstat(int __var, const char *path, struct stat *buf) {
	fprintf(stderr, "stat(\"%s\", %p) = ", path, (void *)buf);
	LOAD_FUNC(real_stat, "__xstat");
	int ret = real_stat(3, path, buf);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
	fprintf(stderr, "pwrite(%d, %p, %ld, %ld) = ", fd, (void *)buf, count, offset);
	LOAD_FUNC(real_pwrite, "pwrite");
	ssize_t ret = real_pwrite(fd, buf, count, offset);
	fprintf(stderr, "%ld\n", ret);
	return ret;
}

FILE *fopen(const char *pathname, const char *mode) {
	fprintf(stderr, "fopen(\"%s\", \"%s\") = ", pathname, mode);
	LOAD_FUNC(real_fopen, "fopen");
	FILE *ret = real_fopen(pathname, mode);
	fprintf(stderr, "%p\n", (void *)ret);
	return ret;
}

int fclose(FILE *stream) {
	fprintf(stderr, "fclose(%p) = ", (void *)stream);
	LOAD_FUNC(real_fclose, "fclose");
	int ret = real_fclose(stream);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	fprintf(stderr, "fread(%p, %ld, %ld, %p) = ", ptr, size, nmemb, (void *)stream);
	LOAD_FUNC(real_fread, "fread");
	size_t ret = real_fread(ptr, size, nmemb, stream);
	fprintf(stderr, "%ld\n", ret);
	return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	fprintf(stderr, "fwrite(%p, %ld, %ld, %p) = ", ptr, size, nmemb, (void *)stream);
	LOAD_FUNC(real_fwrite, "fwrite");
	size_t ret = real_fwrite(ptr, size, nmemb, stream);
	fprintf(stderr, "%ld\n", ret);
	return ret;
}

int fgetc(FILE *stream) {
	fprintf(stderr, "fgetc(%p) = ", (void *)stream);
	LOAD_FUNC(real_fgetc, "fgetc");
	int ret = real_fgetc(stream);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

/*
int fscanf(FILE *stream, const char *format, ...) {
	fprintf(stderr, "fscanf(%p, %s, ...) = ", (void *)stream, format);
	LOAD_FUNC(real_fscanf, "fscanf");
	int ret = real_fscanf(stream, format, ...);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int fprintf(FILE *stream, const char *format, ...) {
	fprintf(stderr, "fgetc(%p, %s, ...) = ", (void *)stream, format);
	LOAD_FUNC(real_fprintf, "fprintf");
	int ret = real_fprintf(stream, format, ...);
	fprintf(stderr, "%d\n", ret);
	return ret;
}
*/

int chdir(const char *path) {
	fprintf(stderr, "chdir(\"%s\") = ", path);
	LOAD_FUNC(real_chdir, "chdir");
	int ret = real_chdir(path);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int chown(const char *pathname, uid_t owner, gid_t group) {
	fprintf(stderr, "chown(\"%s\", %d, %d) = ", pathname, owner, group);
	LOAD_FUNC(real_chown, "chown");
	int ret = real_chown(pathname, owner, group);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int chmod(const char *pathname, mode_t mode) {
	fprintf(stderr, "chmod(\"%s\", %d) = ", pathname, mode);
	LOAD_FUNC(real_chmod, "chmod");
	int ret = real_chmod(pathname, mode);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int remove(const char *pathname) {
	fprintf(stderr, "remove(\"%s\") = ", pathname);
	LOAD_FUNC(real_remove, "remove");
	int ret = real_remove(pathname);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int rename(const char *oldpath, const char *newpath) {
	fprintf(stderr, "rename(\"%s\", \"%s\") = ", oldpath, newpath);
	LOAD_FUNC(real_rename, "rename");
	int ret = real_rename(oldpath, newpath);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int link(const char *oldpath, const char *newpath) {
	fprintf(stderr, "link(\"%s\", \"%s\") = ", oldpath, newpath);
	LOAD_FUNC(real_link, "link");
	int ret = real_link(oldpath, newpath);
	fprintf(stderr, "%d\n", ret);
	return ret;
}

int unlink(const char *pathname) {
	fprintf(stderr, "unlink(\"%s\") = ", pathname);
	LOAD_FUNC(real_unlink, "unlink");
	int ret = real_unlink(pathname);
	fprintf(stderr, "%d\n", ret);
	return ret;
}
