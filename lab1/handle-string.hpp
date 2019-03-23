#include <cstring>

#define N 99
#define LEN 999

const char *proc_path = "/proc";
const char *tcp_file_path = "/proc/net/tcp";
const char *tcp6_file_path = "/proc/net/tcp6";
const char *udp_file_path = "/proc/net/udp";
const char *udp6_file_path = "/proc/net/udp6";

typedef struct stat {
	int inode;
	char protocol[N];
	char local_address[N];
	int local_port;
	char foreign_address[N];
	int foreign_port;
	int pid;
	char program[LEN];
} Stat;

bool is_dot(const char *name) {
    if (!strcmp(name, ".") || !strcmp(name, ".."))
        return true;
    return false;
}

bool is_not_pid(const char *name) {
    int l = strlen(name);
    for (int i = 0; i < l; i++)
        if (name[i] < '0' || name[i] > '9')
            return true;

    return false;
}

void cat_path(char *ret, const char *a, const char *b) {
	int l = strlen(a);
	strcpy(ret, a);
	if (a[l-1] == '/' && b[0] == '/') {
		ret[l-1] = 0;
	}
	else if (a[l-1] != '/' && b[0] != '/') {
		ret[l] = '/';
		ret[l+1] = 0;
	}
	strcat(ret, b);
}
