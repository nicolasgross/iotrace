#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main() {
	// syscall implementation
	int filedesc = open("./test-file.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);

	if (filedesc < 0) {
		printf("syscall impl error");
		exit(1);
	}

	write(filedesc, "0", 1);
	close(filedesc);


	// FILE implementation
	FILE *fptr;
	fptr = fopen("./test-file.txt","a");

	if(fptr == NULL) {
		printf("FILE impl error");
		exit(1);
	}

	fprintf(fptr,"%d", 1);
	fclose(fptr);

	return 0;
}
