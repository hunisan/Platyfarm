#pragma once

#include "Visible.h"
#include "Item.h"

class Entity : public Visible
{
    public:
        struct Interaction
        {
            String obj;
            String script;
        };
        vector<Interaction> interactions;

        int id;

        float alt, offset;

        int shake = 0;
        int flip = 1;

        int destx, desty;

        int clipw, cliph, imgw, imgh;

        String name;

        String anim_state = "none";

        String type, category;

        String leftover;

        bool pickable = false;
        bool fade = false;
        bool rigid = false;

        String pick;

        String destroy;

        int hits;
        int health;

        vector<String> drops;
        vector<int> mins;
        vector<int> maxs;

        //vector<image> alts;
        map<String, image> alts;

        map<String, int> int_attribs;
        map<String, String> string_attribs;


        bool Interacts(Item * i, int n);
        int Interacts(Item * i);

        String runScript(String s);

        Entity();
        Entity(Entity _ent, float _x, float _y);// : Entity(_ent);
        Entity(image _img, float _x, float _y, float _w, float _h);// : Visible(_img,_x,_y,_w,_h)
        Entity(float x, float y, float w, float h);
        
        virtual bool ContainsPoint(int x, int y);
        float GetDistance(float srcX, float srcY);
        float GetAngle(float srcX,float srcY);
        void KnockBack(float srcX, float srcY, float force);
        void Shake(int amplitude = -1);
        virtual void Draw(float cX = 0, float cY = 0);

        virtual ~Entity();



    protected:

    private:
};

static bool Contains(Entity * e, int mX, int mY, int cx, int cy)
{
    return (mX+cx >= e->x-e->offset/2 && mX+cx <= e->x + e->w + e->offset/2 &&
       mY+cy >= e->y - e->alt && mY+cy <= e->y + e->h);

}

