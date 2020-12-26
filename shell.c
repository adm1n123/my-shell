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


int pipefd[2];


void commander(char *input, int commandType, char *args[]);
int getCommandType(char *input);


//--------------------------------- CASE 6 ---------------------------------------------------------------------------------------------
void executeCommands(char *input, char *args[]) {
	split(input);

	if(array[1] == NULL) { // no input file in arguments
		array[1] = args[0];
	}

	int fd = open(array[1], O_RDONLY);
	if(fd == -1) {
		printError();
		exit(1);
	}
	char *commandsInput, *command, *commandList[500];
	commandsInput = getArray(5000);
	int n=0, bytes = read(fd, commandsInput, 50000);
	command = strtok(commandsInput, "\n");
	
	while(command != NULL) {
		commandList[n++] = command;
		command = strtok(NULL, "\n");
	}
	commandList[n] = NULL;
	char *outputfile = args[1], *append = args[2], *arguments[10] = {NULL};
	if(outputfile != NULL) {
		if(append == NULL) { // if it is not append create file
			int fd = open(outputfile, O_CREAT | O_WRONLY | O_TRUNC, 0777);
			if(fd != -1) {
				arguments[1] = outputfile;
				arguments[2] = ">>"; // we need to append output of all the sequencial commands anyway
			}
			close(fd);
		} else {
			int fd = open(outputfile, O_CREAT | O_WRONLY | O_APPEND, 0777); // 0777 is in octal
			if(fd != -1) {
				arguments[1] = outputfile;
				arguments[2] = ">>";
			}
			close(fd);
		}
	}
	for(int i = 0; i < n; i++) {
		strcpy(rawInput, commandList[i]);
		removeSpaces(commandList[i]);
		commander(commandList[i], -1, arguments);
	}
	exit(0);
}


//------------------------------- CASE 7 ------------------------------------------------------------------------------------------------

void signalHandler(int sigType) {
	if(sigType == SIGINT) {
		printf("the program is interrupted, do you want to exit [Y/N]");
		fflush(stdout);
		char *input = readline();
		if(input != NULL && strcmp(input, "Y") == 0) {
			exit(sigType);
		} else {
			printf("myShell> ");
			fflush(stdout);
		}
	} else if(sigType == SIGTERM) {
		printf("Got SIGTERM-Leaving\n");
		fflush(stdout);
		exit(sigType);
	}
}
//------------------------- CASE 8a ------------------------------------------------------------------------------------------------------

void IO_Redirection_a(char *input, char *args[]) {
	char * inputfile = getInputFile(input);
	char * outputfile = getOutputFile(input);
	char * inputString, *append = isAppend(input);
	char * pattern = getPattern(rawInput);


	if(outputfile == NULL) { // when type 8a command is inside type 6
		outputfile = args[1]; // output redirection is given in 6 but not in 8a
		append = args[2];
	}


	char * programAndArgs = strtok(input, "<>"); // strtok modify the supplied string
	removeSpaces(programAndArgs);
	split(programAndArgs); // dont call any method after this which also use split
	char * programName = array[0];
	if(programName == NULL) {
		printError();
		exit(1);
	}

	inputString = getArray(100);
	if(strcmp(programName, "checkcpupercentage") == 0) {
		strcpy(inputString, programName);
		if(isInteger(array[1]) == 1) {
			strcat(inputString, " ");
			strcat(inputString, array[1]);
		}
		char * arguments[] = {inputfile, outputfile, append, NULL};
		checkCPUPercentage(inputString, arguments);
	} else if(strcmp(programName, "checkresidentmemory") == 0) {
		strcpy(inputString, programName);
		if(isInteger(array[1]) == 1) {
			strcat(inputString, " ");
			strcat(inputString, array[1]);
		}
		char * arguments[] = {inputfile, outputfile, append, NULL};
		checkResidentMemory(inputString, arguments);
	} else if(strcmp(programName, "listFiles") == 0) {
		strcpy(inputString, programName);
		char * arguments[] = {NULL, outputfile, append, NULL};
		listfiles(inputString, arguments);
	} else if(strcmp(programName, "sortFile") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		char * arguments[] = {inputfile, outputfile, append, NULL};
		sortfile(inputString, arguments);
	} else if(strcmp(programName, "executeCommands") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		char * arguments[] = {inputfile, outputfile, append, NULL};
		executeCommands(inputString, arguments);
	} else if(strcmp(programName, "cat") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		char * arguments[] = {inputfile, outputfile, append, NULL};
		cat(inputString, arguments);
	} else if(strcmp(programName, "grep") == 0) {
		if(inputfile == NULL) { // take last argument as file which is not pattern.
			int i;
			for(i = 0; i < 10 && array[i] != NULL; i++);
			inputfile = array[i-1]; // take last one.
			if(i < 3) inputfile = NULL;
			if(getCharIndex(inputfile, '"') != -1) inputfile = NULL;
		}
		char * arguments[] = {inputfile, outputfile, append, pattern, NULL};
		grep(arguments);
	} else {
		printError();
		exit(1);
	}
	exit(1);
}

