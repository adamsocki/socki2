#include "game.h"

#include "input.cpp"

#include "log.cpp"

#include "render.cpp"
#include "render_opengl.cpp"

#include "mesh.cpp"

#include "game_code.cpp"

bool ReadConfigFile(char *path) {
    FILE *file = fopen(path, "r");

    if (file != NULL) {
        int c = fgetc(file);

        enum ConfigState {
            ConfigState_Invalid,
            ConfigState_ScreenWidth,
            ConfigState_ScreenHeight,
            ConfigState_Volume,
            ConfigState_ServerIP,
            ConfigState_Port,
        };

        ConfigState state = ConfigState_ScreenWidth;

        char currentToken[64];
        memset(currentToken, 0, 64);

        int32 tokenLength = 0;
        bool parsedToken = false;

        // @NOTE: this is not an elegant way to do this
        // It would be much nicer if we broke it into tokens first.
        // It would also be nice if we had more file reading features
        while (c != EOF) {
            if (c == '\n' || c == ' ') {
                goto nextChar;
            }

            if (state == ConfigState_ScreenWidth) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }

                if (!parsedToken) {
                    if (strcmp(currentToken, "screenWidth:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->screenWidth = atoi(currentToken);
                        state = ConfigState_ScreenHeight;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

            if (state == ConfigState_ScreenHeight) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }

                if (!parsedToken) {
                    if (strcmp(currentToken, "screenHeight:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->screenHeight = atoi(currentToken);
                        state = ConfigState_Volume;

                        state = ConfigState_Volume;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }

            }

            if (state == ConfigState_Volume) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }

                if (!parsedToken) {
                    if (strcmp(currentToken, "volume:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->audioPlayer.volume = atof(currentToken);

                        state = ConfigState_ServerIP;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

            if (state == ConfigState_ServerIP) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }

                if (!parsedToken) {
                    if (strcmp(currentToken, "server_ip:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->networkInfo.serverIPString = (char *)malloc(tokenLength + 1);
                        memcpy(Game->networkInfo.serverIPString, currentToken, tokenLength + 1);

                        state = ConfigState_Port;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

            if (state == ConfigState_Port) {

                if (c != ';') {
                    currentToken[tokenLength++] = c;
                }

                if (!parsedToken) {
                    if (strcmp(currentToken, "socket_port:") == 0) {
                        tokenLength = 0;
                        parsedToken = true;

                        memset(currentToken, 0, 64);
                    }
                }
                else {
                    if (c == ';') {
                        Game->networkInfo.configPort = atoi(currentToken);

                        state = ConfigState_Invalid;
                        tokenLength = 0;
                        memset(currentToken, 0, 64);
                        parsedToken = false;
                    }
                }
            }

            nextChar:
            c = fgetc(file);
        }

        return true;
    }
    else {
        return false;
    }
}

void GameInit(GameMemory *gameMem) {
    Game = gameMem;

    Input = &Game->inputManager;

    AllocateMemoryArena(&Game->permanentArena, Megabytes(16));
    AllocateMemoryArena(&Game->frameMem, Megabytes(16));

    Game->log.head = (DebugLogNode *)malloc(sizeof(DebugLogNode));
    AllocateDebugLogNode(Game->log.head, LOG_BUFFER_CAPACITY);
    Game->log.current = Game->log.head;
    Game->log.head->next = NULL;

    Camera *cam = &gameMem->camera;
    cam->size = 1;
    cam->type = CameraType_Orthographic;
    cam->width = 16;
    cam->height = 9;
    cam->projection = Orthographic(cam->width * -0.5f * cam->size, cam->width * 0.5f * cam->size,
                                   cam->height * -0.5f * cam->size, cam->height * 0.5f * cam->size,
                                   0.0, 100.0f);

    UpdateCamera(cam, gameMem->cameraPosition, gameMem->cameraRotation);
    // Init Graphics
    AllocateQuad(&gameMem->quad);
    InitMesh(&gameMem->quad);

    AllocateRectBuffer(256 * 256, &Game->rectBuffer);
    printf("hi");
#if WINDOWS
    {
        LoadShader("shaders/instanced_quad_shader.vert", "shaders/instanced_quad_shader.frag", &gameMem->instancedQuadShader);
        const char *uniforms[] = {
                "viewProjection",
        };
        CompileShader(&gameMem->instancedQuadShader, 1, uniforms);
    }
#endif
    //InitGlyphBuffers(GlyphBufferCount);
}

void GameDeinit() {
    if (IS_SERVER) {
        WriteLogToFile("output/server_log.txt");    
    }
    else {
        WriteLogToFile("output/log.txt");    
    }
}

void GameUpdateAndRender(GameMemory *gameMem) {

    UpdateInput(&Game->inputManager);

    InputManager *input = &gameMem->inputManager;

    if (InputPressed(Game->keyboard, Input_Escape)) {
        gameMem->running = false;
    }

    Game->currentGlyphBufferIndex = 0;

    // @TODO: pick a key to step frame and then check if that's pressed
    // We want to do this before the update obviously

    if (!Game->paused || Game->steppingFrame) {
        MyGameUpdate();
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.6f, 0.5f, 0.2f, 1.0f);
//        glClearColor(Mosaic->screenColor.r, Mosaic->screenColor.g, Mosaic->screenColor.b, 1.0f);

        //Mosaic->rectBuffer.count = 0;
        {
           // vec2 pos = 0.0f + V2(16.0f * 0.5f, -16.0f * 0.5f);
            DrawRect(pos, 16.0f * 0.5f, V4(0, 0, 0, 1));
        }
    }

    Camera *cam = &gameMem->camera;
    UpdateCamera(&gameMem->camera, gameMem->cameraPosition, gameMem->cameraRotation);

    Game->steppingFrame = false;
    RenderRectBuffer(&Game->rectBuffer);
    Game->rectBuffer.count = 0;

 //   DrawGlyphs(gameMem->glyphBuffers);

    //DeleteEntities(&Game->entityDB);


    Game->fps = (real32)Game->frame / (Game->time - Game->startTime);
    printf("fps: %f\n", Game->fps);

    gameMem->frame++;
//    printf("frame: %f\n", Game->frame);

    ClearMemoryArena(&Game->frameMem);

    ClearInputManager(input);
}
