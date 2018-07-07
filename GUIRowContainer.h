#pragma once
#include "GUI.h"

class GUIRowContainer : public GUI
{
public:

    vector<GUI*> rows;

    void Scroll(int dir);
    void ArrangeRows();
    void SetPosition(int x, int y);
    void AddElem(GUI* elem);
    void Draw(float cX = 0, float cY = 0);

    int HandleClick(int mX, int mY);
    void Exit();
};
