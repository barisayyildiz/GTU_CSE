#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef unsigned char byte;

int my_dup(int oldfd){
	int newfd = fcntl(oldfd, F_DUPFD, 0);
	if(newfd == -1){
		return -1;
	}
	return newfd;
}

int my_dup2(int oldfd, int newfd){
	if(oldfd == newfd){
		if(fcntl(oldfd, F_GETFL) == -1){
			errno = EBADF;
			return -1;
		}
		return oldfd;
	}
	if(close(newfd) == -1){
		errno = EBADF;
		return -1;
	}
	int res = fcntl(oldfd, F_DUPFD, newfd);
	return res;
}

int main( int argc, char *argv[] )  {
	// my_dup
	int copy_desc = my_dup(1);
	write(copy_desc,"This is from copy descriptor of stdin\n", 38);
  write(1, "This is from stdin\n", 19);

  // // close(1);

	// my_dup2
	int fd = open("text2.txt", O_WRONLY | O_CREAT, 0777);
	dup2(fd, 1);
	write(fd, "this is from fd\n", 16);
  write(1, "this if from stdin2\n", 20);


	return 0;
}
