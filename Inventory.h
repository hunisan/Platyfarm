#pragma once
#include "Entity.h"
#include "Item.h"

class Inventory : public Entity
{
public:
    int allowedSize=INVENTORY_TOOLBAR_SIZE;

    Item* items[INVENTORY_SIZE];
    //int size;
    int selected=0;
    Item * toolbar[INVENTORY_TOOLBAR_SIZE];
    Inventory();

    bool IsFull();
    void Select(int i);
    void Add(Item _item);
    void Add(Item _item, int q);
    void Add(image _img);
    void Remove(int id, int where = 0);
    void Remove(String target);
    int Count(String target);
    int Count(Item _item);
    void Remove(String target, int nr);
    void HandleClick(int mx, int my);
    void Draw(float cX = 0, float cY = 0);
};
