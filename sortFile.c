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

//--------------------------------CASE 4 ------------------------------------------------------------------------------------------------------

void sortFile() {  ///////////// handle the case when file name has spaces and it is given like "ab cd"
	char *arguments[] = {"sort", NULL};
	int status = execvp("sort", arguments); // sort executable will do exit() hence program will not execute after this statement
	if(status == -1) { // if execvp fails then only porgram will resume from this line
		printError();
		exit(1);
	}
	exit(0);
}

void main(int argc, char *args[]) {
	sortFile();
	return;
}