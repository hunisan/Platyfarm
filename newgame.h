#pragma once
#include "gamestate.h"
#include "objects.h"

class NewGame : public GameState
{
private:
    Cursor * cursor;
public:

    void Init();

    int Update();

    void Draw();
};
