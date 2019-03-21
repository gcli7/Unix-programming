#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include "handle-string.hpp"

extern const char *proc_path;
extern const char *tcp_file_path;
extern const char *udp_file_path;

void traverse_pid(const char *pid_path) {
	DIR *dir;
	struct dirent *f;
	char tmp[LEN] = {0};

	dir = opendir(proc_path);
	if (dir == NULL)
		return;

	while ((f = readdir(dir)) != NULL) {
		if (strcmp(f->d_name, "cmdline"))
			continue;
		
		cat_path(tmp, pid_path, f->d_name);
		FILE *fptr = fopen(tmp, "r");
		while (fgets(tmp, LEN, fptr) != NULL) {
			printf("%s\n", tmp);
		}
	}
}

void traverse_proc() {
	DIR *dir;
	struct dirent *f;
	char tmp[LEN] = {0};

	dir = opendir(proc_path);
	if (dir == NULL)
		return;

	while ((f = readdir(dir)) != NULL) {
		if (!strcmp(f->d_name, ".") || !strcmp(f->d_name, "..") || is_contain_letter(f->d_name))
			continue;

		printf("%s\n", f->d_name);
		cat_path(tmp, proc_path, f->d_name);
		traverse_pid(tmp);
	}
}

int main(int argc, char **argv) {
	/*
	FILE *tcp_file = fopen(tcp_file_path, "r");
	FILE *udp_file = fopen(udp_file_path, "r");

	if (tcp_file_path == NULL) {
		printf("The path of the tcp file is wrong.\n");
		exit(-1);
	}
	if (udp_file_path == NULL) {
		printf("The path of the udp file is wrong.\n");
		exit(-1);
	}

	char tmp[LEN] = {0};

	while (fgets(tmp, LEN, tcp_file) != NULL) {
		printf("%s", tmp);
	}
	puts("");
	while (fgets(tmp, LEN, udp_file) != NULL) {
		printf("%s", tmp);
	}
	*/

	traverse_proc();
	
	return 0;
}
