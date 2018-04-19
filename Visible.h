#ifndef VISIBLE_H
#define VISIBLE_H

#include "images.h"

#include "Object.h"

class Visible
{
    public:
        float x,y;
        float w,h;
        image img;

        Visible();
        Visible(image _img, float _x, float _y, float _w, float _h);

        virtual void Draw(float cX = 0, float cY = 0);

        virtual ~Visible();


        ///Overridable setter functions
        virtual void setX(float x);
        virtual void setY(float y);
        virtual inline void addX(float deltaX)
        {
            setX(this->x+deltaX);
        }

        virtual inline void addY(float deltaY)
        {
            setY(this->y+deltaY);
        }
        virtual inline void addToPos(float deltaX, float deltaY)
        {
            addX(deltaX);
            addY(deltaY);
        }
        virtual inline void setPos(float x, float y)
        {
            setX(x);
            setY(y);
        }

        virtual void setWidth(float w);
        virtual void setHeight(float h);

    protected:

    private:
};


#endif // VISIBLE_H