//--------------------------------------------- CASE 8b ---------------------------------------------------------------------------------
void pipeWriter(char *input1, char *args[]) {
	char *input = getArray(100);
	strcpy(input, input1);
	input = strtok(input, "|");
	if(input == NULL) {
		printError(); exit(1);
	}
	removeSpaces(input);
	char *inputfile = getInputFile(input);
	input = strtok(input, "<>");
	removeSpaces(input);
	split(input);

	char * programName = array[0];
	if(programName == NULL) {
		printError();
		exit(1);
	}

	char *inputString = getArray(100);
	if(strcmp(programName, "checkcpupercentage") == 0) {
		strcpy(inputString, programName);
		if(isInteger(array[1]) == 1) {
			strcat(inputString, " ");
			strcat(inputString, array[1]);
		}
		char * arguments[] = {inputfile, NULL, NULL, NULL};
		checkCPUPercentage(inputString, arguments);
	} else if(strcmp(programName, "checkresidentmemory") == 0) {
		strcpy(inputString, programName);
		if(isInteger(array[1]) == 1) {
			strcat(inputString, " ");
			strcat(inputString, array[1]);
		}
		char * arguments[] = {inputfile, NULL, NULL, NULL};
		checkResidentMemory(inputString, arguments);
	} else if(strcmp(programName, "listFiles") == 0) {
		strcpy(inputString, programName);
		char * arguments[] = {NULL, NULL, NULL, NULL};
		listfiles(inputString, arguments);
	} else if(strcmp(programName, "sortFile") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		char * arguments[] = {inputfile, NULL, NULL, NULL};
		sortfile(inputString, arguments);
	} else if(strcmp(programName, "executeCommands") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		char * arguments[] = {inputfile, NULL, NULL, NULL};
		executeCommands(inputString, arguments);
	} else if(strcmp(programName, "cat") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		char * arguments[] = {inputfile, NULL, NULL, NULL};
		cat(inputString, arguments);
	} else if(strcmp(programName, "grep") == 0) {
		char *pattern = getArray(100);
		strcpy(pattern, rawInput);
		pattern = strtok(pattern, "|");
		pattern = getPattern(pattern);
		if(inputfile == NULL) { // take last argument as file which is not pattern.
			int i;
			for(i = 0; i < 10 && array[i] != NULL; i++);
			inputfile = array[i-1]; // take last one.
			if(i < 3) inputfile = NULL;
			if(getCharIndex(inputfile, '"') != -1) inputfile = NULL;
		}
		char * arguments[] = {inputfile, NULL, NULL, pattern, NULL};
		grep(arguments);
	} else {
		printError();
		exit(1);
	}
	exit(1);
}

