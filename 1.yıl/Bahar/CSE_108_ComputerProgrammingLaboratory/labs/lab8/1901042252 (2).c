#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int* initStack(int *arr, int n);

int* push(int *arr, int temp, int *filled, int *capacity);

int main()
{
	srand(time(0));

	int *arr;
	int n; //initial length
	int max = 4, min = 2;
	int capacity;
	
	int temp;
	int filled = 0;

	n = rand()%(max - min + 1) + min;
	capacity = n;


	arr = initStack(arr, n);

	for(int i=0; i<(n*n); i++)
	{
		temp = rand()%(max - min + 1) + min;
		printf("temp : %d\n", temp);
		arr = push(arr, temp, &filled, &capacity);
	}


	for(int i=0; i<n*n; i++)
		printf("%d\n", arr[i]);

	

	//free(arr);



	return 0;
}

int* initStack(int *arr, int n)
{
	arr = (int*)malloc(sizeof(int) * n);
	return arr;
}

int* push(int *arr, int temp, int *filled, int *capacity)
{
	int index;

	if(*filled == 0)
	{
		arr[(*filled)++] = temp;
		return arr;
	}


	int *saved = (int*)malloc(sizeof(int));
	int savedIndex = 0;

	index = (*filled) - 1;

	while(temp > arr[index])
	{
		saved[savedIndex++] = arr[index];
		saved = (int*)realloc(saved, savedIndex+1);
		index--;
	}

	index++;

	if(*filled > *capacity)
	{
		arr = (int*)realloc(arr, ++(*capacity));
	}
	

	arr[index++] = temp;

	for(int i=0; i<=savedIndex; i++)
	{
		arr[index++] = saved[savedIndex-i];
	}


	free(saved);

	return arr;

}