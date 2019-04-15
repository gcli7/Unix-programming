#ifndef LAB2_H
#define LAB2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

#define LOAD_FUNC(f, lib_f) do { \
								void *handle = dlopen("libc.so.6", RTLD_LAZY); \
								if (!handle) { \
									fprintf(stderr, "libc error!\n"); \
									exit(-1); \
								} \
								f = dlsym(handle, lib_f); \
								if (!f) { \
									fprintf(stderr, "%s error!\n", lib_f); \
									exit(-1); \
								} \
							} while (0)

static int (*real_closedir)(DIR *) = NULL;
static DIR * (*real_opendir)(const char *) = NULL;
static struct dirent * (*real_readdir)(DIR *) = NULL;
static int (*real_creat)(const char *, mode_t) = NULL;

#endif
