#include <stdio.h>
int isNumeric(char string[], int *sign, int *leftCounter, int*rightCounter, int operation);
float convertFloat(char string[], int *sign, int *leftCounter, int *rightCounter);

void operationSetOne(int operation);
void operationSetTwo(int operation);


int main()
{
	int command;
	int flag = 1;

	while(flag)
	{
		printf("%% WELCOME TO GTU CALCULATOR MACHINE	%%\n"
		   "%% STUDENT NAME: Barış Ayyıldız		%%\n"
		   "%% PLEASE SELECT FROM THE FOLLOWING  	%%\n"
		   "%% MENU :                            \n"
		   "(1) ADD TWO NUMBERS\n"
		   "(2) SUBTRACT TWO NUMBERS\n"
		   "(3) MULTIPLY TWO NUMBERS\n"
           "(4) DIVIDE TWO NUMBERS\n"
           "(5) TAKE THE NTH POWER OF A NUMBER\n"
           "(6) FIND AVERAGE OF NUMBERS INPUTTED\n"
           "(7) FIND THE MAXIMUM OF NUMBERS INPUTTED\n"
           "(0) EXIT\n");

		printf("Your command is : ");

		scanf("%d", &command);

		if(command == 1)
		{
			//add();
			operationSetOne(command);
		}else if(command == 2)
		{
			//subtract();
			operationSetOne(command);
		}else if(command == 3)
		{
			//multiply();
			operationSetOne(command);
		}else if(command == 4)
		{
			//divide();
			operationSetOne(command);
		}else if (command == 5)
		{
			//power();
			operationSetOne(command);
		}else if (command == 6)
		{
			//averageNumber();
			operationSetTwo(command);
		}else if (command == 7)
		{
			//maxNumber();
			operationSetTwo(command);
		}else if (command == 0)
		{
			flag = 0;
			printf("Have a nice day...\n");
		}else
		{
			printf("Not a valid command...\n");
			while ((getchar()) != '\n'); //to prevent keyboard buffer
		}
	}


	return 0;
}
int isNumeric(char string[], int *sign, int *leftCounter, int*rightCounter, int operation)
{
	int pointCounter = 0;//

	int stringLength = 0;
	int turn = 0; //0 for adding to leftCounter, 1 for adding to rightCounter


	for(int i=0; string[i] != '\0'; i++)
	{
		stringLength++;
	}

	if((operation == 6 || operation == 7) && (string[0] == 'q') && (stringLength == 1))
	{
		return -1;
	}


	for(int i=0; i<stringLength; i++)
	{
		if(string[i] >= '0' && string[i] <= '9')
		{
			if(turn == 0)
			{
				*leftCounter += 1;
			}else if(turn == 1)
			{
				*rightCounter += 1;
			}

		}else if(i == 0 && (string[i] == '-' || string[i] == '+'))
		{
			if(string[i] == '-')
			{
				*sign = 1;//0 is for positive, 1 is for negative
			}

		}else if(i != 0 && string[i] == '.')
		{
			pointCounter++;
			if(pointCounter > 1)
			{
				return 0;
			}else
			{
				turn = 1;//change the side, start adding to rightCounter
			}
		}else
		{
			return 0;
		}
	}

	return 1;




}

float convertFloat(char string[], int *sign, int *leftCounter, int *rightCounter)
{
	float exponent;
	float tempNumber;
	float leftTotal = 0;
	float rightTotal = 0;
	float generalTotal = 0;

	for(int i=0; i<*leftCounter; i++)
	{

		exponent = 1;

		//left side of the number
		for(int j = 0; j<*leftCounter - i - 1; j++)
		{
			exponent *= 10;
		}

		if(string[0] == '-' || string[0] == '+')//if the first element in array is used for minus or plus sign, we skip that element
		{
			//In ASCII table, digits start at 48
			//In ASCII table => 0 is 48, 1 is 49, 2 is 50...
			tempNumber = string[i+1] - 48;
			tempNumber *= exponent;
		}else
		{
			tempNumber = string[i] - 48;
			tempNumber *= exponent;	
		}
		


		leftTotal += tempNumber;
		


		}

		//sağ kısma bakılıyor
		for(int i=0; i<*rightCounter; i++)
		{
			exponent = 1;
			for(int j = 0; j<i+1; j++)
			{
				exponent /= 10;
			}


			if(string[0] == '-' || string[0] == '+')
			{

				tempNumber = string[*leftCounter + 2 + i] - 48,
				tempNumber *= exponent;
			}else
			{

				tempNumber = string[*leftCounter + 1 +i] - 48;
				tempNumber *= exponent;
			}


			rightTotal += tempNumber;
		}


		generalTotal = leftTotal + rightTotal;
		if(*sign == 1)//If the first element in array is '-', we take the negative of generalTotal
		{
			generalTotal = -generalTotal;
		}

		return generalTotal;

}