void pipeReader(char *input1, char *args[]) {

	char *input = getArray(100);
	strcpy(input, input1);
	input = strtok(input, "|");
	input = strtok(NULL, "|");
	if(input == NULL) {
		printError(); exit(1);
	}
	removeSpaces(input);
	char *outputfile = getOutputFile(input), *append = isAppend(input);
	input = strtok(input, "<>");
	removeSpaces(input);
	split(input);

	char * programName = array[0];
	if(programName == NULL) {
		printError();
		exit(1);
	}
	char *pipeInput = getArray(100);

	char *inputString = getArray(100);
	if(strcmp(programName, "checkcpupercentage") == 0) {
		strcpy(inputString, programName);
		if(isInteger(array[1]) == 1) {
			strcat(inputString, " ");
			strcat(inputString, array[1]);
		} else { // read pid form pipe
			char *pid = readPipe(50);
			pid = strtok(pid, " \n");
			if(isInteger(pid) == 1) {
				strcat(inputString, " ");
				strcat(inputString, pid);
			} else {
				printError(); exit(1);
			}
		}
		char * arguments[] = {NULL, outputfile, append, NULL};
		checkCPUPercentage(inputString, arguments);
	} else if(strcmp(programName, "checkresidentmemory") == 0) {
		strcpy(inputString, programName);
		if(isInteger(array[1]) == 1) {
			strcat(inputString, " ");
			strcat(inputString, array[1]);
		} else { // read pid form pipe
			char *pid = readPipe(50);
			pid = strtok(pid, " \n");
			if(isInteger(pid) == 1) {
				strcat(inputString, " ");
				strcat(inputString, pid);
			} else {
				printError(); exit(1);
			}
		}
		char * arguments[] = {NULL, outputfile, append, NULL};
		checkResidentMemory(inputString, arguments);
	} else if(strcmp(programName, "listFiles") == 0) {
		strcpy(inputString, programName);
		char * arguments[] = {NULL, NULL, append, NULL};
		listfiles(inputString, arguments);
	} else if(strcmp(programName, "sortFile") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		} else { // read file name from pipe
			char *file = readPipe(50);
			file = strtok(file, " \n");
			if(isFileName(file) == 1) {
				strcat(inputString, " ");
				strcat(inputString, file);
			} else {
				printError(); exit(1);
			}
		}
		char * arguments[] = {NULL, outputfile, append, NULL};
		sortfile(inputString, arguments);
	} else if(strcmp(programName, "executeCommands") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		} else { // read file name from pipe
			char *file = readPipe(50);
			file = strtok(file, " \n");
			if(isFileName(file) == 1) {
				strcat(inputString, " ");
				strcat(inputString, file);
			} else {
				printError(); exit(1);
			}
		}
		char * arguments[] = {NULL, outputfile, append, NULL};
		executeCommands(inputString, arguments);
	} else if(strcmp(programName, "cat") == 0) {
		strcpy(inputString, programName);
		if(array[1] != NULL) {
			strcat(inputString, " ");
			strcat(inputString, array[1]); // copy input file
		}
		// read STDIN data from pipe no need of file name if file name is not given
		char * arguments[] = {NULL, outputfile, append, NULL};
		cat(inputString, arguments);
	} else if(strcmp(programName, "grep") == 0) {
		char *pattern = getArray(100);
		strcpy(pattern, rawInput);
		pattern = strtok(pattern, "|");
		pattern = strtok(NULL, "|");
		pattern = getPattern(pattern);
		// read data from pipe
		char * arguments[] = {NULL, outputfile, append, pattern, NULL};
		grep(arguments);
	} else {
		printError();
		exit(1);
	}
	exit(1);
}


/*------------------------------------------- PARSER ------------------------------------------------------------------------------------*/

