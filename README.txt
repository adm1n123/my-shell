CS744 Programming Assignment 1, Building Shell

Deepak Singh Baghel - 203050005 - 203050005@iitb.ac.in - deepakbaghel@iitb.ac.in

COMPILE SOURCE CODE:
	$ make

EXECUTING PROGRAM:
	$ ./shell
	
CLEAN OBJECT FILES:
	$ make clean

CLEAN SOURCE & OBJECT FILES:
	$ make cleanall

ASSUMPTIONS FOR PARSING INPUT:
	1. File name should not have spaces and single quotes in file names are not accepted.
	2. Except point 1. file name could be anything and any number of spaces can be given between any two tokens in input.
	3. For special tokens( '<', '>', '>>', '|', ';') in input no space separation is required with/without space both works.
	4. Pattern in grep can be given with or without single/double quotation marks.
	5. Except the custom commands both "cat" & "grep" works only. And linux commands like "ls", "sort" etc. are not accepted.

REFERENCES:
	https://www.thinkage.ca/gcos/expl/c/lib/open.html
	https://www.educative.io/edpresso/splitting-a-string-using-strtok-in-c
	https://www.geeksforgeeks.org/data-types-in-c/
	https://www.techiedelight.com/pass-2d-array-function-parameter/
	https://unix.stackexchange.com/questions/209688/how-to-locate-a-chrome-tab-from-its-process-pid
	https://www.tutorialspoint.com/unix_system_calls/execve.htm
	https://stackoverflow.com/questions/33205558/trying-to-create-file-in-c-with-full-permissions-for-all
	https://stackoverflow.com/questions/24931456/how-does-sig-atomic-t-actually-work
	https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html#_005f_005fatomic-Builtins	
	https://stackoverflow.com/questions/7136416/opening-file-in-append-mode-using-open-api
	https://stackoverflow.com/questions/37946775/how-to-make-open-truncate-an-existing-file/37947021
	https://stackoverflow.com/questions/5128664/how-to-split-a-c-program-into-multiple-files#:~:text=In%20general%2C%20you%20should%20define,the%20functions%20defined%20in%20another%20.
	https://man7.org/linux/man-pages/man2/pipe.2.html
	https://stackoverflow.com/questions/47356860/c-how-to-detect-eof-in-pipe-when-reading-only-single-char

	Makefile
	https://stackoverflow.com/questions/5950395/makefile-to-compile-multiple-c-programs

