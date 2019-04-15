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
