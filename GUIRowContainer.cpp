#include "GUIRowContainer.h"

void GUIRowContainer::Scroll(int dir)
{
    for(auto e : rows)
        e->Scroll(dir);
}
void GUIRowContainer::ArrangeRows()
{
    int pY = y;
    for(auto e : rows)
    {
        e->SetPosition(x,pY);
        pY += e->h;
        e->w = w;

    }
}
void GUIRowContainer::SetPosition(int x, int y)
{
    this->x = x;
    this->y = y;
    ArrangeRows();
}
void GUIRowContainer::AddElem(GUI* elem)
{
    rows.push_back(elem);
    h += elem->h;
    if(elem->w > w)
        w = elem->w;
    x = SCREEN_WIDTH/2-w/2;
    y = SCREEN_HEIGHT/2-h/2;

    ArrangeRows();
}
void GUIRowContainer::Draw(float cX, float cY)
{
    for(auto g : rows)
    {
        g->Draw();
    }
    for(auto g : rows)
    {
        DrawImage(im("gui2"),g->x,g->y+g->h,g->w,1);
    }
}

int GUIRowContainer::HandleClick(int mX, int mY)
{
    for(auto g : rows)
    {
        if(Contains(g,mX,mY,0,0))
        {
            g->HandleClick(mX,mY);
        }
    }
}
void GUIRowContainer::Exit()
{
    for(auto g : rows)
    {
        g->Exit();
    }
}