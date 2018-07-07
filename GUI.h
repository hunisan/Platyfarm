#pragma once
#include "Entity.h"
#include "Item.h"

class GUI : public Entity
{
public:
    String type;

    GUI();

    virtual void FeedText(String text);

    virtual void Scroll(int dir);

    virtual void Hover(int mx, int my);

    virtual void Draw(float cX = 0, float cY = 0);

    virtual Item *GetClick(int mX, int mY, int& id);

    virtual int HandleClick(int mX, int mY);

    virtual int HandleRightClick(int mX, int mY);
    virtual int HandleMouseMove(int mX, int mY);
    virtual void Exit();
    virtual void SetPosition(int x, int y);
};
