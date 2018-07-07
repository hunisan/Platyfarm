#include "Entity.h"

Entity::Entity()
{
    //ctor
    alt=0;
    offset=0;
}

Entity::Entity(Entity _ent, float _x, float _y) : Entity(_ent)
{
    x = _x;
    y = _y;
    //alt = 0;
    //offset = 0;
}
Entity::Entity(float x, float y, float w, float h) : Entity()
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}
Entity::Entity(image _img, float _x, float _y, float _w, float _h) : Visible(_img,_x,_y,_w,_h)
{
    alt=0;
    offset=0;
    destx = _x;
    desty = _y;
    clipw = imgw = _w;
    cliph = imgh = _h;
    health = 1;
}

Entity::~Entity()
{
    //dtor
}

bool Entity::ContainsPoint(int x, int y)
{
    return x>=this->x&&x<=this->x+this->w && y>=this->y && y<=this->y+this->h;
}

float Entity::GetDistance(float srcX, float srcY)
{
    float dX = srcX-x;
    float dY = srcY-y;
    return sqrt(dX*dX+dY*dY);
}

float Entity::GetAngle(float srcX, float srcY)
{
    return atan2(srcX-x,srcY-y);
}

void Entity::KnockBack(float srcX, float srcY, float force)
{
    /*float angle = GetAngle(srcX,srcY);

    this->addX(cos(angle)*force);
    this->addY(sin(angle)*force);

    cout << angle << endl;*/

    if(abs(x-srcX)>abs(y-srcY))
    {
        if(x < srcX)
            addX(-force);
        else if(x > srcX)
            addX(force);

    }
    else
    {
        if(y < srcY)
            addY(-force);
        else if(y > srcY)
            addY(force);

    }
}
String Entity::runScript(String s)
{
    String response = "";
    String c = FirstWord(s);
    String p1 = SecondWord(s);
    String p2 = nWord(3,s);

    if(c=="script")
    {
        for(auto d : split(s,"script"))
            response += "script " + runScript(d) + " ";
    }
    else if(c=="attrib")
    {
        string_attribs[p1]=p2;
    }
    else if(c=="harvest")
    {
        response = "script item " + p1 + " " + p2;
        response += " script delete";
    }
    else if(c =="change")
    {
        response = "change " + p1;
    }
    else if(c == "dialog")
    {
        response = "dialog " + p1;
    }


    return response;
}
void Entity::Shake(int amplitude)
{
    if(!(rand()%2))
        shake = 1;
    else
        shake = amplitude;
}
void Entity::Draw(float cX, float cY)
{
    if(anim_state != "none" && alts.count(anim_state))
    {
        DrawClip(alts[anim_state],x-offset/2-cX+shake,y-alt-cY+shake,w+offset,h+alt,0,0,clipw,cliph,imgw,imgh,(float)(!fade)/2 + 0.5);
    }
    else if(alts.count("damaged") != 0 && health < hits)
    {
        DrawClip(alts["damaged"],x-offset/2-cX+shake,y-alt-cY+shake,w+offset,h+alt,0,0,clipw,cliph,imgw,imgh,(float)(!fade)/2 + 0.5);
    }
    else if(category == "plant")
    {
        DrawClip(img,x-offset/2-cX+shake,y-alt-cY+shake,w+offset,h+alt,int_attribs["growth"]*clipw,0,clipw,cliph,imgw,imgh,(float)(!fade)/2 + 0.5);
    }
    else
        DrawClip(img,x-offset/2-cX+shake+(flip>0?0:w),y-alt-cY+shake,flip*(w+offset),h+alt,0,0,clipw,cliph,imgw,imgh,(float)(!fade)/2 + 0.5);

    shake = 0;
}
bool Entity::Interacts(Item * i, int n)
{
    Interaction intr = interactions[n];

    if((FirstWord(intr.obj)=="name" && SecondWord(intr.obj)==i->name)||
           (FirstWord(intr.obj)=="category" && SecondWord(intr.obj)==i->category)||
           (FirstWord(intr.obj)=="type" && SecondWord(intr.obj)==i->type)||
           (FirstWord(intr.obj)=="compare" && i->string_attribs.count(SecondWord(intr.obj)) && nWord(3,intr.obj)==i->string_attribs[SecondWord(intr.obj)]) ||
           (FirstWord(intr.obj)=="attr" && i->int_attribs.count(SecondWord(intr.obj))))
           return true;

    return false;
}
int Entity::Interacts(Item * i)
{
    for(int j = 0; j < interactions.size(); j++)
        if(Interacts(i,j))
            return j;

    return -1;
}