void operationSetOne(int operation)
{
	char num1char[20];
	char num2char[20];
	float num1;
	float num2;
	float result;

	int leftCounter = 0;
	int rightCounter = 0;

	int sign = 0;
	int flag = 1;


	while(flag)
	{
		printf("Please enter the first number : ");
		scanf("%s", num1char);

		if(isNumeric(num1char, &sign, &leftCounter, &rightCounter, operation) == 1)
		{

			num1 = convertFloat(num1char, &sign, &leftCounter, &rightCounter);
			flag  = 0;
		}
		else
		{
			sign = 0;
			leftCounter = 0;
			rightCounter = 0;

			printf("Not a valid number...\n");
		}
	}

	sign = 0;
	leftCounter = 0;
	rightCounter = 0;

	flag = 1;

	while(flag)
	{
		printf("Please enter the second number : ");
		scanf("%s", num2char);

		if(isNumeric(num2char, &sign, &leftCounter, &rightCounter, operation) == 1)
		{
			num2 = convertFloat(num2char, &sign, &leftCounter, &rightCounter);
			flag = 0;
		}
		else
		{
			sign = 0;
			leftCounter = 0;
			rightCounter = 0;

			printf("Not a valid number...\n");
		}

	}
	if(operation == 1)
	{
		//add
		result = 0;
		result = num1 + num2;
		printf("\n%f + %f =  %f\n\n", num1, num2, result);
	}else if(operation == 2)
	{
		//subtract
		result = 0;
		result = num1 - num2;
		printf("\n%f - %f =  %f\n\n", num1, num2, result);
	}else if(operation == 3)
	{
		//multiply
		result = 0;
		result = num1 * num2;
		printf("\n%f x %f =  %f\n\n", num1, num2, result);
	}else if(operation == 4)
	{
		//divide
		result = 0;
		if(num2 == 0)
		{
			printf("Zero division error...\n");
		}else
		{
			result = num1 / num2;
			printf("\n%f / %f =  %f\n\n", num1, num2, result);
		}

	}else if(operation == 5)
	{
		//exponentiation
		result = 1;
		printf("\n%f to the power %f = ", num1, num2);
		if(num2 < 0)
		{
			num1 = 1/num1;
			num2 = -num2;
		}
		for(int i = 0; i<num2; i++)
		{
			result *= num1;
		}
		printf("%f\n\n", result);
	}

}

void operationSetTwo(int operation)
{
	char string[20];

	int isFilled = 0;//for maximum number
	float maxNumber;//for maximum number 
	int control = 0;

	int counter = 0;//for average
	float total = 0;//for average

	float currentNumber;
	float result; //for average

	int leftCounter = 0;
	int rightCounter = 0;

	int sign = 0;

	int flag = 1;


	while(flag)
	{
		printf("Press q to quit or enter a number\n");
		scanf("%s", string);

		if(isNumeric(string, &sign, &leftCounter, &rightCounter, operation) == 1)
		{
			currentNumber = convertFloat(string, &sign, &leftCounter, &rightCounter);

			printf("Current number : %f\n", currentNumber);

			total += currentNumber;
			counter++;

			if(isFilled == 0)
			{
				maxNumber = currentNumber;
				isFilled = 1;
			}else if(currentNumber > maxNumber)
			{
				maxNumber = currentNumber;
			}


		}else if(isNumeric(string, &sign, &leftCounter, &rightCounter, operation) == 0)
		{
			printf("Not a valid number\n");

		}else if(isNumeric(string, &sign, &leftCounter, &rightCounter, operation) == -1)
		{
			if(counter == 0)
			{
				printf("You didn't enter any number...\n");
			}else
			{
				if(operation == 6)
				{
					//average number
					result = total/counter;
					printf("\nAverage : %f\n\n", result);
				}else if(operation == 7)
				{
					//maximum number
					printf("\nMaximum number : %f\n\n", maxNumber);
				}
			}

			flag = 0;
		}

		sign = 0;
		leftCounter = 0;
		rightCounter = 0;


	}

}