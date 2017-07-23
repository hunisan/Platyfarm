#pragma once
#include "gamestate.h"
#include "objects.h"

class NewGame : public GameState
{
private:
    Cursor * cursor;
public:

    void Init()
    {
        cursor = new Cursor();

    }
    int Update()
    {
        cursor->Update();

        return NOTHING;
    }
    void Draw()
    {
        DrawImage(images[ids["background"]],0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

        cursor->Draw();
    }
};
