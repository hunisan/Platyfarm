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

void Visible::setX(float x)
{
    this->x = x;
}

void Visible::setY(float y)
{
    this->y = y;
}

void Visible::setWidth(float w)
{
    this->w = w;
}

void Visible::setHeight(float h)
{
    this->h = h;
}
