#ifndef __LIBMINI_H__
#define __LIBMINI_H__       /* avoid reentrant */

typedef long long size_t;
typedef long long ssize_t;
/* Extended code */
#define _NSIG       64
#define _NSIG_BPW   64
#define _NSIG_WORDS (_NSIG / _NSIG_BPW)

#define SIG_DFL ((sighandler_t)0) /* default signal handling */
#define SIG_IGN ((sighandler_t)1) /* ignore signal */
#define SIG_ERR ((sighandler_t)-1)    /* error return from signal */

typedef void (*sighandler_t)(int);

typedef struct {
    unsigned long sig[_NSIG_WORDS];
} sigset_t;

struct sigaction {
    /*
    sighandler_t sa_handler;
    void (*sa_sigaction)(int, void *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
    */
    sighandler_t sa_handler;
    unsigned long sa_flags;
    void (*sa_restorer)(void);
    sigset_t sa_mask;
};
/* End of extended code */

extern long errno;

#define NULL ((void*) 0)

/* from /usr/include/x86_64-linux-gnu/asm/signal.h */
#define SIGHUP       1
#define SIGINT       2
#define SIGQUIT      3
#define SIGILL       4
#define SIGTRAP      5
#define SIGABRT      6
#define SIGIOT       6
#define SIGBUS       7
#define SIGFPE       8
#define SIGKILL      9
#define SIGUSR1     10
#define SIGSEGV     11
#define SIGUSR2     12
#define SIGPIPE     13
#define SIGALRM     14
#define SIGTERM     15
#define SIGSTKFLT   16
#define SIGCHLD     17
#define SIGCONT     18
#define SIGSTOP     19
#define SIGTSTP     20
#define SIGTTIN     21
#define SIGTTOU     22
#define SIGURG      23
#define SIGXCPU     24
#define SIGXFSZ     25
#define SIGVTALRM   26
#define SIGPROF     27
#define SIGWINCH    28
#define SIGIO       29
#define SIGPOLL     SIGIO

/* from /usr/include/x86_64-linux-gnu/bits/sigaction.h */
#define SA_NOCLDSTOP  1      /* Don't send SIGCHLD when children stop.  */
#define SA_NOCLDWAIT  2      /* Don't create zombie on child death.  */
#define SA_SIGINFO    4      /* Invoke signal-catching function with
                                three arguments instead of one.  */
#define SA_RESTORER  0x04000000
#define SA_ONSTACK   0x08000000 /* Use signal stack by using `sa_restorer'. */
#define SA_RESTART   0x10000000 /* Restart syscall on signal return.  */
#define SA_INTERRUPT 0x20000000 /* Historical no-op.  */
#define SA_NODEFER   0x40000000 /* Don't automatically block the signal when
                                    its handler is being executed.  */
#define SA_RESETHAND 0x80000000 /* Reset to SIG_DFL on entry to handler.  */

#define SIG_BLOCK     0      /* Block signals.  */
#define SIG_UNBLOCK   1      /* Unblock signals.  */
#define SIG_SETMASK   2      /* Set the set of blocked signals.  */

struct timespec {
    long    tv_sec;     /* seconds */
    long    tv_nsec;    /* nanoseconds */
};

struct timeval {
    long    tv_sec;     /* seconds */
    long    tv_usec;    /* microseconds */
};

struct timezone {
    int tz_minuteswest; /* minutes west of Greenwich */
    int tz_dsttime;     /* type of DST correction */
};

/* system calls */
long sys_write(int fd, const void *buf, size_t count);
long sys_pause();
long sys_nanosleep(struct timespec *rqtp, struct timespec *rmtp);
long sys_exit(int error_code) __attribute__ ((noreturn));
/* Extended code */
long sys_alarm(unsigned int seconds);
long sys_sigprocmask(int how, const sigset_t *set, sigset_t *oldset, size_t sigsetsize);
long sys_sigpending(sigset_t *set, size_t sigsetsize);
long sys_sigaction(int signum, struct sigaction *act, struct sigaction *oldact, size_t sigsetsize);
long sigreturn(unsigned long unused);
/* End of extended code */

/* wrappers */
ssize_t write(int fd, const void *buf, size_t count);
int pause();
int nanosleep(struct timespec *rqtp, struct timespec *rmtp);
void exit(int error_code);
/* Extended code */
int alarm(unsigned int seconds);
void sigemptyset(sigset_t *set);
void sigaddset(sigset_t *set, int signum);
int sigismember(sigset_t *set, int signum);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigpending(sigset_t *set);
sighandler_t signal(int signum, sighandler_t handler);
int sigaction(int signum, struct sigaction *act, struct sigaction *oldact);
/* End of extended code */

size_t strlen(const char *s);
void perror(const char *prefix);
unsigned int sleep(unsigned int s);

#endif  /* __LIBMINI_H__ */
