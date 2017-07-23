#pragma once
#include "statelist.h"
#include "timer.h"
#include "graphics.h"
#include "keydata.h"
#include "audio.h"

class Game
{
public:
    GameState * currentState = NULL;
    Graphics * graphicsEngine = NULL;
    AudioSystem * audioSystem = NULL;

    SDL_Event eventQueue;

    bool isRunning = false;

    Game()
    {
        currentState = new Menu();

        graphicsEngine = new Graphics();

        audioSystem = new AudioSystem();
    }

    bool Init()
    {
        srand(time(0)); ///randomgenerátor seedelése

        isRunning = true;

        LoadGlobals(); ///globális változók betöltése

        if(!graphicsEngine->Init()) ///grafikus motor inicializálása
            return 0;

        audioSystem->Init();
        //graphicsEngine->LoadContent();
        graphicsEngine->LoadImages(); ///képanyag betöltése

        audioSystem->LoadContent();

        currentState->Init(); ///játékhelyzet inicializálása

        return 1;
    }

    void SwitchState(GameState * newState)
    {

        currentState = newState;
        currentState->Init();
    }

    void HandleEvents()
    {

        while( SDL_PollEvent( &eventQueue ) != 0 )
        {
            switch(eventQueue.type)
            {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                KeyData.AnyPress=true;
                switch(eventQueue.key.keysym.sym)
                {
                case SDLK_UP:
                    KeyData.UpDown = true;
                    KeyData.UpPress = true;
                    break;
                case SDLK_DOWN:
                    KeyData.DownDown = true;
                    KeyData.DownPress = true;
                    break;
                case SDLK_LEFT:
                    KeyData.LeftDown = true;
                    KeyData.LeftPress = true;
                    break;
                case SDLK_RIGHT:
                    KeyData.RightDown = true;
                    KeyData.RightPress = true;
                    break;
                case SDLK_RETURN:
                    KeyData.EnterPress = true;
                    break;
                case SDLK_ESCAPE:
                    KeyData.EscapePress = true;
                    break;
                case SDLK_x:
                    KeyData.XPress = true;
                    break;
                case SDLK_k:
                    KeyData.KPress = true;
                    break;
                case SDLK_o:
                    KeyData.OPress = true;
                    break;
                case SDLK_e:
                    KeyData.EPress = true;
                    break;
                case SDLK_w:
                    KeyData.lastv = 1;
                    KeyData.WDown = true;
                    KeyData.WPress = true;
                    break;
                case SDLK_s:
                    KeyData.lastv = 2;
                    KeyData.SDown = true;
                    KeyData.SPress = true;
                    break;
                case SDLK_a:
                    KeyData.lasth = 1;
                    KeyData.ADown = true;
                    KeyData.APress = true;
                    break;
                case SDLK_d:
                    KeyData.lasth = 2;
                    KeyData.DDown = true;
                    KeyData.DPress = true;
                    break;
                case SDLK_c:
                    KeyData.CPress = true;
                    break;
                case SDLK_f:
                    KeyData.FPress = true;
                default:
                    break;


                }
                break;
            case SDL_KEYUP:
                switch(eventQueue.key.keysym.sym)
                {
                case SDLK_UP:
                    KeyData.UpDown = false;
                    break;
                    case SDLK_DOWN:
                    KeyData.DownDown = false;
                    break;
                    case SDLK_LEFT:
                    KeyData.LeftDown = false;
                    break;
                case SDLK_RIGHT:
                    KeyData.RightDown = false;
                    break;
                case SDLK_w:
                    KeyData.WDown = false;
                    break;
                case SDLK_s:
                    KeyData.SDown = false;
                    break;
                case SDLK_a:
                    KeyData.ADown = false;
                    break;
                case SDLK_d:
                    KeyData.DDown = false;
                    break;
                default:
                    break;


                }
                break;

            case SDL_MOUSEMOTION:
                KeyData.MouseMove = true;
                KeyData.MouseX = eventQueue.motion.x;
                KeyData.MouseY = eventQueue.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch(eventQueue.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        KeyData.Click = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        KeyData.RightClick = true;
                        break;

                }
                break;
            case SDL_MOUSEWHEEL:
                if(eventQueue.wheel.y < 0)
                    KeyData.wheel = -1;
                else
                    KeyData.wheel = 1;
                break;
            default:
                break;
            };
        }
    }
    void SwitchState(int newState)
    {
        switch(newState)
        {
            case EXIT:
                isRunning = false;
                break;
            case MENU:
                SwitchState(new Menu());
                break;
            case GAME:
                SwitchState(new Ingame());
                break;
            case LOAD:
                SwitchState(new LoadGame());
                break;
            case CREATE:
                SwitchState(new CreateGame());
                break;
            case START:
                SwitchState(new NewGame());///még nem használt "játék létrehozása" interfész
            default:
                return;
        }
    }
    void Update()
    {
        ///Bemeneti perifériák kezelése
        HandleEvents();

        ///Frissítse a játékhelyzetet
        int returnstate = currentState->Update();
         ///Ha a frissítés során más helyzetbe került a program
        if(returnstate != NOTHING)
            ///Cserélje ki a currentState objektumot
            SwitchState(returnstate);

        audioSystem->Update();
       ///Nullázza le a bemeneti perifériákra vonatkozó adatokat
        KeyData.Update();

    }

    void Draw()
    {
        graphicsEngine->Clear(); ///képernyő törlése
        currentState->Draw(); ///jelenlegi helyzet kirajzolása
        graphicsEngine->Finalize(); ///kirajzolás véglegesítése
    }

    void Quit()
    {

    }

};
