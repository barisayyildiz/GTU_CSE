#include <stdio.h>
#include <stdlib.h>

char *func(char *s);

int main()
{
	char s[100];

	printf("DNA sequence : ");
	scanf("%s", s);
	printf("\nThe result of the encoding : %s\n", func(s));


	return 0;
}

char *func(char *s)
{
	int index = 0, temp;
	char *result = (char*)malloc(sizeof(char) * 200);
	char current;
	int digit;

	while(*s != '\0')
	{
		temp = 1;
		current = *s;
		s++;
		while(*s == current && *s != '\0')
		{
			temp++;
			s++;
		}
		result[index++] = current;
		result[index++] = (char)temp + 48;
	}

	result[index] = '\0';
	return result;
}