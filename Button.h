#pragma once
#include "GUI.h"

class Button : public GUI
{
    float oX,oY,oW,oH;
    int expand;
public:
    Button(image img,int x,int y,int w,int h);

    void Hover(int mx, int my);

    void Draw(float cX = 0, float cY = 0);

    void ResetSize();
};
