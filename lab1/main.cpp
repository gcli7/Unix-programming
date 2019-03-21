#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "handle-string.hpp"

extern const char *proc_path;
extern const char *tcp_file_path;
extern const char *udp_file_path;

void get_pid(const char *path, Stat &stat) {
	int l = strlen(path);
	int index = l;
	for (--index; path[index] != '/'; index--);
	index++;
	for (int i = 0; index < l; index++, i++)
		stat.pid[i] = path[index];
	//printf("pid = %s\n", stat.pid);
}

void traverse_pid(const char *path, std::map<std::string, Stat> &table) {
    DIR *dir;
    struct dirent *f;
    char fd_path[LEN] = {0};
	char descriptor_path[LEN] = {0};
	char inode[N] = {0};
	char buf[LEN];
	int l;
	int indexI, indexJ;

	cat_path(fd_path, path, "fd");
    dir = opendir(fd_path);
    if (dir == NULL)
        return;

    while ((f = readdir(dir)) != NULL) {
		if (is_dot(f->d_name))
			continue;

		cat_path(descriptor_path, fd_path, f->d_name);
		memset(buf, 0, LEN);
		if (readlink(descriptor_path, buf, LEN) == -1)
			printf("Unable to read link.\n");

		l = strlen(buf);
		if (buf[0] == '/' || buf[l-1] != ']' || buf[l-2] < '0' || buf[l-2] > '9' )
			continue;

		indexI = 0;
		indexJ = 0;
		while (buf[indexI++] != '[');
		while (buf[indexI] != ']')
			inode[indexJ++] = buf[indexI++];

		if (table.find(inode) != table.end()) {
			get_pid(path, table[inode]);
			//printf("path = %s, inode = %s\n", path, inode);
		}
    }
	closedir(dir);
}

/*
 * Find all of the pid directories in /proc/.
 */
void find_pid_dir(std::map<std::string, Stat> &table) {
	DIR *dir;
	struct dirent *f;
	char pid_path[LEN] = {0};

	dir = opendir(proc_path);
	if (dir == NULL)
		return;

	while ((f = readdir(dir)) != NULL) {
		if (is_dot(f->d_name) || is_not_pid(f->d_name))
			continue;

		cat_path(pid_path, proc_path, f->d_name);
		//printf("pid_path = %s\n", pid_path);
		traverse_pid(pid_path, table);
	}
	closedir(dir);
}

/*
 * Get the information in /proc/net/tcp, tcp6, udp, udp6.
 */
void get_stat(std::vector<Stat> &base, const char *path, const char *protocol, std::map<std::string, Stat> &table) {
	FILE *file_ptr = fopen(path, "r");
	char buf[N];
	int limit;

    if (file_ptr == NULL) {
        fprintf(stderr, "Unable to open %s\n", tcp_file_path);
        exit(-1);
    }
	
	/*
	 * Read the labels.
	 */
	while(fgetc(file_ptr) != '\n');
	
	/*
	 * Tcp and tcp6 have 17 columns, udp and udp6 have 13 columns.
	 * 'limit' is for read the redundant columns.
	 */
	if (!strcmp(protocol, "tcp") || !strcmp(protocol, "tcp6"))
		limit = 8;
	else
		limit = 4;

	/*
	 * Local address is at 2, foreign address is at 3 and inode is at 10.
	 */
    while (fscanf(file_ptr, "%s", buf) != -1) {
		Stat tmp;
	    char inode[N];

		fscanf(file_ptr, "%s", tmp.local_address);
		fscanf(file_ptr, "%s", tmp.foreign_address);
		for (int i = 1; i < 7; i++)
        	fscanf(file_ptr, "%s", buf);
		fscanf(file_ptr, "%s", inode);
		table.emplace(inode, tmp);
		for (int i = 1; i < limit; i++)
        	fscanf(file_ptr, "%s", buf);
		
		//printf("inode = %s", inode);
		//printf(", local = %s", table[inode].local_address);
		//printf(", foreign = %s\n", table[inode].foreign_address);
    }
	fclose(file_ptr);
}

int main(int argc, char **argv) {
	std::vector<Stat> tcp;
	std::vector<Stat> tcp6;
	std::vector<Stat> udp;
	std::vector<Stat> udp6;
	std::map<std::string, Stat> inode_table;

	//printf("========== tcp ==========\n");
	get_stat(tcp, tcp_file_path, "tcp", inode_table);
	//printf("========== tcp6 ==========\n");
	get_stat(tcp6, tcp6_file_path, "tcp6", inode_table);
	//printf("========== udp ==========\n");
	get_stat(udp, udp_file_path, "udp", inode_table);
	//printf("========== udp6 ==========\n");
	get_stat(udp6, udp6_file_path, "udp6", inode_table);

	find_pid_dir(inode_table);

	return 0;
}
