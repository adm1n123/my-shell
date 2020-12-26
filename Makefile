all: file1 file2 file3 file4 file5
file1: shell.c
		gcc -o shell shell.c
file2: checkresidentmemory.c
		gcc -o checkresidentmemory checkresidentmemory.c
file3: checkcpupercentage.c
		gcc -o checkcpupercentage checkcpupercentage.c
file4: listFiles.c
		gcc -o listFiles listFiles.c
file5: sortFile.c
		gcc -o sortFile sortFile.c

clean:
	rm shell
	rm checkresidentmemory
	rm checkcpupercentage
	rm listFiles
	rm sortFile
cleanall:
	rm shell
	rm checkresidentmemory
	rm checkcpupercentage
	rm listFiles
	rm sortFile
	rm *.c
