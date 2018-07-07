#include "GUIElemContainer.h"

void GUIElemContainer::Scroll(int dir)
{
    for(auto e : GUIElems)
        e->Scroll(dir);
}
void GUIElemContainer::ArrangeElems()
{
    int pX = x;
    for(auto e : GUIElems)
    {
        e->SetPosition(pX,y);
        pX += e->w;
        e->h = h;

    }
}
void GUIElemContainer::SetPosition(int x, int y)
{
    this->x = x;
    this->y = y;
    ArrangeElems();
}
void GUIElemContainer::AddElem(GUI* elem)
{
    GUIElems.push_back(elem);
    w += elem->w;
    if(elem->h > h)
        h = elem->h;
    x = SCREEN_WIDTH/2-w/2;
    y = SCREEN_HEIGHT/2-h/2;

    ArrangeElems();
}
void GUIElemContainer::Draw(float cX, float cY)
{
    for(auto g : GUIElems)
    {
        g->Draw();
    }
    for(auto g : GUIElems)
    {
        DrawImage(im("gui2"),g->x+g->w,g->y,1,g->h);
    }
}

int GUIElemContainer::HandleClick(int mX, int mY)
{
    for(auto g : GUIElems)
    {
        if(Contains(g,mX,mY,0,0))
        {
            g->HandleClick(mX,mY);
        }
    }
}

void GUIElemContainer::Exit()
{
    for(auto g : GUIElems)
    {
        g->Exit();
    }
}