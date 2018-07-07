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
        srand(time(0));

        isRunning = true;

        LoadGlobals();

        if(!graphicsEngine->Init())
            return 0;

        audioSystem->Init();
        //graphicsEngine->LoadContent();
        graphicsEngine->LoadImages();

        audioSystem->LoadContent();

        currentState->Init();

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
            case SDL_TEXTINPUT:
                currentState->FeedText(eventQueue.text.text);
                break;
            case SDL_KEYDOWN:
                KeyData.AnyPress=true;
                if(eventQueue.key.keysym.sym >= SDLK_0 && eventQueue.key.keysym.sym <= SDLK_9)
                {
                    KeyData.numPress[eventQueue.key.keysym.sym-SDLK_0]=true;
                }
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
                        KeyData.MouseLeftDown = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        KeyData.RightClick = true;
                        break;

                }
                break;
            case SDL_MOUSEBUTTONUP:
                KeyData.MouseLeftDown = false;
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
            case EDITOR:
                SwitchState(new Editor());
                break;
            case START:
                SwitchState(new NewGame());
            default:
                return;
        }
    }
    void Update()
    {
        HandleEvents();

        int returnstate = currentState->Update();
        if(returnstate != NOTHING)
            SwitchState(returnstate);

        audioSystem->Update();

        KeyData.Update();

    }

    void Draw()
    {
        graphicsEngine->Clear();
        currentState->Draw();
        graphicsEngine->Finalize();
    }

    void Quit()
    {

    }

};
