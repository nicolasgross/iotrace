#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main() {
	int fd = open("./test-file.txt", O_RDONLY);

	if (fd < 0) {
		printf("syscall impl error");
		exit(1);
	}

	int dupped_fd = dup(fd);

	printf("Original fd: %d, dupped fd: %d\n", fd, dupped_fd);

	close(fd);
	close(dupped_fd);

	return 0;
}
