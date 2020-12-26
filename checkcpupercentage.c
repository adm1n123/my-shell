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

// -------------------  CASE 1 --------------------------------------------------------------------------------------------------------------------------
void checkcpupercentage(char *pid) {

	// reading user process time
	char path[50];
	strcpy(path, "/proc/");
	strcat(path, pid);
	strcat(path, "/stat");
	int fd = open(path, O_RDONLY);
	if(fd == -1) {
		printError(); exit(1);
	}
	char data[300], userModeTime[20], systemModeTime[20];
	long long int userModeStart = 0, userModeEnd = 0, systemModeStart = 0, systemModeEnd = 0;
	int bytes = read(fd, data, 300); // read appedn '\0' at the end dont worry
	getWord(' ', data, userModeTime, 14);
	getWord(' ', data, systemModeTime, 15);
	userModeStart = parseLLInt(userModeTime);
	systemModeStart = parseLLInt(systemModeTime);
	
	// reading CPU time
	int fdCPU = open("/proc/stat", O_RDONLY);
	if(fdCPU == -1) {
		printError(); exit(1);
	}
	char cpuData[300], **timeRow, **timeArray;
	timeRow = getTwoDarray(1, 300); // first row of CPU data
	timeArray = getTwoDarray(50, 25);	// splitted first row of CPU data
	read(fdCPU, cpuData, 300);
	splitString(cpuData, timeRow, 1, '\n');
	
	long long int cputStartTime = 0, cpuEndTime = 0;
	splitString(timeRow[0], timeArray, 50, ' ');

	for(int i = 1; i < 50 && timeArray[i] != NULL; i++) { // loop for either 50 row or end of 2D array
		if(isInteger(timeArray[i]) == 1) 
			cputStartTime += parseLLInt(timeArray[i]);
	}
	close(fd); // close the file other wise it will resume reading from last location
	close(fdCPU);

	sleep(1);
	
	// reading user again
	fd = open(path, O_RDONLY);
	if(fd == -1) {
		printError(); exit(1);
	}

	bytes = read(fd, data, 300);
	getWord(' ', data, userModeTime, 14);
	getWord(' ', data, systemModeTime, 15);
	userModeEnd = parseLLInt(userModeTime);
	systemModeEnd = parseLLInt(systemModeTime);
	
	// reading system again
	fdCPU = open("/proc/stat", O_RDONLY);
	if(fdCPU == -1) {
		printError(); exit(1);
	}
	read(fdCPU, cpuData, 300);
	splitString(cpuData, timeRow, 1, '\n');
	splitString(timeRow[0], timeArray, 50, ' ');
	for(int i = 1; i < 50 && timeArray[i] != NULL; i++) {
		
		if(isInteger(timeArray[i]) == 1) 
			cpuEndTime += parseLLInt(timeArray[i]);
	}
	// final calculation
	long double userModePerc, systemModePerc;
	userModePerc = (userModeEnd - userModeStart) * 100;
	userModePerc /= cpuEndTime - cputStartTime;
	systemModePerc = (systemModeEnd - systemModeStart) * 100;
	systemModePerc /= cpuEndTime - cputStartTime;
	printf("user mode cpu percentage: %0.2Lf%\n", userModePerc);
	printf("system mode cpu percentage: %0.2Lf%\n", systemModePerc);
	
	close(fd);
	close(fdCPU);
	exit(0);
}

void main(int argc, char *args[]) {
	checkcpupercentage(args[0]);
	return;
}