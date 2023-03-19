#include <stdio.h>
#include <stdlib.h>

int longest[100];
int longestSize = 0;

void printArr(int *arr, int n){
	if(n == 0)
		return;
	printf("candidate sequence : [");
	for(int i=0; i<n; i++){
		printf("%d,", arr[i]);
	}
	printf("\b], size=%d\n", n);
}

void changeLongest(int *temp, int tempSize){
	for(int i=0; i<tempSize; i++){
		longest[i] = temp[i];
	}
	longestSize = tempSize;
}

void algo(int *arr, int arrSize, int *temp, int tempSize, int index){
	if(index >= arrSize){
		if(tempSize > longestSize){
			changeLongest(temp, tempSize);
		}
		printArr(temp, tempSize);
		return;
	}
	if(tempSize == 0 || *arr > temp[tempSize-1]){
		temp[tempSize] = *arr;
		algo(arr+1, arrSize, temp, tempSize+1, index+1);
	}
	algo(arr+1, arrSize, temp, tempSize, index+1);
}

int main(){
	int arr[] = {3,10,7,9,4,11};
	int temp[6];
	algo(arr, 6, temp, 0, 0);
	printf("----------------------------------------\n");
	printArr(longest, longestSize);

	return 0;
}
