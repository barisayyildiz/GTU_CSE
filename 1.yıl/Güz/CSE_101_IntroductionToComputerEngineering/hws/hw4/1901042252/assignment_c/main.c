#include <stdio.h>
#include "arduino-serial-lib.c"
#include "arduino-serial-lib.h"

int main(void)
{
	int dev = serialport_init("/dev/ttyUSB0", 9600);

    int bufMax = 256;

    char buf[bufMax];
    char num[bufMax];

	int command;
	int flag = 1;

	while(flag)
	{
		printf("## WELCOME TO GTU ARDUINO LAB 		##\n"
		   "## STUDENT NAME: BARIŞ AYYILDIZ			##\n"
		   "## PLEASE SELECT FROM THE FOLLOWING  	##\n"
		   "## MENU :                     	    	##\n"
		   "(1) TURN ON LED ON ARDUINO\n"
		   "(2) TURN OFF LED ON ARDUINO\n"
		   "(3) FLASH ARDUINO LED 3 TIMES\n"
           "(4) SEND A NUMBER TO ARDUINO TO COMPUTE SQUARE BY ARDUINO\n"
           "(5) Button press counter (bonus item)\n"
           "(0) EXIT\n"
           "PLEASE SELECT:\n");

		printf("\nYour command is : ");
		scanf("%d", &command);

		if(command == 1)
		{
			printf("Led on\n\n");
			serialport_write(dev, "1");//sends data to arduino

			sleep(1.5);//waits for user to see the result
		}else if(command == 2)
		{
			printf("Led off\n\n");
			serialport_write(dev, "2");//sends data to arduino

			sleep(1.5);//waits for user to see the result
		}else if(command == 3)
		{
			printf("Blinking 3 times...\n\n");
			serialport_write(dev, "3");//sends data to arduino

			sleep(3.5);//waits for user to see the result
		}else if(command == 4)
		{
			printf("Please enter a number : ");
            scanf("%s", num);

            printf("Calculating...\n");

            serialport_write(dev, num);//sends data to arduino

            printf("square of %s : ", num);

            sleep(3); //waits for arduino...
            
            serialport_read_until(dev, buf, '\n', 300, 5000);//reads the result from arduino

            printf("%s\n", buf);//prints the result

            sleep(2); //waits for user to see the result
		}else if(command == 5)
		{
			printf("Under construction...\n\n");
			sleep(1.5);
		}else if(command == 0)
		{
			flag = 0;
			printf("Have a nice day...\n");
		}else
		{
			printf("Not a valid command...\n\n");
			while(getchar() != '\n');//to prevent keyboard buffer

			sleep(1.5);
		}
	}

	serialport_close(dev);

    return 0;
}