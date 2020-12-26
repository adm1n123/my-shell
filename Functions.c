#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include <signal.h>


/*------------------------------------------READING INPUT -------------------------------------------------------------------------*/
char *array[10], *rawInput; // store the command to be executed
char * readline();
void removeSpaces(char *string);
int isSpaceChar(char c);

// take the input remove the spaces and return string in heap.
char * readline() {
	char *input = (char *)malloc(100*sizeof(char));
	rawInput = (char *)malloc(100*sizeof(char));
	int byte = read(0, input, 100);
	if(byte == -1) byte = 1;// so that input is null terminated
	input[byte-1] = '\0';
	strcpy(rawInput, input);
	removeSpaces(input);
	return input;
}
char * readPipe(int n) {
	char *input = (char *)malloc(100*sizeof(char));
	int byte = read(STDIN_FILENO, input, n);
	if(byte == -1) byte = 1;// so that input is null terminated
	input[byte-1] = '\0';
	return input;
}

void trim(char *string) {
	if(string == NULL) return;
	int i=0,j = 0;
	while(isSpaceChar(string[i]) == 1) i++;
	for(; string[i] != '\0'; i++) {
		string[j++] = string[i];
	}
	string[j] = '\0';
}

void  removeSpaces(char *string) {
	if(string == NULL) return;
	int i = 0;
	trim(string);
	if(strlen(string) == 0) return;	
	string[i++] = string[0];
	for(int j = 1; string[j] != '\0'; j++) {
		if(isSpaceChar(string[i-1]) == 1 && isSpaceChar(string[j]) == 1) {
			continue;
		}
		string[i++] = string[j];
	}
	if(i > 0 && isSpaceChar(string[i-1]) == 1) i--;
	string[i] = '\0';
}

int isSpaceChar(char c) {
	if(c == ' ' || c == '\t') {
		return 1;
	} else {
		return 0;
	}
}

/*------------------------------------------------------ UTILITY ----------------------------------------------------------*/
void  split(char *input) {
	if(input == NULL) {
		for(int i = 0; i < 10; i++) array[i] = NULL; return;
	}
	int i, j;
	for(i = 0; i < 10; i++) {
		array[i] = (char *)malloc(50*sizeof(char));
	}
	i = 0; j = 0;
	for(int k = 0; input[k] != '\0'; k++) {
		if(isSpaceChar(input[k]) == 1) {
			array[i][j] = '\0';
			i++; j = 0; continue; 
		}
		array[i][j++] = input[k];
	}
	array[i][j] = '\0';
	for(i++;i < 10; i++) {
		array[i] = NULL;
	}
}	

int isExit(char *input) {
	if(input == NULL) return 0;
	if(strcmp(input,"exit") == 0) {
		return 1;
	} else {
		return 0;
	}
}

int isInteger(char *input) {
	if(input == NULL) return 0;
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] < '0' || input[i] > '9') return 0;
	}
	return 1;
}
int parseInt(char *input) {
	if(input == NULL) return 0;
	int value = 0;
	for(int i = 0; i < strlen(input); i++) {
		value = value * 10 + input[i] - '0';
	}
	return value;
}
long long int parseLLInt(char *input) {
	if(input == NULL) return 0;
	long long int value = 0;
	for(int i = 0; i < strlen(input); i++) {
		value = value * 10 + input[i] - '0';
	}
	return value;
}
int getCommandCount(char *input) {
	if(input == NULL) return 0;
	int count = 0;
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == ';') {
			count++;
		}
	}
	return count+1;
}
void printError() {
	printf("Illegal command or arguments\n");
}

// return the index of first char; if char is not present return -1;
int getCharIndex(char *input, char c) { 
	if(input == NULL) return -1;
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == c) return i;
	}
	return -1;
}
char ** getTwoDarray(int row, int col) {
	char ** array = (char **)malloc(row * sizeof(char *));
	for(int i = 0; i < row; i++) {
		array[i] = (char *)malloc(col * sizeof(char));
	}
	return array;
}
char * getArray(int size) {
	char * array = (char *)malloc(size * sizeof(char));
	return array;
}

// -------------------  CASE 1 --------------------------------------------------------------------------------------------------------------------------

