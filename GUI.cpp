#include "GUI.h"

GUI::GUI()
{
    x=y=w=h=0;
}

void GUI::Scroll(int dir)
{

}

void GUI::Draw(float cX, float cY)
{

}

void GUI::Hover(int mx, int my)
{
    
}
Item * GUI::GetClick(int mX, int mY, int& id)
{
    return NULL;
}

int GUI::HandleClick(int mX, int mY)
{
    return 0;
}

int GUI::HandleRightClick(int mX, int mY)
{
    return 0;
}
int GUI::HandleMouseMove(int mX, int mY)
{
    return 0;
}
void GUI::Exit()
{

}
void GUI::SetPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}
void GUI::FeedText(String text)
{
    
}