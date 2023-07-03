#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char byte;

// appendMeMore filename num-bytes [x]
int main( int argc, char *argv[] )  {
	if(argc != 3 && argc != 4){
		printf("2 or 3 arguments should be passed\n");
		return 1;
	}

	int isLSeek = 0;
	if(argc == 4){
		if(strcmp("x", argv[3]) != 0){
			printf("3rd argument should be 'x'\n");
			return 1;
		}
		isLSeek = 1;
	}

	char *filename = argv[1];
	int numOfBytes = atoi(argv[2]);

	int flags = O_CREAT | O_WRONLY;
	if(!isLSeek){
		flags = O_CREAT | O_WRONLY | O_APPEND;
	}

	int fd = open(filename, flags, 0777);

	byte b1 = 'a';
	for(int i=0; i<numOfBytes; i++){
		if(isLSeek){
			lseek(fd, 0, SEEK_END);
		}
		write(fd, &b1, 1);
	}
	close(fd);
	
}