// row is max number of row you want form input after splitting
void splitString(char *input, char **output, int row, char splitChar) { 
	int i = 0, j = 0;
	for(int k = 0; i < row && input[k] != '\0'; k++) {
		if(input[k] == splitChar) {
			output[i][j] = '\0';
			i++; j = 0; continue; 
		}
		output[i][j++] = input[k];
	}
	if(i < row) {
		output[i][j] = '\0';
		for(i++;i < row; i++) {
		output[i] = NULL;
		}
	}
}

// return the word when splitted by splitChar; word number starts from 1.
void getWord(char splitChar, char input[], char output[], int wordNumber) { 
	wordNumber--;
	int i = 0, j = 0;
	for(; i < strlen(input); i++) {
		if(wordNumber == 0) {
			for(; i < strlen(input) && input[i] != splitChar; i++) {
				output[j++] = input[i];
			}
			output[j] = '\0';
			break;
		} else if(input[i] == splitChar) {
			wordNumber--;
		}
	}
}

void checkCPUPercentage(char *input, char *args[]) {
	split(input);
	if(array[1] == NULL) { // no pid in arguments
		int fd = open(args[0], O_RDONLY); // args[0] has input file
		if(fd == -1) {
			printError(); exit(1);
		}
		array[1] = getArray(20);
		read(fd, array[1], 20);
		array[1] = strtok(array[1], " \n");
		close(fd);
	}
	if(args[1] != NULL) { // writing to output file
		int fd;
		if(args[2] == NULL) // append is null
			fd = open(args[1], O_CREAT | O_WRONLY | O_TRUNC, 0777); // 0777 is in octal
		else
			fd = open(args[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, 1);
		close(fd);
	}
	char *arguments[] = {array[1], NULL};
	int status = execve("checkcpupercentage", arguments, NULL);
	printError();
	exit(1);
}

// ---------------------- CASE 2 --------------------------------------------------------------------------------------------------------

void checkResidentMemory(char *input, char *args[]) {
	split(input);

	if(array[1] == NULL) { // no pid in arguments
		int fd = open(args[0], O_RDONLY); // args[0] has input file
		if(fd == -1) {
			printError(); exit(1);
		}
		array[1] = getArray(20);
		read(fd, array[1], 20);
		array[1] = strtok(array[1], " \n");
		close(fd);
	}
	if(args[1] != NULL) { // writing to output file
		int fd;
		if(args[2] == NULL) // append is null
			fd = open(args[1], O_CREAT | O_WRONLY | O_TRUNC, 0777); // 0777 is in octal
		else
			fd = open(args[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, 1);
		close(fd);
	}
	char *arguments[] = {array[1], NULL};
	int status = execve("checkresidentmemory", arguments, NULL);
	printError();
	exit(1);
}

//--------------------------- CASE 3 -------------------------------------------------------------------------------------------------------

void listfiles(char *input, char *args[]) {
	split(input);

	if(args[1] != NULL) { // writing to output file provide
		int fd;
		if(args[2] == NULL) // append is null
			fd = open(args[1], O_CREAT | O_WRONLY | O_TRUNC, 0777); // 0777 is in octal for full access for all users
		else
			fd = open(args[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, 1);
		close(fd);
	} else {
		int fd = open("files.txt", O_WRONLY | O_TRUNC | O_CREAT, 0777); // O_WRONLY will write the data from begining of file
		if(fd == -1) {
			printError();
			exit(1);
		}
		dup2(fd, 1); // 1 is the file descriptor of STDOUT hence now STDOUT will be written on fd
		close(fd);
	}
	
	char *arguments[] = {NULL};
	int status = execve("listFiles", arguments, NULL);
	printError();
	exit(1);
}

//--------------------------------CASE 4 ------------------------------------------------------------------------------------------------------
int isFileName(char *input) {
	if(input == NULL) return 0;
	if(strcmp(".", input) == 0) return 0;
	for(int i = 0; i < strlen(input); i++) if(input[i] == '/') return 0;
	return 1;
}
void sortfile(char *input, char *args[]) {  ///////////// handle the case when file name has spaces and it is given like "ab cd"
	split(input);
	if(array[1] == NULL) { // no input file in arguments
		array[1] = args[0];
	}
	if(args[1] != NULL) { // writing to output file
		int fd;
		if(args[2] == NULL) // append is null
			fd = open(args[1], O_CREAT | O_WRONLY | O_TRUNC, 0777); // 0777 is in octal
		else
			fd = open(args[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, 1);
		close(fd);
	}

	int fd = open(array[1], O_RDONLY);
	if(fd == -1) {
		printError();
		exit(1);
	}
	dup2(fd, 0); // mapping the STDIN to fd so that if process take input from STDIN it can take from file
	// file discriptor number for STDIN is 0, and for STDOUT is 1.
	// now both fd, and STDIN points to file.
	close(fd);// we can close this fd because 0 is also fd of file so no problem
	// but if we do close(fd) and close(0) then file will be close because all the fd of files are closed
	char *arguments[] = {NULL};
	int status = execve("sortFile", arguments, NULL); // sort executable will do exit() hence program will not execute after this statement
	// if execve fails then only porgram will resume from this line
	printError();
	exit(1);
}

//------------------------- CASE 8a ------------------------------------------------------------------------------------------------------
char * isAppend(char *input) {
	if(input == NULL) return NULL;
	char * append = (char *)malloc(3 * sizeof(char));
	append[0] = append[1] = '>'; append[2] = '\0';
	for(int i = 1; i < strlen(input); i++) {
		if(input[i-1] == '>' && input[i] == '>') return append;
	}
	return NULL;
}
char * getProgramName(char *input1) {
	if(input1 == NULL) return NULL;
	char * input = getArray(100);
	strcpy(input, input1);
	char *name = strtok(input, " <>");
	if(strcmp(name, "checkcpupercentage") == 0) return name;
	if(strcmp(name, "checkresidentmemory") == 0) return name;
	if(strcmp(name, "listFiles") == 0) return name;
	if(strcmp(name, "sortFile") == 0) return name;
	if(strcmp(name, "executeCommands") == 0) return name;
	return NULL;
}
char * getInputFile(char *input1) {
	if(input1 == NULL) return NULL;
	char * input = getArray(100);
	strcpy(input, input1);
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == '<') {
			char *file = strtok(input, "<");
			file = strtok(NULL, "<");
			removeSpaces(file); // remove preceding spaces
			file = strtok(file, " >");
			return file;
		}
	}
	return NULL;
}
char * getOutputFile(char *input1) {
	if(input1 == NULL) return NULL;
	char *input = getArray(100);
	char *append = isAppend(input);
	strcpy(input, input1);
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == '>') {
			char *file = strtok(input, ">");
			file = strtok(NULL, ">");
			if(append != NULL) {
				file = strtok(NULL, ">");
			}
			removeSpaces(file);
			file = strtok(file, " ");
			return file;
		}
	}
	return NULL;
}
char * getPattern(char *input1) {
	if(input1 == NULL) return NULL;
	char *input = getArray(100);
	strcpy(input, input1);
	char *pattern = strtok(input, "\"'");
	pattern = strtok(NULL, "\"'");
	if(pattern == NULL) {
		removeSpaces(input);
		pattern = strtok(input, " <>");
		pattern = strtok(NULL, " <>");
	}
	return pattern;

}
void cat(char *input, char*args[]) {
	split(input);
	if(array[1] == NULL) array[1] = args[0]; // no filename in arguments
	if(array[1] != NULL) { // args[0] has input file
		int fd = open(array[1], O_RDONLY); 
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if(args[1] != NULL) { // writing to output file
		int fd;
		if(args[2] == NULL) // append is null
			fd = open(args[1], O_CREAT | O_WRONLY | O_TRUNC, 0777); // 0777 is in octal
		else
			fd = open(args[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	char * arguments[] = {"cat", NULL};
	int status = execvp("cat", arguments);
	printError();
	exit(1);
}
void grep(char*args[]) {
	if(args[3] == NULL) { // no pattern
		printError(); exit(1);
	}
	if(args[0] != NULL) { // args[0] has input file
		int fd = open(args[0], O_RDONLY); 
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	if(args[1] != NULL) { // writing to output file
		int fd;
		if(args[2] == NULL) // append is null
			fd = open(args[1], O_CREAT | O_WRONLY | O_TRUNC, 0777); // 0777 is in octal
		else
			fd = open(args[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
		if(fd == -1) {
			printError(); exit(1);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
	}
	char * arguments[] = {"grep",args[3], NULL};
	int status = execvp("grep", arguments);
	printError();
	exit(1);
}
//--------------------------------------------- CASE 8b ---------------------------------------------------------------------------------
