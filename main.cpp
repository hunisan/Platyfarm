#include <iostream>
#include <stdio.h>
#include "game.h"
#include "timer.h"

using namespace std;

int main(int argc, char **argv)
{
    Game * gameSession = NULL;
    Timer capTimer;

    gameSession = new Game();

    if(gameSession->Init())
    {
        while(gameSession->isRunning)
        {
            capTimer.start();
            gameSession->Update();
            gameSession->Draw();

            int frameTicks = capTimer.getTicks();
            if( capTimer.getTicks() < SCREEN_TICKS_PER_FRAME )
                SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
        }
        gameSession->Quit();
    }
    else
        printf("Initialization failed. Quitting...");


    return 0;
}
