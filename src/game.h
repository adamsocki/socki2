#include "types.h"

#include "memory.h"

#include "dynamic_array.h"

#include "log.h"

#include "math/math.h"

#include "input.h"


struct GameMemory
{
	bool running;

	real32 systemTime;
	real32 startTime;

	uint32 screenHeight;
	uint32 screenWidth;


    uint32 frame;
    real32 fps;

	InputManager inputManager;

	InputDevice *keyboard;
    InputDevice *mouse;


    MemoryArena permanentArena;

    DebugLog log;

    
    MemoryArena frameMem;


};

real32 Time = 0;
real32 DeltaTime = 0;

GameMemory *Game = NULL;

InputDevice *Keyboard = NULL;
InputDevice *Mouse = NULL;