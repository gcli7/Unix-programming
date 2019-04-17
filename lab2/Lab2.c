#include "Lab2.h"

__attribute__((constructor)) void set_output_fd() {
	const char *s = getenv("MONITOR_OUTPUT");
	LOAD_FUNC(output_func, "fprintf");
	if (s) {
		LOAD_FUNC(real_open, "open");
		LOAD_FUNC(real_dup2, "dup2");
		LOAD_FUNC(real_close, "close");

		output_fd = real_open(s, O_CREAT | O_TRUNC | O_WRONLY,
							  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		real_dup2(output_fd, STDERR_FILENO);
		real_close(output_fd);
	}
	output_func(stderr, "%s\n", "test");
}

void read_fd_link(int fd) {
	char path[256] = {'\0'};
	LOAD_FUNC(real_readlink, "readlink");

	sprintf(path, "/proc/self/fd/%d", fd);
	real_readlink(path, fd_name, sizeof(fd_name));
}

char * get_fd_name(int fd) {
	char *current_path = get_current_dir_name();
	char *ptr = NULL;

	switch (fd) {
		case 0:
			strcpy(fd_name, "STDIN");
			break;
		case 1:
			strcpy(fd_name, "STDOUT");
			break;
		case 2:
			strcpy(fd_name, "STDERR");
			break;
		default:
			read_fd_link(fd);
			break;
	}

	ptr = strstr(fd_name, current_path);
	if (ptr) {
		char tmp[256] = {"."};
		strcat(tmp, ptr + strlen(current_path));
		strcpy(fd_name, tmp);
	}

	return fd_name;
}

/*
 * Monitored Functions.
 */
int closedir(DIR *dir) {
	get_fd_name(dirfd(dir));
	LOAD_FUNC(real_closedir, "closedir");
	int ret = real_closedir(dir);
	output_func(stderr, "# closedir(%p) = %d\n", (void *)dir, ret);
	return ret;
}

DIR * opendir(const char *s) {
	LOAD_FUNC(real_opendir, "opendir");
	DIR *ret = real_opendir(s);
	if (ret)
		output_func(stderr, "# opendir(\"%s\") = %p\n", s, (void *)ret);
	else
		output_func(stderr, "# opendir(\"%s\") = NULL\n", s);
	return ret;
}

struct dirent * readdir(DIR *dir) {
	LOAD_FUNC(real_readdir, "readdir");
	struct dirent *ret = real_readdir(dir);
	if (ret)
		output_func(stderr, "# readdir(\"%s\") = %s\n",
					get_fd_name(dirfd(dir)), ret->d_name);
	else
		output_func(stderr, "# readdir(%p) = NULL\n", (void *)dir);
	return ret;
}

int creat(const char *path, mode_t mode) {
	LOAD_FUNC(real_creat, "creat");
	int ret = real_creat(path, mode);
	output_func(stderr, "# creat(\"%s\", 0x%x) = %d\n", path, mode, ret);
	return ret;
}

int open(const char *pathname, int flags, ...) {
	va_list args;
	va_start(args, flags);
	mode_t m = va_arg(args, mode_t);
	va_end(args);

	LOAD_FUNC(real_open, "open");
	int ret = real_open(pathname, flags, m);
	if (flags & (O_CREAT | O_TMPFILE))
		output_func(stderr, "# open(\"%s\", 0x%x, 0x%x) = %d\n",
					pathname, flags, m, ret);
	else
		output_func(stderr, "# open(\"%s\", 0x%x) = %d\n",
					pathname, flags, ret);
	return ret;
}

ssize_t read(int fd, void *buf, size_t count) {
	LOAD_FUNC(real_read, "read");
	ssize_t ret = real_read(fd, buf, count);
	output_func(stderr, "# read(\"%s\", %p, %ld) = %ld\n",
				get_fd_name(fd), buf, count, ret);
	return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
	LOAD_FUNC(real_write, "write");
	ssize_t ret = real_write(fd, buf, count);
	output_func(stderr, "# write(\"%s\", %p, %ld) = %d\n",
				get_fd_name(fd), buf, count, ret);
	return ret;
}

int dup(int oldfd) {
	LOAD_FUNC(real_dup, "dup");
	int ret = real_dup(oldfd);
	output_func(stderr, "# dup(\"%s\") = %d\n",
				get_fd_name(oldfd), ret);
	return ret;
}

int dup2(int oldfd, int newfd) {
	LOAD_FUNC(real_dup2, "dup2");
	int ret = real_dup2(oldfd, newfd);
	output_func(stderr, "# dup2(\"%s\", \"%s\") = %d\n",
				get_fd_name(oldfd), get_fd_name(newfd), ret);
	return ret;
}

int close(int fd) {
	output_func(stderr, "# close(\"%s\") = ", get_fd_name(fd));
	LOAD_FUNC(real_close, "close");
	int ret = real_close(fd);
	output_func(stderr, "%d\n", ret);
	return ret;
}

int __lxstat(int __var, const char *path, struct stat *buf) {
	LOAD_FUNC(real_lstat, "__lxstat");
	int ret = real_lstat(__var, path, buf);
	output_func(stderr, "# lstat(\"%s\", %p {mode = %05o, size = %d}) = %d\n",
				path, (void *)buf, buf->st_mode, buf->st_size, ret);
	return ret;
}

int __xstat(int __var, const char *path, struct stat *buf) {
	LOAD_FUNC(real_stat, "__xstat");
	int ret = real_stat(__var, path, buf);
	output_func(stderr, "# stat(\"%s\", %p {mode = %05o, size = %d}) = %d\n",
				path, (void *)buf, buf->st_mode, buf->st_size, ret);
	return ret;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
	LOAD_FUNC(real_pwrite, "pwrite");
	ssize_t ret = real_pwrite(fd, buf, count, offset);
	output_func(stderr, "# pwrite(%d, %p, %ld, %ld) = %ld\n",
				get_fd_name(fd), (void *)buf, count, offset, ret);
	return ret;
}

FILE *fopen(const char *pathname, const char *mode) {
	LOAD_FUNC(real_fopen, "fopen");
	FILE *ret = real_fopen(pathname, mode);
	output_func(stderr, "# fopen(\"%s\", \"%s\") = %p\n", pathname, mode, (void *)ret);
	return ret;
}

int fclose(FILE *stream) {
	get_fd_name(fileno(stream));
	LOAD_FUNC(real_fclose, "fclose");
	int ret = real_fclose(stream);
	output_func(stderr, "# fclose(\"%s\") = %d\n", fd_name, ret);
	return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	LOAD_FUNC(real_fread, "fread");
	size_t ret = real_fread(ptr, size, nmemb, stream);
	output_func(stderr, "# fread(%p, %ld, %ld, \"%s\") = %ld\n",
				ptr, size, nmemb, get_fd_name(fileno(stream)), ret);
	return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	LOAD_FUNC(real_fwrite, "fwrite");
	size_t ret = real_fwrite(ptr, size, nmemb, stream);
	output_func(stderr, "# fwrite(%p, %ld, %ld, \"%s\") = %ld\n",
				ptr, size, nmemb, get_fd_name(fileno(stream)), ret);
	return ret;
}

int fgetc(FILE *stream) {
	LOAD_FUNC(real_fgetc, "fgetc");
	int ret = real_fgetc(stream);
	output_func(stderr, "# fgetc(\"%s\") = %d\n", get_fd_name(fileno(stream)), ret);
	return ret;
}

char * fgets(char *s, int size, FILE *stream) {
	LOAD_FUNC(real_fgets, "fgets");
	char *ret = real_fgets(s, size, stream);
	if (ret)
		output_func(stderr, "# fgets(\"%s\", %d, \"%s\") = %s\n", s, size, get_fd_name(fileno(stream)), ret);
	else
		output_func(stderr, "# fgets(\"%s\", %d, \"%s\") = NULL\n", s, size, get_fd_name(fileno(stream)));
	return ret;
}

int __isoc99_fscanf(FILE *stream, const char *format, ...) {
	va_list arg_list;
    va_start(arg_list, format);
	int ret = vfscanf(stream, format, arg_list);
    va_end(arg_list);
	output_func(stderr, "# fscanf(\"%s\", %s, ...) = %d\n",
				get_fd_name(fileno(stream)), format, ret);
	return ret;
}

int fprintf(FILE *stream, const char *format, ...) {
	va_list arg_list;
    va_start(arg_list, format);
    int ret = vfprintf(stream, format, arg_list);
    va_end(arg_list);
	output_func(stderr, "# fprintf(\"%s\", %s, ...) = %d\n",
				get_fd_name(fileno(stream)), format, ret);
	return ret;
}

int chdir(const char *path) {
	LOAD_FUNC(real_chdir, "chdir");
	int ret = real_chdir(path);
	output_func(stderr, "# chdir(\"%s\") = %d\n", path, ret);
	return ret;
}

int chown(const char *pathname, uid_t owner, gid_t group) {
	LOAD_FUNC(real_chown, "chown");
	int ret = real_chown(pathname, owner, group);
	output_func(stderr, "# chown(\"%s\", %d, %d) = %d\n", pathname, owner, group, ret);
	return ret;
}

int chmod(const char *pathname, mode_t mode) {
	LOAD_FUNC(real_chmod, "chmod");
	int ret = real_chmod(pathname, mode);
	output_func(stderr, "# chmod(\"%s\", %05o) = %d\n", pathname, mode, ret);
	return ret;
}

int remove(const char *pathname) {
	LOAD_FUNC(real_remove, "remove");
	int ret = real_remove(pathname);
	output_func(stderr, "# remove(\"%s\") = %d\n", pathname, ret);
	return ret;
}

int rename(const char *oldpath, const char *newpath) {
	LOAD_FUNC(real_rename, "rename");
	int ret = real_rename(oldpath, newpath);
	output_func(stderr, "# rename(\"%s\", \"%s\") = %d\n", oldpath, newpath, ret);
	return ret;
}

int link(const char *oldpath, const char *newpath) {
	LOAD_FUNC(real_link, "link");
	int ret = real_link(oldpath, newpath);
	output_func(stderr, "link(\"%s\", \"%s\") = %d\n", oldpath, newpath, ret);
	return ret;
}

int unlink(const char *pathname) {
	LOAD_FUNC(real_unlink, "unlink");
	int ret = real_unlink(pathname);
	output_func(stderr, "# unlink(\"%s\") = %d\n", pathname, ret);
	return ret;
}

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
	LOAD_FUNC(real_readlink, "readlink");
	ssize_t ret = real_readlink(pathname, buf, bufsiz);
	output_func(stderr, "# readlink(\"%s\", \"%s\", %ld) = %ld\n",
				pathname, buf, bufsiz, ret);
	return ret;
}

int symlink(const char *target, const char *linkpath) {
	LOAD_FUNC(real_symlink, "symlink");
	int ret = real_symlink(target, linkpath);
	output_func(stderr, "# symlink(\"%s\", \"%s\") = %d\n", target, linkpath, ret);
	return ret;
}

int mkdir(const char *pathname, mode_t mode) {
	LOAD_FUNC(real_mkdir, "mkdir");
	int ret = real_mkdir(pathname, mode);
	output_func(stderr, "# mkdir(\"%s\", %d) = %d\n", pathname, mode, ret);
	return ret;
}

int rmdir(const char *pathname) {
	LOAD_FUNC(real_rmdir, "rmdir");
	int ret = real_rmdir(pathname);
	output_func(stderr, "# rmdir(\"%s\") = %d\n", pathname, ret);
	return ret;
}
