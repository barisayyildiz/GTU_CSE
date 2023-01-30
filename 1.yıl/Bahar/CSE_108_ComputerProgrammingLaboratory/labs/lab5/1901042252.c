#include <stdio.h>

int bintodec(int binary, int initialLength);
//girilen sayının başlangıç uzunluğunun da verilmesi gerektiğini düşünüyorum
//aksi halde loop kullanılması gerekir
//101 sayısı için, soldaki 1 sayısına ulaştığımızda sayının uzunluğu 1 oluyor
//ancak bizim 3-1 = 2 (yani 2^2 ile çarpmamız gerekli), 3 burada başlangıç uzunluğu


int countdigits(int number);
int powerfunction(int base, int power);



int main()
{
	int num;
	while(1)
	{
		printf("Enter a binary number : ");
		scanf(" %d", &num);

		if(num < 0)
			break;

		printf("Decimal : %d\n", bintodec(num, countdigits(num)));
	}

	return 0;
}
int bintodec(int binary, int initialLength)
{
	int total = 0;
	int numDigit;
	if(binary == 0)
	{
		return total;
	}

	total += bintodec(binary/10, initialLength);

	//printf("countdigits : %d, num : %d\n", countdigits(binary), binary);
	//printf("result : %d\n", binary%10 * powerfunction(2, countdigits(binary)));

	//numDigit = countdigits(binary) - 1; => bu şekilde ters bir sonuç veriyor  1010 => 5 gibi...
	numDigit = initialLength - countdigits(binary);

	return total + binary%10 * powerfunction(2, numDigit);


}


int countdigits(int number)
{
	int total = 0;
	if(number == 0)
		return total;

	total += countdigits(number/10);

	return total+1;
}


int powerfunction(int base, int power)
{
	int result = 1;

	if(power == 0)
	{
		return result;
	}

	result *= powerfunction(base, power-1);

	return result*base;
}