int getCommandType(char *input) {
	if(input == NULL) return 0;
	
	int noOfCommands = getCommandCount(input);
	if(noOfCommands == 2) return 5; // type 5 command
	if(noOfCommands > 2) return 0; // more than two parallel commands not allowed

	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == '|') return 82; // 8b
	}
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == '<' || input[i] == '>') return 81; // 8a
	}

	split(input);
	
	if(strcmp(array[0], "cat") == 0 || strcmp(array[0], "grep") == 0) return 81;

	if(strlen(input) > 0 && strcmp(input, "exit") == 0) {
		return 9;
	}
	if(strcmp(array[0],"checkcpupercentage") == 0 && isInteger(array[1]) == 1 && array[2] == NULL) {
		return 1; // case no. 
	}
	if(strcmp(array[0], "checkresidentmemory") == 0 && isInteger(array[1]) == 1 && array[2] == NULL) {
		return 2;
	}
	if(strcmp(array[0], "listFiles") == 0 && array[1] == NULL) {
		return 3;
	}
	if(strcmp(array[0], "sortFile") == 0 && isFileName(array[1]) == 1 && array[2] == NULL) {
		return 4;
	}
	if(strcmp(array[0], "executeCommands") == 0 && isFileName(array[1]) == 1 && array[2] == NULL) {
		return 6;
	}
	return 0;
}

/*-------------------------------------- Running command -----------------------------------------------------------------------------------*/

void commander(char *input, int commandType, char * args[]) {
	if(commandType == -1)  commandType = getCommandType(input);
	char *input1, *input2;
	int cid, cid1, cid2;
	switch(commandType) {
		case 0:
			if(args[1] == NULL) printError();
			else { // when type 6 has Output redirection and illegal command in list
				cid = fork();
				if(cid == 0) {
					int fd = open(args[1], O_WRONLY | O_CREAT | O_APPEND, 0777);
					if(fd != -1) {
						dup2(fd, 1); close(fd);
					}
					printError();
					exit(1);
				} else {
					wait(NULL);
				}
			}
			break;
		case 1:
			cid = fork();
			if(cid == 0) {
				checkCPUPercentage(input, args);
			} else {
				wait(NULL);
			}
			break;
		case 2:
			cid = fork();
			if(cid == 0) {
				checkResidentMemory(input, args);
			} else {
				wait(NULL);
			}
			break;
		case 3:
			cid = fork();
			if(cid == 0) {
				listfiles(input, args);
			} else {
				wait(NULL);
			}
			break;
		case 4:
			cid = fork();
			if(cid == 0) {
				sortfile(input, args);
			} else {
				wait(NULL);
			}
			break;
		case 5:
			input1 = strtok(input, ";");
			input2 = strtok(NULL, ";");
			removeSpaces(input1);
			removeSpaces(input2);
			char * rawInput1 = strtok(rawInput, ";");
			char * rawInput2 = strtok(NULL, ";");
			cid1 = fork();
			if(cid1 == 0) {
				rawInput = rawInput1;
				commander(input1, -1, args);
				exit(0);
			} else if(cid1 > 0){
				cid2 = fork();
				if(cid2 == 0) {
					rawInput = rawInput2;
					commander(input2, -1, args);
					exit(0);
				} else if(cid2 > 0) {
					waitpid(cid1, NULL, 0); // instead of NULL pass integer so that child store status in that
					waitpid(cid2, NULL, 0); // instead of 0 pass options field to wait till different type of situation
				}
			}
			break;
		case 6:
			cid = fork();
			if(cid == 0) {
				executeCommands(input, args);
			} else {
				int status=-1;
				wait(&status);
				if(WEXITSTATUS(status) == 2) { // there is exit in between commands.
					exit(0);
				}
			}
			break;
		case 81:
			cid = fork();
			if(cid == 0) {
				IO_Redirection_a(input, args);
			} else {
				wait(NULL);
			}
			break;
		case 82:
			if(pipe(pipefd) == -1) {
				printf("pipe creation error\n");
				exit(1);
			}
			cid1 = fork();
			if(cid1 == 0) { // child writes
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				pipeWriter(input, args);
			} else { // parent reads
				close(pipefd[1]);
				cid2 = fork();
				if(cid2 == 0) {
					dup2(pipefd[0], STDIN_FILENO);
					pipeReader(input, args);
				} else {
					close(pipefd[0]);
					wait(NULL);
					wait(NULL);
				}
			}
			break;
		case 9:
			exit(2);
	}
}




void main() {
	char *input;
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	while (1) {
		printf("myShell> ");
		fflush(stdout);
		input = readline();
		char *arg[10] = {NULL};
		commander(input, -1, arg);
	}
	return;
}
