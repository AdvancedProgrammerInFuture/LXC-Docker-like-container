#include<iostream> 
#include<sys/types.h>
#include<unistd.h> 
#include<sys/wait.h>
#include<sched.h> 	

char* stack_memory();
int jail(void *args);
int run(const char *name);
void setup_root(const char *folder);
void setup_variables();

int main(int argc, char **argv) {
	printf("parent pid %d\n", getpid());
	printf("Hello world\n");
	
	clone(jail, stack_memory(), CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD, 0); // SIGCHLD - flag that tells process to emit a signal when finished, 4 for pass arguments (no need), CLONE_NEWUTS
	printf("Where child\n");
	wait(nullptr); // wait until any child-process terminate, for clone not become zombie-proces(process that finishes, but has entry in process entry point) wait remove entry from table	
	return EXIT_SUCCESS;
	
}


char* stack_memory() {
	const int stack_size = 65536;
	auto *stack = new (std::nothrow) char[stack_size];
	
	if(stack == nullptr) {
		printf("Cannot allocate memory\n");
		exit(EXIT_FAILURE); // exit from all threads and end of processes
	}
	
	return stack+stack_size; // because stack grows backward 
	 		
}

int jail(void *args) {
	printf("child pid = %d\n", getpid());
	
	setup_variables();
	setup_root("./root");
	mount("proc", "/proc", "proc");

	run("/bin/sh");

	umount("/proc"); // release that not use;
	return EXIT_SUCCESS;
				
}

int run(const char *name) {
	char *_args[] = {(char *)name, (char *)0};
	execvp(name, _args); // execvp - replace the current process, the child instance of the programm 
}

void setup_root(const char *folder) {
	chroot(folder); // chroot for change the root folder, for process cannot get access for resourses in other folders 
	chdir("/"); // tell process to jump in new folder 
}

void setup_variables() { 
	clearenv(); // clear all environment variables and move that need
	setenv("TERM", "xterm-256color", 0);
  	setenv("PATH", "/bin/:/sbin/:usr/bin:/usr/sbin", 0); // 
}

