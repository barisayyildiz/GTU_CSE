#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef unsigned char byte;

int main( int argc, char *argv[] )  {

	int fd, fd2;
	off_t offset, offset2;
	fd = open("text3.txt", O_CREAT | O_WRONLY, 0777);
	write(fd, "hello", 5);
  lseek(fd, 3, SEEK_CUR);

  // copy fd to fd2
	fd2 = dup(fd);

	offset = lseek(fd, 0, SEEK_CUR);
	offset2 = lseek(fd2, 0, SEEK_CUR);

  printf("offset of fd : %ld\n", (long)offset);
  printf("offset of fd2 : %ld\n", (long)offset2);

	close(fd);
  close(fd2);

	return 0;
}