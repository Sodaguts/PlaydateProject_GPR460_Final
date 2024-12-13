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

#include <stdbool.h>

#include "pd_api.h"

static int update(void* userdata);
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont* font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

#define MAX_GAMEOBJECTS 10
#define MAX_RENDERERS 10
#define MAX_RESIZERS 5




/// <summary>
/// Renderer component determines whether or not
/// the object it is attached to is rendered.
/// </summary>
struct RendererComponent 
{
	int posX;
	int posY;
	int width;
	int height;
	int lineWidth;
	float startAngle;
	float endAngle;
	LCDColor color;
	bool isActive;
};
struct RendererComponent rendererWorld[MAX_RENDERERS];
struct RendererComponent go_createRenderer(int posX, int posY, int width, int height, int lineWidth, float startAngle, float endAngle, LCDColor color) 
{
	for (int i = 0; i < MAX_RENDERERS; i++) 
	{
		if (rendererWorld[i].isActive == false)
		{
			struct RendererComponent rc = { posX, posY, width, height, lineWidth, startAngle, endAngle, color, true};
			rendererWorld[i] = rc;
			return rc;
		}
	}
}
void go_deactivateRendererAt(int index) 
{
	rendererWorld[index].isActive = false;
}

/// <summary>
/// Resize component resizes object (renderer)
/// by size based on the turn angle of the crank
/// </summary>
struct ResizeComponent 
{
	float size;
	bool isActive;
};
struct ResizeComponent resizerWorld[MAX_RESIZERS];
struct ResizeComponent go_createResizeComponent(float size) 
{
	for (int i = 0; i < MAX_RESIZERS; i++) 
	{
		if (resizerWorld[i].isActive == false)
		{
			struct ResizeComponent rc = { size, true };
			resizerWorld[i] = rc;
			return rc;
		}
	}
}
void go_deactivateResizerAt(int index) 
{
	resizerWorld[index].isActive = false;
}

struct GameObject
{
	//maintains its own position
	float x;
	float y;
	bool isActive;
	struct RendererComponent renderer;
	struct ResizeComponent resizer;
};

struct GameObject gameObjectWorld[MAX_GAMEOBJECTS];

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg)
{
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit )
	{
		const char* err;
		font = pd->graphics->loadFont(fontpath, &err);
		
		if ( font == NULL )
			pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);

		// create max gameobjects
		//setup
		for (int i = 0; i < MAX_GAMEOBJECTS; i++)
		{
			int randX = rand() % (200 - 10 + 1) + 10;
			int randY = rand() % (60 - 10 + 1) + 10;
			struct GameObject go = { randX,randY,true };
			go.renderer = go_createRenderer(randX, randY, 25,25,5,0,360,kColorBlack);
			go.resizer = go_createResizeComponent(5);
			gameObjectWorld[i] = go;
		}
		

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
	
	pd->graphics->clear(kColorWhite);
	pd->graphics->setFont(font);
	pd->graphics->drawText("Goodbye World!", strlen("Goodbye World!"), kASCIIEncoding, x, y);

	pd->graphics->drawText("Here is some example text", strlen("Here is some example text"), kASCIIEncoding, 100, 100);

	//crank input
	float crankAngle = pd->system->getCrankAngle();

	PDButtons current;
	pd->system->getButtonState(NULL, &current, NULL);

	if (current & kButtonA) 
	{
		for (int i = 0; i < MAX_GAMEOBJECTS; i++) 
		{
			if (gameObjectWorld[i].isActive == false)
			{
				gameObjectWorld[i].isActive = true;
				int randX = rand() % (200 - 10 + 1) + 10;
				int randY = rand() % (60 - 10 + 1) + 10;
				gameObjectWorld[i].x = randX;
				gameObjectWorld[i].y = randY;
				gameObjectWorld[i].renderer = go_createRenderer(randX, randY, 25,25,5,0,360,kColorBlack);
				break;
			}
		}
	}
	if (current & kButtonB) 
	{
		//deactivate components first
		for (int i = 0; i < MAX_RENDERERS; i++) 
		{
			if (rendererWorld[i].isActive == true)
			{
				go_deactivateRendererAt(i);
				break;
			}
		}

		for (int i = 0; i < MAX_GAMEOBJECTS; i++) 
		{
			if (gameObjectWorld[i].isActive == true)
			{
				gameObjectWorld[i].isActive = false;
				break;
			}
		}
	}

	for (int i = 0; i < MAX_RESIZERS; i++)
	{
		if (resizerWorld[i].isActive == true)
		{
			resizerWorld[i].size = crankAngle;
			if (rendererWorld[i].isActive == true)
			{
				rendererWorld[i].width = crankAngle;
				rendererWorld[i].height = crankAngle;
			}
		}
	}


	

	// render 

	for (int i = 0; i < MAX_RENDERERS; i++) 
	{
		if (rendererWorld[i].isActive == true)
		{
			pd->graphics->drawEllipse(rendererWorld[i].posX, rendererWorld[i].posY, rendererWorld[i].width,
									  rendererWorld[i].height, rendererWorld[i].lineWidth, rendererWorld[i].startAngle,
									  rendererWorld[i].endAngle, rendererWorld[i].color);
		}
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

