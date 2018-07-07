#pragma once
#include "GUI.h"

class GUIElemContainer : public GUI
{
    public:
    vector<GUI*> GUIElems;

    void Scroll(int dir);
    void ArrangeElems();
    void SetPosition(int x, int y);
    void AddElem(GUI* elem);
    void Draw(float cX = 0, float cY = 0);

    int HandleClick(int mX, int mY);

    void Exit();

};
