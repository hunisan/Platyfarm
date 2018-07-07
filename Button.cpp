#include "Button.h"

Button::Button(image img, int x, int y, int w, int h)
{
    this->x=x;
    this->y=y;
    this->w=w;
    this->h=h;
    this->img = img;

    oX=x;
    oY=y;
    oW=w;
    oH=h;
    expand=5;
    //cout << x << " " << y << " " << w << " " << h << endl;
}


void Button::Hover(int mx, int my)
{
    //cout << "Hover" << endl;
    setX(oX-expand);
    setWidth(oW+2*expand);
    setY(oY-expand);
    setHeight(oH+2*expand);
}

void Button::ResetSize()
{
    setX(oX);
    setY(oY);
    setWidth(oW);
    setHeight(oH);
}
void Button::Draw(float cX, float cY)
{
    DrawImage(img,x,y,w,h);

    ResetSize();
}
