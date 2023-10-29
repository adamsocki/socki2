#include "game.h"

#include "log.cpp"

void GameInit(GameMemory *gameMem) {
    Game = gameMem;
}

void GameDeinit() {
    if (IS_SERVER) {
        WriteLogToFile("output/server_log.txt");    
    }
    else {
        WriteLogToFile("output/log.txt");    
    }
}