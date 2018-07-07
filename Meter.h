#pragma once
#include "Entity.h"

class Meter : public Entity
{
    int * maxValue;
    int * currentValue;
    String barColor,emptyColor;
    String type;
    
    public:

    Meter(int x, int y, int w, int h, int * currentValue, int * maxValue, String barColor, String emptyColor, String type);

    void Draw(float cX = 0, float cY = 0);

};
