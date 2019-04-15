#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define TESTFILE "./test-file.txt"


int main() {
	int fd = open(TESTFILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

	if (fd < 0) {
		printf("error while opening '%s'", TESTFILE);
		exit(1);
	}

	int dupped_fd = dup(fd);

	printf("Original fd: %d, dupped fd: %d\n", fd, dupped_fd);

	write(fd, "orig", 4);
	write(dupped_fd, "dup", 3);

	close(fd);
	close(dupped_fd);

	return 0;
}
