#include "Inventory.h"
Inventory::Inventory()
{
    //size = 10;
    for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        toolbar[i] = NULL;//Item(content::platy32);
    for(int i = 0; i < INVENTORY_SIZE; i++)
        items[i] = NULL;
    w = 6 + 10*TILESIZE + 9;
    h = 6 + TILESIZE;
    x = SCREEN_WIDTH/2-w/2;
    y = SCREEN_HEIGHT - h;
}

void Inventory::Select(int i)
{
    if(i < INVENTORY_TOOLBAR_SIZE && i>=0)
        selected = i;
}
void Inventory::Add(Item _item)
{
    bool added = false;
    for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        if(toolbar[i] && toolbar[i]->name == _item.name && toolbar[i]->quantity < toolbar[i]->max_stack)
        {
            toolbar[i]->quantity++;
            added = true;
            break;
        }

    if(!added)
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(items[i] && items[i]->name == _item.name && items[i]->quantity < items[i]->max_stack)
        {
            items[i]->quantity++;
            added = true;
            break;
        }

    if(!added)
    for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
    {

        if(!toolbar[i])
        {
            toolbar[i] = new Item(_item);
            added = true;
            break;
        }
    }

    if(!added)
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(!items[i])
        {
            items[i] = new Item(_item);
            added = true;
            break;
        }

}
bool Inventory::IsFull()
{
    for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        if(toolbar[i]==NULL)
            return false;

    for(int i = 0; i < allowedSize; i++)
        if(items[i]==NULL)
            return false;

    return true;
}
void Inventory::Add(Item _item, int q)
{
    if(q)
        for(int i = 0 ; i < q; i++)
            Add(_item);
}
void Inventory::Add(image _img)
{

    Add( Item(_img));
}
void Inventory::Remove(int id, int where)
{
    if(!where)
    {
        toolbar[id]->quantity--;
        if(!toolbar[id]->quantity)
            toolbar[id] = NULL;

    }
    else
    {
        items[id]->quantity--;
        if(!items[id]->quantity)
            items[id] = NULL;
    }
}
void Inventory::Remove(String target)
{
    bool removed = false;
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(items[i] && items[i]->name == target)
        {
            Remove(i,1);
            removed = true;
        }

    if(!removed)
    for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        if(toolbar[i] && toolbar[i]->name == target)
        {
            Remove(i);
            removed = true;
            break;
        }

}
int Inventory::Count(String target)
{
    int _count = 0;
    for(int i = 0; i < INVENTORY_SIZE; i++)
        if(items[i] && items[i]->name == target)
        {
            _count += items[i]->quantity;
        }

    for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        if(toolbar[i] && toolbar[i]->name == target)
        {
            _count += toolbar[i]->quantity;
        }

    return _count;
}
int Inventory::Count(Item _item)
{
    return Count(_item.name);
}
void Inventory::Remove(String target, int nr)
{
    for(int i = 0; i < nr; i++)
        Remove(target);
}
void Inventory::HandleClick(int mx, int my)
{
    mx -= x;
    Select(std::floor(mx/TILESIZE));
}
void Inventory::Draw(float cX, float cY)
{
    DrawImage(images[ids["itembar"]],x,y,w,h);
    for(int i =0 ;i < INVENTORY_TOOLBAR_SIZE; i++)
    {
        if(toolbar[i])
        {
            DrawImage(toolbar[i]->img,x+3+i*TILESIZE+i,SCREEN_HEIGHT-TILESIZE-3,TILESIZE,TILESIZE);

            if(toolbar[i]->quantity > 1)
                DrawString(to_string(toolbar[i]->quantity),x+3+i*TILESIZE+i+TILESIZE-(toolbar[i]->quantity>=10 ? 2 : 1)*DIGITWIDTH,SCREEN_HEIGHT-TILESIZE-3+TILESIZE-FONTSIZE,-1, "font2");
        }

    }
    DrawImage(images[ids["frame"]],x+selected*TILESIZE+selected,y,h,h);
}
