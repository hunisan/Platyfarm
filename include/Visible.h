#ifndef VISIBLE_H
#define VISIBLE_H

//#include "../public.h"
#include "../images.h"

#include "Object.h"

class Visible
{
    public:
        float x,y;
        int w,h;
        image img;

        Visible();
        Visible(image _img, float _x, float _y, float _w, float _h);

        virtual void Draw(float cX = 0, float cY = 0);

        virtual ~Visible();

    protected:

    private:
};


#endif // VISIBLE_H
