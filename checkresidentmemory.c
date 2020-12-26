#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<signal.h>
#include "Functions.c"

// ---------------------- CASE 2 --------------------------------------------------------------------------------------------------------

void checkresidentmemory(char *pid) {

	char path[50];
	strcpy(path, "/proc/");
	strcat(path, pid);
	strcat(path, "/stat");
	int fd = open(path, O_RDONLY); // testing whether process exit or not
	if(fd == -1) {
		printError(); exit(1);
	}
	close(fd);

	char *arguments[] = {"ps", "-p", pid, "-o", "rss=", NULL}; // -p parameter to denote process id -o to output rss
	int status = execvp("ps", arguments); // 'rss='' instead of 'rss' is to remove the header while printing
	if(status == -1) {
		printError();
		exit(1);
	}
	exit(0);
}

void main(int argc, char *args[]) {
	checkresidentmemory(args[0]);
	return;
}