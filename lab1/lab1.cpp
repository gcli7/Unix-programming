#include <cstdio>
#include <cstdlib>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <getopt.h>
#include <regex.h>
#include "handle-string.hpp"

extern const char *proc_path;
extern const char *tcp_file_path;
extern const char *udp_file_path;
std::vector<Stat> table;

/*
 * According to the argc and argv, decide what should be displayed.
 */
void display_result(int &argc, char **argv) {
	bool tcp_flag = false;
	bool udp_flag = false;
	bool display_flag = true;
	bool filter_flag = false;
	char filter_string[LEN] = {0};
	regex_t regex;
	const char *opt_string = "tu";
	struct option opts[] = {
		{"tcp", 0, NULL, 't'},
		{"udp", 0, NULL, 'u'},
	};
	char c = 0;

	while((c = getopt_long(argc, argv, opt_string, opts, NULL)) != -1) {
		switch(c) {
			case 't':
				tcp_flag = true;
				break;
			case 'u':
				udp_flag = true;
				break;
			case '?':
				exit(-1);
		}
	}
	if (!(tcp_flag | udp_flag)) {
		tcp_flag = true;
		udp_flag = true;
	}

	if (argv[argc-1][0] != '-') {
		filter_flag = true;
		strcpy(filter_string, argv[argc-1]);
		regcomp(&regex, filter_string, 0);
	}

	/*
	 * Set the range to display.
	 */
	std::vector<Stat>::iterator start, end;
	int redundant = 0;

	if (tcp_flag && udp_flag) {
		start = table.begin();
		end = table.end();
		printf("List of TCP connections:\n");
	}
	else if (tcp_flag) {
		start = table.begin();
		for (end = table.begin(); end != table.end() && end->protocol[0] == 't'; end++);
		printf("List of TCP connections:\n");
	}
	else {
		end = table.end();
		for (start = table.begin(); start != table.end() && start->protocol[0] == 't'; start++);
		printf("List of UDP connections:\n");
	}

	printf("Proto Local Address			  Foreign Address		  PID/Program name and arguments\n");
	for ( ; start != end; start++) {
		if (filter_flag && regexec(&regex, start->program, 0, NULL, 0) == REG_NOMATCH/*strstr(start->program, filter_string) == NULL*/)
			continue;

		if (display_flag && tcp_flag && start->protocol[0] == 'u') {
			printf("\nList of UDP connections:\n");
			display_flag = false;
		}

		/*
		 * Protocol
		 */
		printf("%-6s", start->protocol);
		/*
		 * Local address and port
		 */
		printf("%s:", start->local_address);
		if (start->local_port)
			printf("%d", start->local_port);
		else
			printf("*");
		redundant = 23 - strlen(start->local_address) - numlen(start->local_port);
		for (int i = 0; i < redundant; i++)
			printf(" ");
		/*
		 * Foreign address and port
		 */
		printf("%s:", start->foreign_address);
		if (start->foreign_port)
			printf("%d", start->foreign_port);
		else
			printf("*");
		redundant = 23 - strlen(start->foreign_address) - numlen(start->foreign_port);
		for (int i = 0; i < redundant; i++)
			printf(" ");
		/*
		 * Pid
		 */
		if (start->pid == 0) {
			printf("-\n");
			continue;
		}
		else
			printf("%d/", start->pid);
		/*
		 * Program and arguments
		 */
		 /*	if (strlen(start->program) > 13)
			 for (int i = 0; i < 13; i++)
				 printf("%c", start->program[i]);
		else*/
			printf("%s", start->program);
		printf("\n");
	}
}

/*
 * Find the pid and program, then store them in stat.
 */
