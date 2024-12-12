//
//  main.c
//  Extension
//
//  Created by Dave Hayden on 7/30/14.
//  Copyright (c) 2014 Panic, Inc. All rights reserved.
//
//  playdate_c_project Created by Celica Lysik on 12/10/2024

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

struct GameObject 
{
	//maintains its own position
	float x;
	float y;
}gameObject;

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit )
	{
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, pd);
	}
	
	return 0;
}


#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400-TEXT_WIDTH)/2;
int y = (240-TEXT_HEIGHT)/2;
int dx = 1;
int dy = 2;
int bDrawEllipse = 0;

static int update(void* userdata)
{
	PlaydateAPI* pd = userdata;
	struct GameObject go = {200,60};
	
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Goodbye World!", strlen("Goodbye World!"), kASCIIEncoding, x, y);

	pd->graphics->drawText("Here is some example text", strlen("Here is some example text"), kASCIIEncoding, 100, 100);
	//pd->graphics->drawEllipse(100,200,50,50,10,0,360,kColorBlack);

	PDButtons current;
	pd->system->getButtonState(&current, NULL, NULL);
	if (current & kButtonA) 
	{
		bDrawEllipse = 1;
	}
	if (current & kButtonB) 
	{
		bDrawEllipse = 0;
	}

	if (bDrawEllipse == 1) 
	{
		pd->graphics->drawEllipse(go.x, go.y, 25, 25, 5, 0, 360, kColorBlack);
	}

	x += dx;
	y += dy;
	
	if ( x < 0 || x > LCD_COLUMNS - TEXT_WIDTH )
		dx = -dx;
	
	if ( y < 0 || y > LCD_ROWS - TEXT_HEIGHT )
		dy = -dy;
        
	pd->system->drawFPS(0,0);

	return 1;
}

static int getInput(void* userdata) 
{
	return 1;
}

