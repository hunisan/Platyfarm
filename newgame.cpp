#include "newgame.h"

void NewGame::Init()
{
    cursor = new Cursor();

}

int NewGame::Update()
{
    cursor->Update();

    return NOTHING;

}

void NewGame::Draw()
{
    DrawImage(images[ids["background"]],0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

    cursor->Draw();
}

void NewGame::FeedText(String text)
{

}

