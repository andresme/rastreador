#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>

#ifdef __i386__
	#include <linux/user.h>
#else
	#include <sys/reg.h> 
#endif


#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdlib.h>

typedef enum { false, true } bool;

#define IS_STRING_EQUAL(a, b) strcmp(a,b) == 0

#define TRACEABLE_CHILD 0
#define VERBOSE "-v"
#define VERBOSE_STEP "-V"

#ifdef __i386__
	#define REG_SPACE 4 * ORIG_EAX
#else
	#define REG_SPACE 8 * ORIG_RAX 
#endif


void executeChild(char *cmd, char **argv);
void traceChild(pid_t child, bool verbose_enabled, bool step_enabled);
void printVerbose(char* msg, long reg, bool verbose_enabled);
void initializeArray(void);
void printResults(void);

#ifdef __i386__
		int accumulative_syscalls[338];
		char* names[338];
#else
		int accumulative_syscalls[323];
		char* names[323];
#endif




