#ifndef ImageManager_h
#define ImageManager_h

#include "pd_api.h"
#include "stdio.h"

// load bitmaps and sprites and stuff (not the same thing ig?)
// mainly just load bitmap functions

void loadImage(int w, int h); // 
void drawImage();

int returnOne();

//it will probably be better to store this somewhere else but we'll have drawing text here too 
void displayText(PlaydateAPI* pd, char t[], int x, int y);


#endif // !ImageManager_h
