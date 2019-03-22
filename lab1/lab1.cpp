#include <cstdio>
#include <cstdlib>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include "handle-string.hpp"

extern const char *proc_path;
extern const char *tcp_file_path;
extern const char *udp_file_path;
std::vector<Stat> table;

/*
 * Find the pid and program, then store them in stat.
 */
void store_pid_and_program(const char *descriptor_path, Stat &stat) {
	const char *pid_start_ptr, *pid_end_ptr;
	char cmdline_path[LEN] = {0};
	FILE *cmdline_file_ptr;

	/*
     * Get the pid.
     */
	//printf("descriptor_path = %s\n", descriptor_path);
	pid_start_ptr = strchr(descriptor_path + 1, '/') + 1;
	pid_end_ptr = strchr(pid_start_ptr, '/');
	strncpy(stat.pid, pid_start_ptr, pid_end_ptr - pid_start_ptr);
	//printf("\tpid = %s\n", stat.pid);

	/*
     * Get the program and arguments.
     */
	cat_path(cmdline_path, proc_path, stat.pid);
	cat_path(cmdline_path, cmdline_path, "cmdline");

	cmdline_file_ptr = fopen(cmdline_path, "r");
	if (cmdline_file_ptr == NULL) {
		fprintf(stderr, "Unable to open the cmdline: %s\n", cmdline_path);
		exit(-1);
	}

	fgets(stat.program, LEN, cmdline_file_ptr);
	if (stat.program[0] == '/')
		strcpy(stat.program, strrchr(stat.program, '/') + 1);
	//printf("\t\tcmdline_path = %s, cmdline = %s\n", cmdline_path, stat.program);

	fclose(cmdline_file_ptr);
}

/*
 * Look for the descriptor which has the same inode as someone in table.
 */
void find_inode(const char *descriptor_path) {
	char link[LEN] = {0};
	char link_inode[N] = {0};
	const char *inode_start_ptr, *inode_end_ptr;

	if (readlink(descriptor_path, link, LEN) == -1) {
		fprintf(stderr, "Unable to read the link: %s\n", descriptor_path);
        exit(-1);
	}

	if (strstr(link, "socket") == NULL)
		return;

	/*
	 * Get the inode.
	 */
	inode_start_ptr = strchr(link, '[') + 1;
	inode_end_ptr = strchr(link, ']');
	strncpy(link_inode, inode_start_ptr, inode_end_ptr - inode_start_ptr);
	//printf("link_inode = %s\n", link_inode);

	for (auto &stat : table)
		if (!strcmp(link_inode, stat.inode)) {
			store_pid_and_program(descriptor_path, stat);
			break;
		}
}

/*
 * Traverse the descriptors of someone pid.
 * /proc/{pid}/fd/{descriptors}
 */
void traverse_descriptors(const char *pid_path, std::vector<Stat> &table) {
	DIR *dir;
	struct dirent *files;
	char fd_path[LEN] = {0};

	cat_path(fd_path, pid_path, "fd");
	//printf("\tfd_path = %s\n", fd_path);
	dir = opendir(fd_path);
	if (dir == NULL) {
		fprintf(stderr, "Unable to open the folder: %s\n", fd_path);
		exit(-1);
	}

	while ((files = readdir(dir)) != NULL) {
		if (is_dot(files->d_name))
			continue;

		char descriptor_path[LEN] = {0};

		cat_path(descriptor_path, fd_path, files->d_name);
		//printf("\tdescriptor_path = %s\n", descriptor_path);
		find_inode(descriptor_path);
	}
	closedir(dir);
}

/*
 * Traverse the pid folders.
 * /proc/{pid}.
 */
void traverse_pid_folders() {
	DIR *dir;
	struct dirent *files;
	char pid_path[LEN] = {0};

	dir = opendir(proc_path);
	if (dir == NULL) {
		fprintf(stderr, "Unable to open the folder: %s\n", proc_path);
		exit(-1);
	}

	while ((files = readdir(dir)) != NULL) {
		if (is_dot(files->d_name) || is_not_pid(files->d_name))
			continue;

		cat_path(pid_path, proc_path, files->d_name);
		//printf("pid_path = %s, files->d_name = %s\n", pid_path, files->d_name);
		traverse_descriptors(pid_path, table);
	}
	closedir(dir);
}

/*
 * Get the information in /proc/net/tcp, tcp6, udp, udp6.
 */
void get_stat(const char *path, const char *protocol) {
	FILE *file_ptr;
	Stat stat = {0};
	char buf[LEN] = {0};
	int redundant = 0;

	file_ptr = fopen(path, "r");
	if (file_ptr == NULL) {
		fprintf(stderr, "Unable to open the net file: %s\n", path);
		exit(-1);
	}

	/*
	 * Tcp and tcp6 have 17 columns, udp and udp6 have 13 columns.
	 * 'redundant' is for read the redundant columns.
	 */
	if (protocol[0] == 't')
		redundant = 8;
	else
		redundant = 4;

	/*
	 * Read the labels.
	 */
	fgets(buf, LEN, file_ptr);

	/*
	 * Local address is at 2, foreign address is at 3 and inode is at 10.
	 */
	strcpy(stat.protocol, protocol);
	while (fscanf(file_ptr, "%s", buf) != -1) {
		fscanf(file_ptr, "%s", stat.local_address);
		fscanf(file_ptr, "%s", stat.foreign_address);
		for (int i = 1; i < 7; i++)
			fscanf(file_ptr, "%s", buf);
		fscanf(file_ptr, "%s", stat.inode);
		for (int i = 1; i < redundant; i++)
			fscanf(file_ptr, "%s", buf);
		table.emplace_back(stat);

		//printf("protocol = %s, ", stat.protocol);
		//printf("inode = %s, ", stat.inode);
		//printf("local = %s, ", stat.local_address);
		//printf("foreign = %s\n", stat.foreign_address);
	}
	fclose(file_ptr);
}

void run() {
	//printf("========== tcp ==========\n");
	get_stat(tcp_file_path, "tcp");
	//printf("========== tcp6 ==========\n");
	get_stat(tcp6_file_path, "tcp6");
	//printf("========== udp ==========\n");
	get_stat(udp_file_path, "udp");
	//printf("========== udp6 ==========\n");
	get_stat(udp6_file_path, "udp6");

	traverse_pid_folders();
	
	printf("%-6s%-14s%-24s%s\n", "Proto", "Local Address", "Foreign Address", "PID/Program name and arguments");
	for (auto &stat : table)
		printf("%-6s%-24s%-24s%s/%s\n", stat.protocol, stat.local_address, stat.foreign_address, stat.pid, stat.program);
}

int main(int argc, char **argv) {
	run();

	return 0;
}
