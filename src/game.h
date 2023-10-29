#include "types.h"

#include "memory.h"

#include "dynamic_array.h"

#include "log.h"

#include "math/math.h"
#include "audio.h"
#include "network.h"


#include "camera.h"
#include "render.h"

#include "input.h"

#define HERTZ 160.0f


#define FRAME_RATE 1 / HERTZ

struct GameMemory
{

    bool paused;
	bool running;
    bool steppingFrame;

	real32 systemTime;
    real32 time;
    real32 deltaTime;
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

    AudioPlayer audioPlayer;

    NetworkInfo networkInfo;


    int32 currentGlyphBufferIndex;

    Camera camera;
    vec3 cameraPosition;
    quaternion cameraRotation;

//    RectBuffer rectBuffer;
};

real32 Time = 0;
real32 DeltaTime = 0;

GameMemory *Game = NULL;
InputManager *Input = NULL;

InputDevice *Keyboard = NULL;
InputDevice *Mouse = NULL;