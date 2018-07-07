#pragma once
#include "gamestate.h"
#include "objects.h"

class NewGame : public GameState
{
private:
    Cursor * cursor;
    GUI * gui;
public:

    void Init();

    int Update();

    void Draw();

    void FeedText(String text);
};
