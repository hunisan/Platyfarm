#include "Visible.h"

Visible::Visible()
{
    //ctor
}

Visible::~Visible()
{
    //dtor
}

Visible::Visible(image _img, float _x, float _y, float _w, float _h)
{
    img = _img;
    x = _x;
    y = _y;
    w = _w;
    h = _h;
}

void Visible::Draw(float cX, float cY)
{
    DrawImage(img,x-cX,y-cY,w,h);
}