void store_pid_and_program(const char *descriptor_path, Stat &stat) {
	const char *pid_start_ptr, *pid_end_ptr;
	char cmdline_path[LEN] = {0};
	char pid[N];
	FILE *cmdline_file_ptr;

	/*
	 * Get the pid.
	 */
	pid_start_ptr = strchr(descriptor_path + 1, '/') + 1;
	pid_end_ptr = strchr(pid_start_ptr, '/');
	strncpy(pid, pid_start_ptr, pid_end_ptr - pid_start_ptr);
	stat.pid = atoi(pid);

	/*
	 * Get the program and arguments.
	 */
	cat_path(cmdline_path, proc_path, pid);
	cat_path(cmdline_path, cmdline_path, "cmdline");

	cmdline_file_ptr = fopen(cmdline_path, "r");
	if (cmdline_file_ptr == NULL)
		return;

	fgets(stat.program, LEN, cmdline_file_ptr);
	if (stat.program[0] == '/')
		strcpy(stat.program, strrchr(stat.program, '/') + 1);

	fclose(cmdline_file_ptr);
}

/*
 * Look for the descriptor which has the same inode as someone in table.
 */
void find_inode(const char *descriptor_path) {
	char link[LEN] = {0};
	char link_inode[N] = {0};
	const char *inode_start_ptr, *inode_end_ptr;

	if (readlink(descriptor_path, link, LEN) == -1)
		return;

	if (strstr(link, "socket") == NULL)
		return;

	/*
	 * Get the inode.
	 */
	inode_start_ptr = strchr(link, '[') + 1;
	inode_end_ptr = strchr(link, ']');
	strncpy(link_inode, inode_start_ptr, inode_end_ptr - inode_start_ptr);

	for (auto &stat : table)
		if (atoi(link_inode) == stat.inode) {
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
	dir = opendir(fd_path);
	if (dir == NULL)
		return;

	while ((files = readdir(dir)) != NULL) {
		if (is_dot(files->d_name))
			continue;

		char descriptor_path[LEN] = {0};

		cat_path(descriptor_path, fd_path, files->d_name);
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
		traverse_descriptors(pid_path, table);
	}
	closedir(dir);
}

/*
 * Convert the ip address to a readable format.
 */
void convert_ip_address() {
	struct in_addr ipv4;
	struct in6_addr ipv6;

	for (std::vector<Stat>::iterator vi = table.begin(); vi != table.end(); vi++) {
		if (strlen(vi->protocol) == 3) {
			sscanf(vi->local_address, "%x", &(ipv4.s_addr));
			inet_ntop(AF_INET, &(ipv4.s_addr), vi->local_address, INET_ADDRSTRLEN);
			sscanf(vi->foreign_address, "%x", &(ipv4.s_addr));
			inet_ntop(AF_INET, &(ipv4.s_addr), vi->foreign_address, INET_ADDRSTRLEN);
		}
		else {
			sscanf(vi->local_address + 6, "%2" SCNu8, &(ipv6.s6_addr[0]));
			sscanf(vi->local_address + 4, "%2" SCNu8, &(ipv6.s6_addr[1]));
			sscanf(vi->local_address + 2, "%2" SCNu8, &(ipv6.s6_addr[2]));
			sscanf(vi->local_address + 0, "%2" SCNu8, &(ipv6.s6_addr[3]));
			sscanf(vi->local_address + 14, "%2" SCNu8, &(ipv6.s6_addr[4]));
			sscanf(vi->local_address + 12, "%2" SCNu8, &(ipv6.s6_addr[5]));
			sscanf(vi->local_address + 10, "%2" SCNu8, &(ipv6.s6_addr[6]));
			sscanf(vi->local_address + 8, "%2" SCNu8, &(ipv6.s6_addr[7]));
			sscanf(vi->local_address + 22, "%2" SCNu8, &(ipv6.s6_addr[8]));
			sscanf(vi->local_address + 20, "%2" SCNu8, &(ipv6.s6_addr[9]));
			sscanf(vi->local_address + 18, "%2" SCNu8, &(ipv6.s6_addr[10]));
			sscanf(vi->local_address + 16, "%2" SCNu8, &(ipv6.s6_addr[11]));
			sscanf(vi->local_address + 30, "%2" SCNu8, &(ipv6.s6_addr[12]));
			sscanf(vi->local_address + 28, "%2" SCNu8, &(ipv6.s6_addr[13]));
			sscanf(vi->local_address + 26, "%2" SCNu8, &(ipv6.s6_addr[14]));
			sscanf(vi->local_address + 24, "%2" SCNu8, &(ipv6.s6_addr[15]));
			inet_ntop(AF_INET6, &(ipv6.s6_addr), vi->local_address, INET6_ADDRSTRLEN);
			sscanf(vi->foreign_address + 6, "%2" SCNu8, &(ipv6.s6_addr[0]));
			sscanf(vi->foreign_address + 4, "%2" SCNu8, &(ipv6.s6_addr[1]));
			sscanf(vi->foreign_address + 2, "%2" SCNu8, &(ipv6.s6_addr[2]));
			sscanf(vi->foreign_address + 0, "%2" SCNu8, &(ipv6.s6_addr[3]));
			sscanf(vi->foreign_address + 14, "%2" SCNu8, &(ipv6.s6_addr[4]));
			sscanf(vi->foreign_address + 12, "%2" SCNu8, &(ipv6.s6_addr[5]));
			sscanf(vi->foreign_address + 10, "%2" SCNu8, &(ipv6.s6_addr[6]));
			sscanf(vi->foreign_address + 8, "%2" SCNu8, &(ipv6.s6_addr[7]));
			sscanf(vi->foreign_address + 22, "%2" SCNu8, &(ipv6.s6_addr[8]));
			sscanf(vi->foreign_address + 20, "%2" SCNu8, &(ipv6.s6_addr[9]));
			sscanf(vi->foreign_address + 18, "%2" SCNu8, &(ipv6.s6_addr[10]));
			sscanf(vi->foreign_address + 16, "%2" SCNu8, &(ipv6.s6_addr[11]));
			sscanf(vi->foreign_address + 30, "%2" SCNu8, &(ipv6.s6_addr[12]));
			sscanf(vi->foreign_address + 28, "%2" SCNu8, &(ipv6.s6_addr[13]));
			sscanf(vi->foreign_address + 26, "%2" SCNu8, &(ipv6.s6_addr[14]));
			sscanf(vi->foreign_address + 24, "%2" SCNu8, &(ipv6.s6_addr[15]));
			inet_ntop(AF_INET6, &(ipv6.s6_addr), vi->foreign_address, INET6_ADDRSTRLEN);
		}
	}
}

/*
 * Get the information in /proc/net/tcp, tcp6, udp, udp6.
 */
void get_stat(const char *path, const char *protocol) {
	FILE *file_ptr;
	Stat stat = {0};
	char buf[LEN] = {0};
	char *str_ptr;
	int redundant = 0;

	file_ptr = fopen(path, "r");
	if (file_ptr == NULL) {
		fprintf(stderr, "Unable to open the file: %s\n", path);
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
		str_ptr = strchr(stat.local_address, ':') + 1;
		sscanf(str_ptr, "%x", &stat.local_port);
		*(str_ptr - 1) = 0;
		fscanf(file_ptr, "%s", stat.foreign_address);
		str_ptr = strchr(stat.foreign_address, ':') + 1;
		sscanf(str_ptr, "%x", &stat.foreign_port);
		*(str_ptr - 1) = 0;
		for (int i = 1; i < 7; i++)
			fscanf(file_ptr, "%s", buf);
		fscanf(file_ptr, "%d", &stat.inode);
		for (int i = 1; i < redundant; i++)
			fscanf(file_ptr, "%s", buf);
		table.emplace_back(stat);
	}
	fclose(file_ptr);
}

void run(int &argc, char **argv) {
	//printf("========== tcp ==========\n");
	get_stat(tcp_file_path, "tcp");
	//printf("========== tcp6 ==========\n");
	get_stat(tcp6_file_path, "tcp6");
	//printf("========== udp ==========\n");
	get_stat(udp_file_path, "udp");
	//printf("========== udp6 ==========\n");
	get_stat(udp6_file_path, "udp6");

	convert_ip_address();
	traverse_pid_folders();
	display_result(argc, argv);
}

int main(int argc, char **argv) {
	run(argc, argv);

	return 0;
}
