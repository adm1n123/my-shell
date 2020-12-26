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

//--------------------------- CASE 3 --------

void listFiles() {
	char *arguments[] = {"ls", NULL};
	int status = execvp("ls", arguments);
	if(status == -1) {
		printError();
		exit(1);
	}
	exit(0);
}

void main(int argc, char *args[]) {
	listFiles();
	return;
}