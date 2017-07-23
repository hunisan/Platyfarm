#include <iostream>
#include <stdio.h>
#include "game.h"
#include "timer.h"

using namespace std;

int main(int argc, char **argv)
{
    Game * gameSession = NULL; /// A játékmotor objektum
    Timer capTimer; ///A képfrissitést szabályzó időzítő

    gameSession = new Game();

    if(gameSession->Init())
    {
        while(gameSession->isRunning)
        {
            capTimer.start();
            gameSession->Update();/// A játék frissítése
            gameSession->Draw();///A játék kirajzolása

            ///Kiszámolja hogy hány "tick" telt el az előző képkocka óta
            int frameTicks = capTimer.getTicks();
            ///Ha túl kevés idő telt el, azaz a játék felgyorsulna
            if( capTimer.getTicks() < SCREEN_TICKS_PER_FRAME )
                ///A maradék időre felfüggeszti a programot
                SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
        }
        gameSession->Quit();
    }
    else
        printf("Initialization failed. Quitting..."); ///Az inicializáció sikertelen volt


    return 0;
}
