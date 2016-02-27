#include "main.h"

int main(int argc, char **argv) {
    pid_t child;
    long orig_eax, eax;
    bool verbose_enabled;
    bool step_enabled;
    
    initializeArray();
	if(IS_STRING_EQUAL(argv[1], VERBOSE)){
		verbose_enabled = true;
		step_enabled = false;
	} else if(IS_STRING_EQUAL(argv[1], VERBOSE_STEP)){
		verbose_enabled = true;
		step_enabled = true;
	} else {
		verbose_enabled = false;
		step_enabled = false;
	}

	child = fork();
    if(child == TRACEABLE_CHILD) {
		
		if(IS_STRING_EQUAL(argv[1], VERBOSE) || IS_STRING_EQUAL(argv[1], VERBOSE_STEP)){
			
			char **args_copy = malloc(argc * sizeof(char*));
			
			for(int i = 2; i < argc; i++){
				args_copy[i-2] = malloc(strlen(argv[i])+1);
				strcpy(args_copy[i-2], argv[i]);
			}
			args_copy[argc-1] = 0;
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			execve(args_copy[0], &args_copy[0], NULL);
		} else {
			
			char **args_copy = malloc(argc * sizeof(char*));
			
			for(int i = 1; i < argc; i++){
				args_copy[i-1] = malloc(strlen(argv[i])+1);
				strcpy(args_copy[i-1], argv[i]);
			}
			args_copy[argc-1] = 0;
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			execve(args_copy[0], &args_copy[0], NULL);
		}
		
    }
    else {
		traceChild(child, verbose_enabled, step_enabled);
		printResults();
    }
    return 0;
}

void executeChild(char *cmd, char **argv){
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);
	execve(cmd, argv, NULL);
}

void traceChild(pid_t child, bool verbose_enabled, bool step_enabled){
	bool keep_running = true;
	bool is_in_syscall = true;
	bool first_execve = false;
	long orig_rax;
	struct user_regs_struct regs;
	while(1){
		wait(NULL);
		orig_rax = ptrace(PTRACE_PEEKUSER,
                          child, REG_SPACE,
                          NULL);
        if(orig_rax == -1){
			printVerbose("Exit system call executed\n", 0l, verbose_enabled);
			break;
		} else {
			if(is_in_syscall && first_execve){
				printVerbose("The child made a system call %s", orig_rax, verbose_enabled);
				is_in_syscall = false;
				accumulative_syscalls[orig_rax]++;
			} else if(!is_in_syscall && first_execve){
				printVerbose("Exitted call %s", orig_rax, verbose_enabled);
				is_in_syscall = true;
			} else {
				//just ignore if it's the child calling the process to trace
				first_execve = true;
			}
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
			if(step_enabled){
				printf("Press Enter to continue");
				getchar();
			}
		}
	}
}

void printVerbose(char* msg, long reg, bool verbose_enabled){
	if(verbose_enabled){
		printf(msg, names[reg]);
	}
}

void initializeArray(void){
	#ifdef __i386__
		FILE *file = fopen("syscalls32", "r");
		int max = 338;
	#else
		FILE *file = fopen("syscalls", "r");
		int max = 323;
	#endif
	
	char buff[256];
	rewind(file);
	int i = 0;
	while(fgets(buff, 256, file) != NULL && i < max) {
        names[i] = malloc(strlen(buff) + 1);
        strcpy(names[i], buff);
        i++;
    }
    fclose(file);
}

void printResults(void){
	#ifdef __i386__
		int max = 338;
	#else
		int max = 323;
	#endif
	
	for(int i = 0; i < max; i++){
		if(accumulative_syscalls[i] > 0){
			printf("| code: %d\t|\tcount: %d\t| name: %s", i, accumulative_syscalls[i], names[i]);
		}
	}
}
