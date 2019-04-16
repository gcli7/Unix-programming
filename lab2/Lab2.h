#ifndef LAB2_H
#define LAB2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

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
// open
static ssize_t (*real_read)(int, void *, size_t) = NULL;
static ssize_t (*real_write)(int, const void *, size_t) = NULL;
static int (*real_dup)(int) = NULL;
static int (*real_dup2)(int, int) = NULL;
static int (*real_close)(int) = NULL;
static int (*real_lstat)(int, const char *, struct stat *) = NULL;
static int (*real_stat)(int, const char *, struct stat *) = NULL;
static ssize_t (*real_pwrite)(int, const void *, size_t, off_t) = NULL;
static FILE * (*real_fopen)(const char *, const char *) = NULL;
static int (*real_fclose)(FILE *) = NULL;
static size_t (*real_fread)(void *, size_t, size_t, FILE *) = NULL;
static size_t (*real_fwrite)(const void *, size_t, size_t, FILE *) = NULL;
static int (*real_fgetc)(FILE *) = NULL;
//static int (*real_fscanf)(FILE *, const char *, ...) = NULL;
//static int (*real_fprintf)(FILE *, const char *, ...) = NULL;
static int (*real_chdir)(const char *);
static int (*real_chown)(const char *, uid_t, gid_t) = NULL;
static int (*real_chmod)(const char *, mode_t) = NULL;
static int (*real_remove)(const char *) = NULL;
static int (*real_rename)(const char *, const char *) = NULL;
static int (*real_link)(const char *, const char *) = NULL;
static int (*real_unlink)(const char *) = NULL;

#endif
