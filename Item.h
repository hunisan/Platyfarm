#ifndef ITEM_H
#define ITEM_H

#include "Object.h"


class Item : public Object
{
    public:
        String name;
        String formal, desc;
        String category, type;
        image img;
        int id;
        int max_stack;
        int buy,sell;
        int quantity = 1;

        Item();
        Item(image _img);

        virtual ~Item();

    protected:

    private:
};

#endif // ITEM_H
