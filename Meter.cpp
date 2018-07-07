#include "Meter.h"

Meter::Meter(int x, int y, int w, int h, int * currentValue, int * maxValue, String barColor, String emptyColor, String type) : Entity(x,y,w,h)
{
    this->currentValue = currentValue;
    this->maxValue = maxValue;
    this->barColor = barColor;
    this->emptyColor = emptyColor;
    this->type = type;

}

void Meter::Draw(float cX, float cY)
{
    float scalar = (float)(*currentValue)/(*maxValue);

    DrawImage(im(emptyColor),x,y,w,h);

    if(type=="horizontal")
        DrawImage(im(barColor),x,y,w*scalar,h);
    else
        DrawImage(im(barColor),x,y+(1.0-scalar)*h,w,h*scalar);

}


