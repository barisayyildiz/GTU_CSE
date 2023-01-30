#include <stdio.h>

typedef struct Pixel
{
	char c;

	//didn't use these varibles
	int row, col;
}Pixel;

void read_image(Pixel pixels[]);
void fill_canvas(Pixel pixels[], Pixel canvas[][9]);
void draw_image(Pixel canvas[][9]);

int main()
{

	int row_number=6, col_number=9;
 	int total_pixel_number = row_number * col_number;

	Pixel pixels[total_pixel_number];
 	Pixel canvas[row_number][col_number];


 	read_image(pixels);
 	fill_canvas(pixels, canvas);
 	draw_image(canvas);
 

 return 0;

}

void draw_image(Pixel canvas[][9])
{
	for(int row=0; row<6; row++)
	{
		for(int col=0; col<9; col++)
		{
			printf("%c", canvas[row][col].c);
		}
		printf("\n");
	}
}

void fill_canvas(Pixel pixels[], Pixel canvas[][9])
{
	int index;
	for(int row=0; row<6; row++)
	{
		for(int col=0; col<9; col++)
		{
			index = 9*row + col;
			canvas[row][col] = pixels[index];
		}
	}
}

void read_image(Pixel pixels[])
{
	FILE *fp = fopen("image.txt", "r");

	char c;
	int counter = 0;
	int index;
	int flag = 1;
	char temp;

	while(fscanf(fp, "%c", &c) != EOF)
	{
		if(c == ',' || c == ';')
		{
			continue;
		}else if(c < '0' || c > '9')
		{
			temp = c;
			continue;
		}


		//switch flag variable
		if(flag == 0)
		{
			flag = 1;
		}else if(flag == 1)
		{
			flag = 0;
		}


		if(flag == 0)
		{
			index = 9 * ((int)c -48);
		}else
		{
			index += ((int)c - 48);

			pixels[index].c = temp;

			//saved it but didn't use
			pixels[index].col = ((int)c - 48);
			pixels[index].row = (index - ((int)c - 48)) / 9;
		}
	}
}