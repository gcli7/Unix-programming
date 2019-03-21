#include <cstring>

#define LEN 256

const char *proc_path = "/proc";
const char *tcp_file_path = "/proc/net/tcp";
const char *udp_file_path = "/proc/net/udp";

unsigned short is_contain_letter(const char *s) {
	int l = strlen(s);
	for (int i = 0; i < l; i++)
		if (s[i] < '0' || s[i] > '9')
			return 1;
	return 0;
}

void cat_path(char *ret, const char *a, const char *b) {
	int l = strlen(a);
	memset(ret, 0, sizeof(ret));
	strcat(ret, a);
	if (a[l-1] == '/' && b[0] == '/')
		ret[l-1] = '\0';
	else if (a[l-1] != '/' && b[0] != '/')
		ret[l] = '/';
	strcat(ret, b);
}
