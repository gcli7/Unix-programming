#include "libmini.h"

long errno;

#define WRAPPER_RETval(type)    errno = 0; if(ret < 0) { errno = -ret; return -1; } return ((type) ret);
#define WRAPPER_RETptr(type)    errno = 0; if(ret < 0) { errno = -ret; return NULL; } return ((type) ret);

ssize_t write(int fd, const void *buf, size_t count) {
    long ret = sys_write(fd, buf, count);
    WRAPPER_RETval(ssize_t);
}

int pause() {
    long ret = sys_pause();
    WRAPPER_RETval(int);
}

int nanosleep(struct timespec *rqtp, struct timespec *rmtp) {
    long ret = nanosleep(rqtp, rmtp);
    WRAPPER_RETval(int);
}

void exit(int error_code) {
    sys_exit(error_code);
    /* never returns? */
}

/* Extended code */
int alarm(unsigned int seconds) {
    long ret = sys_alarm(seconds);
    WRAPPER_RETval(int);
}

void sigemptyset(sigset_t *set) {
    for (int i = 0; i < _NSIG_WORDS; i++)
        set->sig[i] = 0;
}

void sigaddset(sigset_t *set, int signum) {
    set->sig[0] |= 1UL << (signum - 1);
}

int sigismember(sigset_t *set, int signum) {
    return set->sig[0] & (1UL << (signum - 1));
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
    long ret = sys_sigprocmask(how, set, oldset, sizeof(sigset_t));
    WRAPPER_RETval(int);
}

int sigpending(sigset_t *set) {
    long ret = sys_sigpending(set, sizeof(sigset_t));
    WRAPPER_RETval(int);
}

sighandler_t signal(int signum, sighandler_t handler) {
    struct sigaction act, oldact;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = handler;
    sigaction(signum, &act, &oldact);
    return handler;
}

int sigaction(int signum, struct sigaction *act, struct sigaction *oldact) {
    act->sa_flags |= SA_RESTORER;
    act->sa_restorer = (void (*)(void))&sigreturn;
    long ret = sys_sigaction(signum, act, oldact, sizeof(sigset_t));
    WRAPPER_RETval(int);
}
/* End of extended code */

size_t strlen(const char *s) {
    size_t count = 0;
    while(*s++) count++;
    return count;
}

#define PERRMSG_MIN 0
#define PERRMSG_MAX 34

static const char *errmsg[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "I/O error",
    "No such device or address",
    "Argument list too long",
    "Exec format error",
    "Bad file number",
    "No child processes",
    "Try again",
    "Out of memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "Cross-device link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "File table overflow",
    "Too many open files",
    "Not a typewriter",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Math argument out of domain of func",
    "Math result not representable"
};

void perror(const char *prefix) {
    const char *unknown = "Unknown";
    long backup = errno;
    if(prefix) {
        write(2, prefix, strlen(prefix));
        write(2, ": ", 2);
    }
    if(errno < PERRMSG_MIN || errno > PERRMSG_MAX) write(2, unknown, strlen(unknown));
    else write(2, errmsg[backup], strlen(errmsg[backup]));
    write(2, "\n", 1);
    return;
}
