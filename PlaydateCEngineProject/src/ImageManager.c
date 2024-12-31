#include "ImageManager.h"

void loadImage(int w, int h) 
{
	// implementation here
}

void drawImage() 
{
	// implmentation here
}

int returnOne() 
{
	return 25;
}

void displayText(PlaydateAPI* pd, char t[], int x, int y)
{
	//use the char array to print out text to the playdate
	pd->graphics->drawText(t, strlen(t), kASCIIEncoding, x, y);
}