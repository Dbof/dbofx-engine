#include <random>
#include <time.h>

#include "random.h"


//n. power of x = x^n
int pow(int x, int n) 
{
	int num = x;
	for(int i = 0; i < n && n != 1; i++)	
		x *= num;
	return x;
}


void InitRandom()
{
		srand((int)time(NULL));
}

int getRandomInt(int llimit, int hlimit)
{
	return llimit + (rand() % (hlimit - llimit + 1));
}
	
float getRandomFloat(float llimit, float hlimit, int numOfDigits)
{
	float fac = (float) pow(10, numOfDigits);

	int l = (int) (llimit * fac);
	int h = (int) (hlimit * fac);
	
	return ((l + (rand() % (h-l + 1))) / fac) ;
}