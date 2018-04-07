#ifndef OBJECTS_H
#define OBJECTS_H

#include "public.h"
#include "keydata.h"
#include "declarations.h"
#include "Object.h"
#include "Visible.h"

/*class Object
{
public:
    virtual int Update()
    {
        return 1;
    }
};
*/
/*class Visible : public Object
{
public:
    float x,y;
    int w,h;
    image img;
    Visible(){};

    Visible(image _img, float _x, float _y, float _w, float _h)
    {
        img = _img;
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }
    virtual void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(img,x-cX,y-cY,w,h);
    }
};
*/
class GameVariables : public Object
{
public:
    map<string,string> string_vars;
    map<string,int> int_vars;

    bool IfDef(string name)
    {
        if(string_vars.count(name))
            return true;

        if(int_vars.count(name))
            return true;

        return false;
    }

    int GetInt(string name)
    {
        if(int_vars.count(name))
            return int_vars[name];

        return -1;
    }
    string GetString(string name)
    {
        if(string_vars.count(name))
            return string_vars[name];

        return "error";
    }
    void SetInt(string name, int value)
    {
        int_vars[name] = value;
    }
    void SetString(string name, string value)
    {
        string_vars[name] = value;
    }
};

class Cursor : public Visible
{
public:
    enum
    {
        NORMAL,
        WAIT,
        PICK,
        INTERACT,
        GIVE,
        TALK,
        INSPECT,
        ADD,
    };

    int state = 0;
    int states = 8;
    string text;
    image preview;
    bool isPreview = false;

    Cursor() : Visible(images[ids["cursor"]],0,0,24,24)
    {
        //state = "cursor";
    }
    int Update()
    {

        x = KeyData.MouseX;
        y = KeyData.MouseY;

        return 1;
    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawClip(img,x,y,w,h,state*64,0,64,64,states*64,2*64);
        if(isPreview)
        {
            int place_x = std::floor((x+camera_x)/TILESIZE);
            int place_y = std::floor((y+camera_y)/TILESIZE);
            DrawImage(preview,place_x*TILESIZE-camera_x,place_y*TILESIZE-camera_y,TILESIZE,TILESIZE);

        }
        DrawImage(images[ids["fade"]],x+w,y,lineWidth(text),FONTSIZE);
        DrawString(text,x+w,y,-1,"font2");
    }
};
class Item : public Object
{
public:
    string name;
    string formal, desc;
    string category, type;
    image img;
    int id;
    int max_stack;
    int buy,sell;
    int quantity = 1;

    map<string, int> int_attribs;
    map<string, string> string_attribs;

    Item(){};
    Item(image _img)
    {
        img = _img;
    };

};

class Entity : public Visible
{
public:

    struct Interaction
    {
        string obj;
        string script;
    };
    vector<Interaction> interactions;

    bool Interacts(Item * i, int n)
    {
        Interaction intr = interactions[n];

        if((FirstWord(intr.obj)=="name" && SecondWord(intr.obj)==i->name)||
               (FirstWord(intr.obj)=="category" && SecondWord(intr.obj)==i->category)||
               (FirstWord(intr.obj)=="type" && SecondWord(intr.obj)==i->type)||
               (FirstWord(intr.obj)=="attr" && i->string_attribs.count(SecondWord(intr.obj)) && nWord(3,intr.obj)==i->string_attribs[SecondWord(intr.obj)]))
               return true;

        return false;
    }
    int Interacts(Item * i)
    {
        for(int j = 0; j < interactions.size(); j++)
            if(Interacts(i,j))
                return j;

        return -1;
    }
    int id;

    float alt, offset;

    int shake = 0;
    int flip = 1;

    int destx, desty;

    int clipw, cliph, imgw, imgh;

    string name;

    string anim_state = "none";

    string type, category;

    string leftover;

    bool pickable = false;
    bool fade = false;
    bool rigid = false;

    string pick;

    string destroy;

    int hits;
    int health;

    vector<string> drops;
    vector<int> mins;
    vector<int> maxs;

    //vector<image> alts;
    map<string, image> alts;

    map<string, int> int_attribs;
    map<string, string> string_attribs;

    string runScript(string s)
    {
        string response = "";
        string c = FirstWord(s);
        string p1 = SecondWord(s);
        string p2 = nWord(3,s);

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
    Entity()
    {
        alt=0;
        offset=0;
    }
    Entity(Entity _ent, float _x, float _y) : Entity(_ent)
    {
        x = _x;
        y = _y;
        //alt = 0;
        //offset = 0;
    }
    Entity(image _img, float _x, float _y, float _w, float _h) : Visible(_img,_x,_y,_w,_h)
    {
        alt=0;
        offset=0;
        destx = _x;
        desty = _y;
        clipw = imgw = _w;
        cliph = imgh = _h;
        health = 1;
    }
    void Shake(int amplitude = -1)
    {
        if(!(rand()%2))
            shake = 1;
        else
            shake = amplitude;
    }
    void Draw(float cX = 0, float cY = 0)
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
        /*if(GUI_ENABLED)
        {

            for(int i = 0; i < 32; i++)
                for(int j = 0; j < 32; j++)
            {
                if(alpha_maps[std::distance(images.begin(),std::find(images.begin(),images.end(),img))].alpha[i][j])
                {
                    DrawImage(images[std::distance(images.begin(),std::find(images.begin(),images.end(),img))],x+i-cX,y+j-cY,1,1);

                }
            }
        }*/
        shake = 0;
    }
};


Entity Door(string portal, int x, int y,int wherex= 0, int wherey = 0)
{
    Entity ent;
    ent = Entity(images[ids["door"]],x,y,TILESIZE,TILESIZE);
    ent.alt = 1*TILESIZE;
    ent.string_attribs["portal"] = portal;
    ent.string_attribs["where"] = to_string(wherex*TILESIZE)+" "+to_string(wherey*TILESIZE);

    return ent;
}
bool OnScreen(Entity * ent, float cX, float cY)
{
    if(ent->x - ent->offset/2 > cX + SCREEN_WIDTH || ent->x + ent->w + ent->offset/2 < cX
        || ent->y - ent->alt > cY + SCREEN_HEIGHT || ent->y + ent->h < cY)
            return false;

    return true;
}
bool Intersect(Entity * ent1, Entity* ent2)
{
    if(ent1->x+1 > ent2->x + ent2->w-1 || ent1->x + ent1->w-1 < ent2->x+1
    || ent1->y+1 > ent2->y + ent2->h-1 || ent1->y + ent1->h-1 < ent2->y+1)
        return false;

    return true;
}
bool Intersect(Entity * ent, int x, int y, int w, int h)
{
    if(ent->x+1 > x+w-1 || ent->x + ent->w < x+1
    || ent->y+1 > y+h-1 || ent->y + ent->h-1 < y+1)
        return false;

    return true;
}
bool Contains(Entity * e, int mX, int mY, int cx, int cy)
{
    return (mX+cx >= e->x-e->offset/2 && mX+cx <= e->x + e->w + e->offset/2 &&
       mY+cy >= e->y - e->alt && mY+cy <= e->y + e->h);

}

bool Sorter(Entity* ent1, Entity* ent2)
{
    if(!(ent1))
        return false;

    if(!(ent2))
        return true;

    if((ent1)->y == (ent2)->y)
    {
        if((ent1)->x == (ent2)->x)
            return (ent1)->id < (ent2)->id;
        else
            return (ent1)->x < (ent2)->x;

    }

    return (ent1)->y < (ent2)->y;
}


class Creature : public Entity
{
public:
    struct Modifier
    {
        string stat;
        int points;
        int duration;
        Modifier(string s, int p, int d) : stat(s),points(p),duration(d)
        {

        }
    };
    float movement_speed;
    int cooldown = 0;
    vector<string> stats;
    map<string, int> stat_levels;
    vector<Modifier> modifiers;

    void recalculateStats()
    {
        movement_speed = stat_levels["speed"];
        for(auto m : modifiers)
        {
            if(m.stat == "speed")
            {
                movement_speed+=m.points;
            }
        }
        if(movement_speed<10)
            movement_speed=10;

        movement_speed/=10;
    }
    void increaseStat(string targetStat, int points, int duration)
    {
        cout << "increasing stat " << targetStat << " by " << points << " points" << endl;
        if(!duration)
            stat_levels[targetStat]+=points;
        else
        {
            modifiers.push_back(Modifier(targetStat,points,duration));
        }

        recalculateStats();
    }
    Creature()
    {
        movement_speed=1;
    }

    Creature(image _img, float _x, float _y, float _w, float _h, float _alt = 0) : Entity(_img,_x,_y,_w,_h)
    {
        movement_speed=3;
        alt = _alt;
    }


    void setCooldown(int cd)
    {
        cooldown = cd;
    }
    int Update()
    {
        for(int i = 0; i < modifiers.size(); i++)
        {
            modifiers[i].duration--;
            if(!modifiers[i].duration)
            {
                modifiers.erase(modifiers.begin()+i);
                i--;
                recalculateStats();
            }

        }
        if(cooldown)
            cooldown--;

        return 1;
    }
};

class Inventory : public Entity
{
public:
    Item* items[INVENTORY_SIZE];
    //int size;
    int selected=0;
    Item * toolbar[INVENTORY_TOOLBAR_SIZE];
    Inventory()
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

    void Select(int i)
    {
        if(i < INVENTORY_TOOLBAR_SIZE && i>=0)
            selected = i;
    }
    void Add(Item _item)
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
    void Add(Item _item, int q)
    {
        if(q)
            for(int i = 0 ; i < q; i++)
                Add(_item);
    }
    void Add(image _img)
    {

        Add( Item(_img));
    }
    void Remove(int id, int where = 0)
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
    void Remove(string target)
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
    int Count(string target)
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
    int Count(Item _item)
    {
        return Count(_item.name);
    }
    void Remove(string target, int nr)
    {
        for(int i = 0; i < nr; i++)
            Remove(target);
    }
    void HandleClick(int mx, int my)
    {
        mx -= x;
        Select(std::floor(mx/TILESIZE));
    }
    void Draw(float cX = 0, float cY = 0)
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
};

class CraftingRecipe
{
public:
    string item;
    int count;
    vector<string> ingredients;
    vector<int> counts;
    string medium;
    string unlock;
    string skill;
    int level;
    bool unlocked;

};
class Player : public Creature
{

public:
    int lastdir;
    float current_frame = 0;

    Player()
    {
        lastdir = 0;
    }
    Player(Creature c) : Creature(c)
    {
        lastdir = 0;
        offset = TILESIZE/2;
        alt = TILESIZE/2;

    }
    void Draw(float cX = 0, float cY = 0)
    {

        DrawClip(img,x-cX-offset/2,y-cY-alt,w+offset,h+alt,floor(current_frame)*50,lastdir*(64),50,64,g("player-anim-frames")*50,4*(64));

        /*if(GUI_ENABLED)
        {

            for(int i = 0; i < 32; i++)
                for(int j = 0; j < 32; j++)
            {
                if(alpha_maps[std::distance(images.begin(),std::find(images.begin(),images.end(),img))].alpha[i][j])
                {
                    DrawImage(images[std::distance(images.begin(),std::find(images.begin(),images.end(),img))],x+i-cX,y+j-cY,1,1);

                }
            }
        }*/
    }
};

class NPC : public Creature
{
protected:
    Entity * player;

public:
    image portrait;
    int on_meet;
    int on_greet;
    map<string,string> attributes;

    string behaviour="passive";

    bool met = false;
    string fullname;

    map<string,int> AIprams;

    string AI()
    {
        if(behaviour == "passive")
        {

        }
        else if(FirstWord(behaviour)=="follow")
        {

        }
        else if(behaviour == "lazy")
        {
            if(AIprams["action"]==0)///sleep
            {
                anim_state="sleep";
                AIprams["energy"]++;
                if(AIprams["energy"] >= globals["sleep_energy"])
                {
                    AIprams["action"]=2;///roam
                    anim_state="none";
                }
            }
            else if(AIprams["action"]==2)
            {
                if(AIprams["move"])
                {
                    float angle = AIprams["angle"]/100.0;
                    if(cos(angle)<0)
                        flip = 1;
                    else
                        flip = -1;

                    /*x += cos(angle);
                    y += sin(angle);
                    if(x < 0)
                        x = 0;
                    if(y < 0)
                        y = 0;*/

                    AIprams["move"]--;
                    AIprams["energy"] -= globals["movement_energy_cost"];
                    if(AIprams["energy"]<=0)
                        AIprams["action"]=0;

                    return "move " + to_string(angle);
                }
                else
                {
                    int c = rand()%100;
                    if(c==1)
                    {
                        AIprams["move"]=10;
                        AIprams["angle"]=rand()%620;
                    }
                }
            }
        }
        else if(behaviour == "roam")
        {
            if(AIprams["move"])
            {
                float angle = AIprams["angle"]/100.0;
                //x += cos(angle);
                //y += sin(angle);
                AIprams["move"]--;
                return "move " + to_string(angle);

            }
            else
            {
                int c = rand()%100;
                if(c==1)
                {
                    AIprams["move"]=10;
                    AIprams["angle"]=rand()%620;
                }
            }
        }


        return "nothing";
    }

    NPC(NPC _npc, int _x, int _y) : NPC(_npc)
    {
        x = _x;
        y = _y;
    }

    NPC()
    {

    }
};

class Map : public Object
{
public:
    int tile[100][100];
    int w,h;

    Map()
    {
        for(int i = 0; i < 100; i++)
        {
            for(int j = 0; j < 100; j++)
            {
                tile[i][j]=0;
            }
        }

        //tile[5][6]=1;
        w = 100;
        h = 100;

    }
    Map(int z)
    {
        w = h = z;
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
                tile[i][j]=1;
        }
    }
    void Draw(float cX = 0, float cY = 0)
    {
        int tX = cX/TILESIZE;
        int tY = cY/TILESIZE;

        for(int i = tX-1; i < tX + SCREEN_WIDTH/TILESIZE+2; i++)
        {
            for(int j = tY-1; j < tY + SCREEN_HEIGHT/TILESIZE+2; j++)
            {
                if(i >= 0 && i < w && j >= 0 && j < h)
                {
                    DrawClip(images[ids["tileset"]],i*TILESIZE-cX,j*TILESIZE-cY,TILESIZE,TILESIZE,(tile[i][j]%g("tiles-horizontal"))*TILESIZE,(tile[i][j]/g("tiles-horizontal"))*TILESIZE,TILESIZE,TILESIZE,g("tiles-horizontal")*TILESIZE,(int)ceil((double)g("tiles")/g("tiles-horizontal"))*TILESIZE);
                }
            }
        }
    }
};
class ParticleSystem : public Object
{
public:
    vector<Entity> particles;
    int cooldown = 0;


    void Add(int x, int y, int w, int h, image img, bool overr = false, int speed=1)
    {
        if(!cooldown || overr)
        {
            cooldown = globals["particle-cooldown"];
            particles.push_back(Entity(img,x*2,y*2,2*w,2*h));
            particles[particles.size()-1].int_attribs["speed"]=speed;
            particles[particles.size()-1].int_attribs["transparency"]=globals["particle_opacity"];
        }
        else
            cooldown--;
    }

    void Add(Entity e, int speed=1)
    {
        particles.push_back(Entity(e.img,e.x*2,e.y*2,e.w*2,e.h*2));
        particles[particles.size()-1].int_attribs["speed"]=speed;
        particles[particles.size()-1].int_attribs["transparency"]=globals["particle_opacity"];

    }
    int Update()
    {
        for(int i = 0; i < particles.size(); i++)
        {
            particles[i].int_attribs["transparency"] -= particles[i].int_attribs["speed"];
            particles[i].w+=2;particles[i].x--;
            particles[i].h+=2;particles[i].y--;
            if(particles[i].int_attribs["transparency"]<=0)
            {
                particles.erase(particles.begin()+i);
                i--;
            }

        }

        return NOTHING;
    }
    void Draw(float cX = 0, float cY = 0)
    {
        for(auto p : particles)
            if(p.x/2>0 && p.y/2>0)
            DrawImage(p.img,p.x/2-cX,p.y/2-cY,p.w/2,p.h/2,p.int_attribs["transparency"]/100.0);
    }
};
class WeatherSystem : public Object
{
public:
    string weather = "rain";
    string climate = "mediterranean";
    ParticleSystem * ps = NULL;
    bool active = true;
    const static int particles = 100;
    float particleSpeed = 8;
    float coordinates[particles][3];
    float limits[particles];

    WeatherSystem()
    {

    }
    WeatherSystem(ParticleSystem * _ps)
    {
        ps = _ps;
        fr(0,particles)
        {
            coordinates[i][1]= -1 * rand()%(SCREEN_HEIGHT);
            coordinates[i][0] = rand()%((int)(SCREEN_WIDTH*1.5));
            coordinates[i][2]= -coordinates[i][1]/SCREEN_HEIGHT;
            limits[i] = 0;//coordinates[i][1]+SCREEN_HEIGHT;
        }

    }

    void FeedClimate(string climate)
    {
        this->climate = climate;
    }
    int Update()
    {
        if(active && weather!="none" && climate=="continental")
        fr(0,particles)
        {
            particleSpeed = 8*coordinates[i][2]+6;
            coordinates[i][1] += particleSpeed*cos(3.14/6);
            coordinates[i][0] -= particleSpeed*sin(3.14/6);
            if(coordinates[i][1] > limits[i] || coordinates[i][0] < 0)
            {
                ps->Add(coordinates[i][0]+camera_x,coordinates[i][1]+camera_y,globals["particle-size"],globals["particle-size"],images[ids["water"]],1);

                coordinates[i][1]= -1 * rand()%(SCREEN_HEIGHT);
                coordinates[i][0] = rand()%((int)(SCREEN_WIDTH*1.5));
                coordinates[i][2]=-coordinates[i][1]/SCREEN_HEIGHT;
                limits[i] = coordinates[i][1]+SCREEN_HEIGHT;
            }
        }
        return 1;
    }
    void Draw(float cX = 0, float cY = 0)
    {
        if(active && weather != "none" && climate=="continental")
        fr(0,particles)
        {
            DrawImage((weather=="rain"?images[ids[particle]]:images[ids["snow"]]),coordinates[i][0]+camera_xvel,coordinates[i][1]+camera_yvel,TILESIZE,TILESIZE);
        }
    }
};


class LightingSystem : public Object
{
public:
    float lightmap[400][400];
    int w = SCREEN_WIDTH/(LIGHTSIZE)+1;
    int h = SCREEN_HEIGHT/(LIGHTSIZE)+1;
    float globallight;

    LightingSystem()
    {
        globallight = 0.8;
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
            {
                lightmap[i][j]= (i*1.0/w+j*1.0/h) / 2;
            }
        }
    }
    void Draw()
    {
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < w; j++)
            {
                /*if(lightmap[i][j] == 1.0)
                    printf("wutifak\n");*/

                DrawRect(i*LIGHTSIZE,j*LIGHTSIZE,LIGHTSIZE,LIGHTSIZE,lightmap[i][j]);
            }
        }
        DrawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,globallight);
    }

    int Update()
    {

        globallight = (current_hour*100-900+current_minute)*1.00/(2359-900);
        for(int i = 0; i < w; i++)
        {
            for(int j = 0; j < h; j++)
            {
                lightmap[i][j]= globallight;//(i*1.0/w+j*1.0/h) / 2;
            }
        }

        for(float i = 0; i < 20; i++)
        {
            for(float j = 0; j < 20; j++)
            {
                if(sqrt(i*i+j*j) < 20)
                {

                    ///TODO CLEANUP !!!!
                    int currentx = KeyData.MouseX/LIGHTSIZE - i;
                    int currenty = KeyData.MouseY/LIGHTSIZE - j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0.0+sqrt(i*i+j*j)/20*globallight;
                    }

                    currentx = KeyData.MouseX/LIGHTSIZE + i;
                    currenty = KeyData.MouseY/LIGHTSIZE - j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0+sqrt(i*i+j*j)/20*globallight;
                    }
                    currentx = KeyData.MouseX/LIGHTSIZE - i;
                    currenty = KeyData.MouseY/LIGHTSIZE + j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0+sqrt(i*i+j*j)/20*globallight;
                    }

                    currentx = KeyData.MouseX/LIGHTSIZE + i;
                    currenty = KeyData.MouseY/LIGHTSIZE + j;

                    if(currentx >= 0 && currenty >= 0 && currentx < w && currenty < h)
                    {
                        lightmap[currentx][currenty]=0+sqrt(i*i+j*j)/20*globallight;
                    }
                }
            }
        }


        return 1;
    }
};

///SABLONOK

map<string, NPC> npc_templates;
map<string, Item> item_templates;
map<string, Entity> object_templates;
vector<string> object_names;
map<string, CraftingRecipe> recipes;

bool LoadObjects()
{
    printf("Loading objects from XML list...\n");
    using namespace tinyxml2;

    XMLDocument xmlDoc;
    XMLError result;

    result = xmlDoc.LoadFile("data/xml/object.xml");

    XMLCheckResult(result);
    printf("XML file loaded..\n");

    XMLElement * pRoot = xmlDoc.FirstChildElement();
    if(pRoot == NULL)
    {
        printf("XML read error (root)\n");
        return 0;
    }
    XMLElement * pElement = pRoot->FirstChildElement();
    if(pElement == NULL)
    {
        printf("XML read error!\n");
        return 0;
    }
    int id;

    //object_templates.push_back(Entity());
    object_templates.clear();
    object_names.clear();

    while (pElement != NULL)
    {
        printf("Loading object\n");

        //pElement->QueryIntAttribute("id", &id);

        Entity newobject;

        newobject.name = pElement->FirstChildElement("name")->GetText();

        object_names.push_back(newobject.name);

        newobject.img = images[ids[pElement->FirstChildElement("img")->GetText()]];
        //newobject.max_stack = atoi(pElement->FirstChildElement("stack")->GetText());
        newobject.category = pElement->FirstChildElement("category")->GetText();
        newobject.type = pElement->FirstChildElement("type")->GetText();
        newobject.w = (pElement->FirstChildElement("width")?atoi(pElement->FirstChildElement("width")->GetText())*TILESIZE:TILESIZE);
        newobject.h = (pElement->FirstChildElement("height")?atoi(pElement->FirstChildElement("height")->GetText())*TILESIZE:TILESIZE);

        if(pElement->FirstChildElement("rigid") != NULL)
            newobject.rigid = true;

        if(pElement->FirstChildElement("pick") != NULL)
        {
            newobject.pickable = true;
            newobject.pick = pElement->FirstChildElement("pick")->GetText();
        }
        if(pElement->FirstChildElement("leftover") != NULL)
        {
            newobject.leftover = pElement->FirstChildElement("leftover")->GetText();
        }
        if(pElement->FirstChildElement("growth")!= NULL)
        {
            newobject.int_attribs["growth"] = 0;
            newobject.int_attribs["cycle"] = atoi(pElement->FirstChildElement("growth")->GetText());
            newobject.string_attribs["ripe"] = pElement->FirstChildElement("ripe")->GetText();
        }
        else
        {
            //newobject.int_attribs["growth"] = 0;
            //newobject.int_attribs["cycle"] = 1;

        }

        if(pElement->FirstChildElement("destroy") != NULL)
            newobject.destroy = pElement->FirstChildElement("destroy")->GetText();

        if(pElement->FirstChildElement("hits") != NULL)
            newobject.hits = atoi(pElement->FirstChildElement("hits")->GetText());
        else
            newobject.hits = 1;

        newobject.health = newobject.hits;

        if(pElement->FirstChildElement("alt_img") != NULL)
            newobject.alts["damaged"] = images[ids[pElement->FirstChildElement("alt_img")->GetText()]];

        if(pElement->FirstChildElement("fade") != NULL)
            newobject.alts["fade"] = images[ids[pElement->FirstChildElement("fade")->GetText()]];

        if(pElement->FirstChildElement("damaged_fade") != NULL)
            newobject.alts["damaged_fade"] = images[ids[pElement->FirstChildElement("damaged_fade")->GetText()]];

        if(pElement->FirstChildElement("alt") != NULL)
        {
            newobject.alt = atoi(pElement->FirstChildElement("alt")->GetText())*TILESIZE;
        }

        if(pElement->FirstChildElement("offset") != NULL)
        {
            newobject.offset = atoi(pElement->FirstChildElement("offset")->GetText())*TILESIZE;
        }

        if(pElement->FirstChildElement("imgw") != NULL)
            newobject.imgw = atoi(pElement->FirstChildElement("imgw")->GetText())*TILESIZE;
        else
            newobject.imgw = TILESIZE;

        if(pElement->FirstChildElement("imgh") != NULL)
            newobject.imgh = atoi(pElement->FirstChildElement("imgh")->GetText())*TILESIZE;
        else
            newobject.imgh = TILESIZE;

        if(pElement->FirstChildElement("clipw") != NULL)
            newobject.clipw = atoi(pElement->FirstChildElement("clipw")->GetText())*TILESIZE;
        else
            newobject.clipw = newobject.imgw;

        if(pElement->FirstChildElement("cliph") != NULL)
            newobject.cliph = atoi(pElement->FirstChildElement("cliph")->GetText())*TILESIZE;
        else
            newobject.cliph = newobject.imgh;

        if(pElement->FirstChildElement("portal")!= NULL)
        {
            newobject.string_attribs["portal"] = pElement->FirstChildElement("portal")->GetText();
        }
        if(pElement->FirstChildElement("attributes")!=NULL)
        {
            XMLElement * listElement = pElement->FirstChildElement("attributes")->FirstChildElement();

            while(listElement)
            {
                if(listElement->GetText())
                {
                    if(is_number(listElement->GetText()))
                    {
                        newobject.int_attribs[listElement->Name()] = atoi(listElement->GetText());

                    }
                    else
                    {
                        newobject.string_attribs[listElement->Name()] = listElement->GetText();

                    }
                }
                else
                    newobject.int_attribs[listElement->Name()] = 1;
                listElement = listElement->NextSiblingElement();
            }
        }
        if(pElement->FirstChildElement("drops")!=NULL)
        {
            XMLElement * listElement = pElement->FirstChildElement("drops")->FirstChildElement();

            while(listElement)
            {
                newobject.drops.push_back(listElement->FirstChildElement("item")->GetText());
                newobject.mins.push_back(atoi(listElement->FirstChildElement("min")->GetText()));
                newobject.maxs.push_back(atoi(listElement->FirstChildElement("max")->GetText()));
                listElement = listElement->NextSiblingElement();
            }
        }
        if(pElement->FirstChildElement("interactions"))
        {
            XMLElement* interaction = pElement->FirstChildElement("interactions")->FirstChildElement("i");

            while(interaction)
            {
                Entity::Interaction intr;
                intr.obj = interaction->FirstChildElement("obj")->GetText();
                intr.script = interaction->FirstChildElement("script")->GetText();
                newobject.interactions.push_back(intr);
                interaction = interaction->NextSiblingElement();
            }
        }
        object_templates[newobject.name]=newobject;

        //object_ids[newobject.name]=id;

        pElement = pElement->NextSiblingElement("object");
    };

    printf("Finished loading objects!\n");
    return 1;
}

class PickupObject : public Entity
{
public:
    PickupObject(int _x, int _y, int _w, int _h, string item) : Entity(item_templates[item].img,_x,_y,_w,_h)
    {
        name = "nothing";

        pickable = true;
        pick = item;
    }

};
class Preview : public Entity
{
    public:
    bool active = false;
    Entity target;

    Preview()
    {
        name = "preview";
        active = false;
    }
    void Add(Entity tg)
    {
        target = tg;
        w = target.w;
        h = target.h;
        alt = target.alt;
        offset = target.offset;

        active = true;
    }
    void Disable()
    {
        active = false;
    }
    void Move(int mx, int my)
    {
        x = mx;
        y = my;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        if(active)
        {
            int place_x = std::floor((x-target.offset/2+cX)/TILESIZE);
            int place_y = std::floor((y-target.alt+cY)/TILESIZE);

            DrawClip(target.img,place_x*TILESIZE-cX,place_y*TILESIZE-cY,w+target.offset,h+alt,0,0,target.clipw,target.cliph,target.imgw,target.imgh,0.5);
        }
    }
};

class Stage : public Object
{
public:
    bool indoors = false;
    map<string,string> attributes;

    int w, h;
    string name;
    Map tilemap;
    Stage * exits[100];

    Entity * grid[MAP_MAX_SIZE][MAP_MAX_SIZE];

    vector<Entity *> objects;
    //vector<Entity *> ptr_to_npcs;

    vector<NPC *> npcs;

    enum {
        GRASS,
        WOOD,
        TILLED,
    };
    void Resize(int w, int h)
    {
        this->w = w;
        this->h = h;
        if(tilemap.w != w)
            tilemap.w = w;
        if(tilemap.h != h)
            tilemap.h = h;
    }
    void ClearGrid()
    {
        for(int i = 0; i < w; i++)
            for(int j = 0; j < h; j++)
                grid[i][j]=NULL;

    }
    void ScanGrid()
    {

        for(auto& o : objects)
        if(o)
            grid[(int)o->x/TILESIZE][(int)o->y/TILESIZE]=o;

    }


    Stage()
    {
        w = h = 100;

        ClearGrid();
    }
    Stage(int z)
    {
        tilemap = Map(z);
        w = h = z;
        ClearGrid();


    }
    bool IsFree(int x, int y)
    {
        return grid[x][y]==NULL;
    }
    bool IsSolid(int x, int y)
    {
        return (tilemap.tile[x][y] >= 38 && tilemap.tile[x][y] < 46) || tilemap.tile[x][y] == 16 || (tilemap.tile[x][y] >= 4 && tilemap.tile[x][y] <= 12);
    }
    bool CanPlant(int x, int y)
    {
        return tilemap.tile[x][y] == TILLED && IsFree(x,y);
    }

    bool IsGround(int x, int y)
    {
        return tilemap.tile[x][y] == GRASS;
    }

    void Set(int x, int y, int n)
    {
        if(n > g("tiles"))
            n = 0;

        tilemap.tile[x][y] = n;
    }
    int Get(int x, int y)
    {
        return tilemap.tile[x][y];
    }
    void Remove(Entity * en)
    {
        int x = std::floor(en->x/TILESIZE);
        int y = std::floor(en->y/TILESIZE);

        for(int i = 0; i < en->w/TILESIZE; i++)
            for(int j = 0; j < en->h/TILESIZE; j++)
                if(grid[x+i][y+j]==en)
                    grid[x+i][y+j]=0;
    }
    void Add(Entity  * en)
    {
        int x = std::floor(en->x/TILESIZE);
        int y = std::floor(en->y/TILESIZE);

        for(int i = 0; i < en->w/TILESIZE; i++)
        {
            for(int j = 0; j < en->h/TILESIZE; j++)
            {
                if(x+i > 0 && x+i < w && y+j > 0 && y+j < h)
                    grid[x+i][y+j] = en;
            }
        }
    }
    void Draw(float cX = 0, float cY = 0)
    {
        tilemap.Draw(cX,cY);
    }
};

class XMLManager
{
public:
    static Stage * LoadStage(string path)
    {
        Stage * stage = new Stage();

        using namespace tinyxml2;

        XMLDocument doc;
        doc.LoadFile(path.c_str());

        XMLElement * root = doc.FirstChildElement();

        stage->name = root->FirstChildElement("name")->GetText();
        string dimensions = root->FirstChildElement("size")->GetText();
        stage->w = atoi(FirstWord(dimensions).c_str());
        stage->h = atoi(SecondWord(dimensions).c_str());

        if(root->FirstChildElement("indoors"))
            stage->indoors = true;

        if(root->FirstChildElement("attributes"))
        {
            XMLElement * attribute = root->FirstChildElement("attributes")->FirstChildElement();

            while(attribute)
            {
                stage->attributes[attribute->Name()] = attribute->GetText();

                attribute = attribute->NextSiblingElement();
            }

        }

        XMLElement * rowElement = root->FirstChildElement("row");
        int height = 0;
        while(rowElement)
        {
            std::stringstream ss;

            string row = rowElement->GetText();

            ss << row;

            fr(0,stage->w)
                {
                    string current;
                    ss >> current;
                    stage->tilemap.tile[i][height] = atoi(current);
                }

            rowElement = rowElement->NextSiblingElement("row");
            height++;
        }

        XMLElement * objectElement = root->FirstChildElement("object");

        while(objectElement)
        {
            Entity * new_object;

            string name = objectElement->FirstChildElement("name")->GetText();
            string position = objectElement->FirstChildElement("pos")->GetText();

            if(name != "door")
            {
                new_object = new Entity(object_templates[name],atoi(FirstWord(position).c_str()),atoi(SecondWord(position)));

            }
            else
            {
                new_object = new Entity(Door("lot",atoi(FirstWord(position).c_str()),atoi(SecondWord(position))));

            }


            XMLElement* attributeElement = NULL;
            if(objectElement->FirstChildElement("attributes") != NULL)
                attributeElement = objectElement->FirstChildElement("attributes")->FirstChildElement();

            while(attributeElement)
            {
                string name = attributeElement->Name();
                string value = attributeElement->GetText();

                if(is_number(value))
                    new_object->int_attribs[name] = atoi(value);
                else
                    new_object->string_attribs[name] = value;

                attributeElement = attributeElement->NextSiblingElement();
            }


            stage->Add(new_object);
            stage->objects.push_back(new_object);

            objectElement = objectElement->NextSiblingElement("object");


        }

        XMLElement * npcElement = root->FirstChildElement("npc");

        while(npcElement)
        {
            NPC * new_object;

            string name = npcElement->FirstChildElement("name")->GetText();
            string position = npcElement->FirstChildElement("pos")->GetText();

            new_object = new NPC(npc_templates[name],atoi(FirstWord(position)),atoi(SecondWord(position).c_str()));

            new_object->name = name;

            if(npcElement->FirstChildElement("met"))
                new_object->met = true;

            stage->npcs.push_back(new_object);


            npcElement = npcElement->NextSiblingElement("npc");
        }

        return stage;
    }
    static void SaveStage(Stage * st, string path)
    {
        using namespace tinyxml2;

        XMLDocument doc;

        XMLNode * root = doc.NewElement("root");

        doc.InsertFirstChild(root);

        XMLElement * pElement;

        auto InsertInt = [&pElement, &root, &doc](string name, int value, XMLNode* elem)
        {
            pElement = doc.NewElement(name.c_str());
            pElement->SetText(value);
            elem->InsertEndChild(pElement);

        };
        auto InsertString = [&pElement, &root, &doc](string name, string value, XMLNode* elem )
        {
            pElement = doc.NewElement(name.c_str());
            pElement->SetText(value.c_str());
            elem->InsertEndChild(pElement);

        };

        InsertString("name",st->name,root);
        InsertString("size",to_string(st->w)+" "+to_string(st->h),root);

        if(st->indoors)
            root->InsertEndChild(doc.NewElement("indoors"));

        fr(0,st->h)
        {
            string row = "";
            for(int j = 0; j < st->w; j++)
                row.append(to_string(st->tilemap.tile[j][i])+" ");

            InsertString("row",row,root);
        }
        for(auto e : st->objects)
        if(e)
        {
            XMLElement * obj = doc.NewElement("object");

            if(e->name != "")
                InsertString("name",e->name,obj);
            else
                InsertString("name","door",obj);

            InsertString("pos",to_string(e->x)+" "+to_string(e->y),obj);

            XMLElement * attribs = doc.NewElement("attributes");

            for(auto a : e->int_attribs)
            {
                InsertString(a.first,to_string(a.second),attribs);
            }
            for(auto a : e->string_attribs)
            {
                InsertString(a.first,a.second,attribs);
            }
            if(attribs->FirstChildElement())
                obj->InsertEndChild(attribs);

            root->InsertEndChild(obj);


        }
        for(auto e : st->npcs)
        if(e)
        {
            XMLElement * obj = doc.NewElement("npc");

            InsertString("name",e->name,obj);

            InsertString("pos",to_string(e->x)+" "+to_string(e->y),obj);

            XMLElement * attribs = doc.NewElement("attributes");

            for(auto a : e->int_attribs)
            {
                InsertString(a.first,to_string(a.second),attribs);
            }
            for(auto a : e->string_attribs)
            {
                InsertString(a.first,a.second,attribs);
            }
            if(attribs->FirstChildElement())
                obj->InsertEndChild(attribs);

            root->InsertEndChild(obj);


        }
        doc.SaveFile(path.c_str());
    }
};

class Task
{
public:
    bool complete = false;
    string text;
    string name;
    vector<string> strings;
    vector<int> ints;

    Task()
    {}
    Task( string t,  string n, vector<string> ss = {}, vector<int> is = {}) : text(t), name(n), strings(ss), ints(is)
    {

    }
};
vector<Task> main_tasks{Task("Cut down a tree","DESTROY",{"tree"}),Task("Talk to the shopkeeper","TALK",{"shopkeeper"}),Task("Plant 10 crops","PLACE",{"plant"})};
vector<Task> daily_tasks{Task("Eat an apple","EAT",{"apple"})};

class EventSystem : public Object
{
public:
    Task mainTask;
    Task dailyTask;

    EventSystem()
    {
        mainTask = main_tasks[main_progress];
        int randnumber = rand()%tasks.size();
        dailyTask = Task(tasks[randnumber],events[randnumber]);

    }
    void Event(string name, vector<string> strings = {}, vector<int> ints = {})
    {
        //cout << strings[0] << endl;
        if(!mainTask.complete && name == mainTask.name)
        {
            bool complete = true;
            for(int i = 0; i < std::min(mainTask.strings.size(),strings.size()); i++)
            {
                //printf("checking");
                //cout << mainTask.strings[i] << endl;

                if(!(mainTask.strings[i] == strings[i] || mainTask.strings[i]=="NEVERMIND"))
                    complete = false;
            }

            if(complete)
            {
                alerts.push("Main task complete!");
                main_progress++;
                if(main_progress < main_tasks.size())
                    mainTask = main_tasks[main_progress];
                else
                    mainTask.complete = true;

            }
        }

        if(!dailyTask.complete && name == dailyTask.name)
        {
            dailyTask.complete = true;
            alerts.push("Daily task complete!");

        }
    }
};

class HealthBar : public Entity
{
    public:
    NPC * n = NULL;
    bool active = false;
    int d = 5;
    int maxfade = 15;
    int fade = 15;
    HealthBar()
    {
        y = 0;
        h = d+FONTSIZE+d+TILESIZE/2+d;
        w = SCREEN_WIDTH/4;
        x = SCREEN_WIDTH/2-w/2;
    }
    void Add(NPC *n)
    {
        this->n = n;
        active = true;
        fade = maxfade;
    }

    int Update()
    {
        if(n && n->int_attribs["hp"]<=0)
        {
            n->int_attribs["hp"] = 0;
            if(fade > 1)
                fade--;
            else
                active = false;

        }
    }

    void Draw(float cX = 0, float cY = 0)
    {
        if(active && n)
        {
            float alph = (float)fade/maxfade;
            DrawImage(im("gui"),x,y,w,h, alph);
            DrawString(n->fullname,x+w/2-lineWidth(n->fullname)/2,d);
            DrawImage(im("dark"),x+d,y+d+FONTSIZE+d,w-2*d,TILESIZE/2,alph);
            DrawImage(im("gui2"),x+d,y+d+FONTSIZE+d,n->int_attribs["hp"]*(w-2*d)/n->int_attribs["health"],TILESIZE/2,alph);
        }
        else
        {
            active = false;
        }
    }
};
class TimeDisplay : public Entity
{
public:
    string displaystring;
    string timestring;
    string taskstring = "Main Task:";
    string dailystring = "Daily Task:";

    EventSystem * eventsys;
    Player * p;

    TimeDisplay(EventSystem * sys, Player * pl)
    {
        displaystring = "Year " + to_string(current_year) + ", " + seasons[current_season-1] + ", Day " + to_string(current_day);

        timestring = "Time:" + (current_hour < 10 ? string(" 0") : string(" ")) + to_string(current_hour) + (current_minute < 10 ? string(":0") : string(":")) + to_string(current_minute);

        w = displaystring.size()*FONTSIZE;
        h = FONTSIZE;
        y = 0;
        x = SCREEN_WIDTH-lineWidth(displaystring);

        eventsys = sys;
        p = pl;
    }

    int Update()
    {
        displaystring = "Year " + to_string(current_year) + ", " + seasons[current_season-1] + ", Day " + to_string(current_day);

        timestring = "Time:" + (current_hour < 10 ? string(" 0") : string(" ")) + to_string(current_hour) + (current_minute < 10 ? string(":0") : string(":")) + to_string(current_minute);

        w = displaystring.size()*FONTSIZE;
        h = FONTSIZE;
        y = 0;
        x = SCREEN_WIDTH-lineWidth(displaystring);

        return 1;
    }
    void DrawLine(string s, int &cY)
    {
        DrawImage(im("fade"),SCREEN_WIDTH-lineWidth(s),cY,lineWidth(s),FONTSIZE);
        DrawString(s,SCREEN_WIDTH-lineWidth(s),cY,-1,"font2");
        cY += FONTSIZE;
    }
    void Draw(float cX = 0, float cY = 0)
    {
        int pY = 0;

        DrawLine(displaystring,pY);
        //DrawImage(images[ids["fade"]],x,y,w,h);
        //DrawString(displaystring,x,y,-1,"font2");

        DrawLine(timestring,pY);
        //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(timestring),y+FONTSIZE,w,h);
        //DrawString(timestring,SCREEN_WIDTH-lineWidth(timestring),y+FONTSIZE,-1,"font2");

        if(p)
        DrawLine("Health: " + to_string(p->stat_levels["health"]),pY);

        DrawLine("Hunger: " + hunger_levels[(int)floor(hunger/HUNGER_SCALE)],pY);
        //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(hunger_levels[(int)floor(hunger/HUNGER_SCALE)]),y+FONTSIZE*2,lineWidth(hunger_levels[(int)floor(hunger/HUNGER_SCALE)]),FONTSIZE);
        //DrawString(hunger_levels[(int)floor(hunger/HUNGER_SCALE)],SCREEN_WIDTH-lineWidth(hunger_levels[(int)floor(hunger/HUNGER_SCALE)]),y+FONTSIZE*2,-1,"font2");


        DrawLine(taskstring,pY);
        //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(taskstring)-3,y+FONTSIZE*3,lineWidth(taskstring),FONTSIZE);
        //DrawString(taskstring,SCREEN_WIDTH-lineWidth(taskstring)-3,y+FONTSIZE*3,-1,"font2");

        if(!eventsys->mainTask.complete)
        {
            DrawLine(eventsys->mainTask.text,pY);
        //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(eventsys->mainTask.text)-3,y+FONTSIZE*4,lineWidth(eventsys->mainTask.text),FONTSIZE);
        //DrawString(eventsys->mainTask.text,SCREEN_WIDTH-lineWidth(eventsys->mainTask.text)-3,y+FONTSIZE*4,-1,"font2");

        }
        else
        {

        }

        DrawLine(dailystring,pY);
        //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(dailystring)-3,y+FONTSIZE*5,lineWidth(dailystring),FONTSIZE);
        //DrawString(dailystring,SCREEN_WIDTH-lineWidth(dailystring)-3,y+FONTSIZE*5,-1,"font2");

        if(!eventsys->dailyTask.complete)
        {
            DrawLine(eventsys->dailyTask.text,pY);
            //DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(eventsys->dailyTask.text)-3,y+FONTSIZE*6,lineWidth(eventsys->dailyTask.text),FONTSIZE);
            //DrawString(eventsys->dailyTask.text,SCREEN_WIDTH-lineWidth(eventsys->dailyTask.text)-3,y+FONTSIZE*6,-1,"font2");

        }
        else
        {

        }
        if(g("debug"))
        {
            string coordinates = to_string(camera_x+KeyData.MouseX)+ " " + to_string(camera_y+KeyData.MouseY);
            DrawImage(im("fade"),0,0,lineWidth(coordinates),FONTSIZE);
            DrawString(coordinates,0,0,-1,"font2");

        }

    }
};
class Dialog
{
public:
    image portrait;

    string speaker;
    vector<string> chain;
    bool hasQuestion = false;

    string question;
    vector<string> answers;
    vector<string> outcomes;
    vector<int> gotos;
    vector<string> conditions;

    Dialog(){};
    Dialog(string msg){
        speaker="narrator";
        question=msg;
        answers.push_back("OK");
        outcomes.push_back("");
        gotos.push_back(0);
        conditions.push_back("");
    };

};

Dialog Prompt(string msg, string script, string positive = "Yes", string negative = "No")
{
    Dialog d;
    d.speaker = "narrator";
    d.question=msg;
    d.answers.push_back(positive);
    d.answers.push_back(negative);
    d.outcomes.push_back(script);
    d.outcomes.push_back("");
    d.gotos.push_back(0);
    d.gotos.push_back(0);
    d.conditions.push_back("");
    d.conditions.push_back("");
    return d;
}

class DialogSystem : public Dialog, public Entity
{
public:
    int base_h;
    //int x,y,w,h;
    int portrait_size;
    int textLength, currentLength;
    int lineLength, ptSize;


    int borderw = 11;
    Dialog Current;
    int currentstring;
    string displaystring;
    int stringsize;
    int selected_answer = 0;
    bool finished_scroll = false;
    bool finished_chain = false;
    int namePtSize = FONTSIZE;
    int lines = 0;


    void calculateLines(string c)
    {
        std::stringstream ss;
        vector<string> words;
        ss << c;
        string newword;
        while(ss >> newword){words.push_back(newword);};
        int i = 0;
        int current_length = 0;
        lines = 1;
        while(i < words.size())
        {
            if(current_length + lineWidth(words[i] + " ",ptSize) + 1 > lineLength)
            {
                lines++;
                current_length = 0;
            }
            else
            {
                current_length += lineWidth(words[i] + " ",ptSize);
                i++;
            }
        }
    }
    bool ProcessCondition(string s)
    {
        if(s == "")
            return true;

        if(FirstWord(s) == "skill")
        {
            if(skills[SecondWord(s)]>=atoi(nWord(3,s)))
                return true;
        }

        return false;
    }
    void Add(Dialog _dialog, string default_sp = "narrator")
    {
        if(_dialog.speaker != "any")
            speaker = _dialog.speaker;
        else
            speaker = default_sp;

        chain = _dialog.chain;
        hasQuestion = _dialog.hasQuestion;
        question = _dialog.question;
        answers.clear();gotos.clear();outcomes.clear();

        if(npc_templates[speaker].attributes.count("sound"))
        {
            audioCommands.push("sound " + npc_templates[speaker].attributes["sound"]);
            cout << "adding sound\n";

        }
        for(int i = 0; i < _dialog.answers.size(); i++)
        if(ProcessCondition(_dialog.conditions[i]))
        {
            answers.push_back(_dialog.answers[i]);
            gotos.push_back(_dialog.gotos[i]);
            outcomes.push_back(_dialog.outcomes[i]);
        }
        //answers = _dialog.answers;
        //gotos = _dialog.gotos;
        //outcomes = _dialog.outcomes;

        if(chain.size()>0)
        {
            displaystring = chain[0];
            currentstring = 0;
            stringsize = 0;
            finished_scroll = false;
            finished_chain = false;
            selected_answer = 0;

        }
        else
        {
            displaystring = question;
            currentstring = 0;
            stringsize = 0;
            finished_chain = true;
            finished_scroll = false;
            selected_answer = 0;
        }

        namePtSize = FONTSIZE;
        while(lineWidth(npc_templates[speaker].fullname,namePtSize) > base_h-5)
        {
            namePtSize--;
        }

    }
    void Scroll()
    {
        if(!finished_chain)
        {
            if(stringsize < chain[currentstring].size())
            {
                stringsize++;
            }
            if(stringsize == chain[currentstring].size())
                finished_scroll = true;

        }
        else
        {
            if(stringsize < question.size())
            {
                stringsize++;
            }
            if(stringsize == question.size())
                finished_scroll = true;
        }
    }
    void Next()
    {
        currentstring++;
        if(currentstring < chain.size())
        {
            finished_scroll = false;
            displaystring = chain[currentstring];
            stringsize = 0;
        }
        else
        {
            finished_chain = true;
            displaystring = question;
            stringsize = 0;
            finished_scroll = false;
        }
    }
    void Draw()
    {
        if(finished_chain)
        {
            h = base_h + (answers.size()-1)*ptSize;
        }
        else
            h = base_h;

        y = SCREEN_HEIGHT-h;
        DrawImage(images[ids["dialog"]],x,y,w,h);

        if(speaker != "narrator")
        {
            DrawImage(images[ids["gui"]],x-base_h,y,base_h,base_h);
            DrawImage(portrait,x-base_h+borderw,y+borderw,base_h-borderw*2,base_h-borderw*2);
            DrawImage(images[ids["portraitframe"]],x-base_h,y,base_h,base_h);
            DrawString(npc_templates[speaker].fullname,x-base_h,y+base_h-namePtSize,namePtSize);
        }
        string drawstring;
        for(int i = 0; i < stringsize; i++)
            drawstring.push_back(displaystring[i]);

        DrawWrapped(drawstring,x+15,y+10, lineLength, ptSize);

        if(finished_chain && finished_scroll)
        {
            calculateLines(drawstring);

            DrawLetter('>',x+15-ptSize,y+15+lines*ptSize+selected_answer*(ptSize+5),ptSize);

            for(int i = 0; i < answers.size(); i++)
                DrawString(answers[i],x+15,y+15+lines*ptSize+i*(ptSize+5),ptSize);
        }
    }

    void Hover(int mx, int my)
    {
        if(finished_chain)
        if(my > y+15+lines*ptSize)
        {
            my -= y+15+lines*ptSize;
            selected_answer = (int)floor(my/(ptSize+5));
            if(selected_answer > answers.size()-1)
                selected_answer = answers.size()-1;
        }
    }
    DialogSystem()
    {
        w = globals["dialog-width"];
        h = base_h = globals["dialog-height"];
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT-h;
        lineLength = globals["dialog-line-length"];
        ptSize = FONTSIZE;//16;
    };

};

class GUI : public Entity
{
public:
    string type;

    GUI()
    {
        x=y=w=h=0;
    }

    virtual void Scroll(int dir)
    {

    }
    virtual void Draw(float cX = 0, float cY = 0)
    {

    }
    virtual Item *GetClick(int mX, int mY, int& id)
    {
        return NULL;
    }
    virtual int HandleClick(int mX, int mY)
    {
        return 0;
    }
    virtual int HandleRightClick(int mX, int mY)
    {
        return 0;
    }
    virtual int HandleMouseMove(int mX, int mY)
    {
        return 0;
    }
    virtual void Exit()
    {

    }
    virtual void SetPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
};

class NumberSelector : public GUI
{
public:
    int number=0,max=99;
    int def=0;
    NumberSelector()
    {

    }
    NumberSelector(int _x, int _y,int _max=99, int _default=1)
    {
        number=def=_default;
        max=_max;
        w = g("number-selector-w");
        h = g("number-selector-h");
        x = _x; y = _y;
    }
    void Reset()
    {
        number=def;
    }
    int Change(int amount)
    {
        if(number+amount>0)
            number += amount;
        else if(number+amount<=0)
            number = max-amount-1;

        if(number>max)
            number-=max;
    }
    int HandleClick(int mX, int mY)
    {
        mX -= x; mY -= y;
        if(mX > w-TILESIZE)
        {
            Change((mY>h/2?-1:1));
        }
    }
    void Draw(float cX=0, float cY=0)
    {
        DrawImage(im("gui2"),x,y,w,h);

        DrawImage(im("itemtile"),x+w-TILESIZE,y,TILESIZE,h/2);
        DrawStringCentered("+",x+w-TILESIZE/2,y,10);
        DrawImage(im("itemtile"),x+w-TILESIZE,y+h/2,TILESIZE,h/2);
        DrawStringCentered("-",x+w-TILESIZE/2,y+h/2,10);

        DrawString(to_string(number),x,y+h/2-FONTSIZE/2);

    }
};
class GUIElemContainer : public GUI
{
    public:
    vector<GUI*> GUIElems;

    void Scroll(int dir)
    {
        for(auto e : GUIElems)
            e->Scroll(dir);
    }
    void ArrangeElems()
    {
        int pX = x;
        for(auto e : GUIElems)
        {
            e->SetPosition(pX,y);
            pX += e->w;
            e->h = h;

        }
    }
    void SetPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
        ArrangeElems();
    }
    void AddElem(GUI* elem)
    {
        GUIElems.push_back(elem);
        w += elem->w;
        if(elem->h > h)
            h = elem->h;
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;

        ArrangeElems();
    }
    void Draw(float cX = 0, float cY = 0)
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

    int HandleClick(int mX, int mY)
    {
        for(auto g : GUIElems)
        {
            if(Contains(g,mX,mY,0,0))
            {
                g->HandleClick(mX,mY);
            }
        }
    }

    void Exit()
    {
        for(auto g : GUIElems)
        {
            g->Exit();
        }
    }

};

class GUIRowContainer : public GUI
{
public:

    vector<GUI*> rows;

    void Scroll(int dir)
    {
        for(auto e : rows)
            e->Scroll(dir);
    }
    void ArrangeRows()
    {
        int pY = y;
        for(auto e : rows)
        {
            e->SetPosition(x,pY);
            pY += e->h;
            e->w = w;

        }
    }
    void SetPosition(int x, int y)
    {
        this->x = x;
        this->y = y;
        ArrangeRows();
    }
    void AddElem(GUI* elem)
    {
        rows.push_back(elem);
        h += elem->h;
        if(elem->w > w)
            w = elem->w;
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;

        ArrangeRows();
    }
    void Draw(float cX = 0, float cY = 0)
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

    int HandleClick(int mX, int mY)
    {
        for(auto g : rows)
        {
            if(Contains(g,mX,mY,0,0))
            {
                g->HandleClick(mX,mY);
            }
        }
    }
    void Exit()
    {
        for(auto g : rows)
        {
            g->Exit();
        }
    }
};

class GUITabContainer : public GUI
{
public:
    vector<GUI*> tabs;
    int activeTab = 0;
    int toolbarHeight = TILESIZE;


    void Scroll(int dir)
    {
        tabs[activeTab]->Scroll(dir);
    }
    void AddElem(GUI * elem)
    {
        tabs.push_back(elem);
        w = elem->w;
        h = elem->h + toolbarHeight;
        elem->SetPosition(SCREEN_WIDTH/2-w/2,SCREEN_HEIGHT/2-h/2+toolbarHeight);
    }
    GUITabContainer()
    {

    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        for(int i = 0; i < tabs.size(); i++)
        {
            DrawImage(im("itemtile"),x+i*toolbarHeight,y,toolbarHeight,toolbarHeight);
        }
        if(tabs.size() > activeTab)
        {
            tabs[activeTab]->Draw();
        }
    }

    void SwitchTab(int i)
    {
        activeTab = i;
        w = tabs[i]->w;
        h = tabs[i]->h + toolbarHeight;
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
    }
    int HandleClick(int mX, int mY)
    {
        if(mY-y < toolbarHeight)
        {
            if(mX-x < toolbarHeight * tabs.size())
            {
                SwitchTab(floor((mX-x)/toolbarHeight));
            }
        }
        else
        {
            tabs[activeTab]->HandleClick(mX,mY);

        }
    }
    void Exit()
    {
        for(auto g : tabs)
        {
            g->Exit();
        }
    }
};

class GUIInventoryItems : public GUI
{
  public:
        Inventory * inv;
        int frame = TILESIZE/2;
        bool holdingItem = false;
        Item * hold;

        GUIInventoryItems()
        {

        }
        GUIInventoryItems(Inventory * _inv)
        {
            type = "inventory";
            inv = _inv;
            w = 10*TILESIZE + 2*frame;
            h = 4*TILESIZE + 2*frame + TILESIZE + frame;
            x = SCREEN_WIDTH/2 - w/2;
            y = SCREEN_HEIGHT/2 - h/2;

        }
        void Draw(float cX = 0, float cY = 0)
        {
            DrawImage(images[ids["gui"]],x,y,w,h);
            //DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 10; j++)
                {
                    DrawImage(images[ids["itemtile"]],x+frame+j*TILESIZE,y+frame+i*TILESIZE,TILESIZE,TILESIZE);
                    if(inv->items[i*10+j])
                    {
                        DrawImage(inv->items[i*10+j]->img,x+frame+j*TILESIZE,y+frame+i*TILESIZE,TILESIZE,TILESIZE);
                        if(inv->items[i*10+j]->quantity > 1)
                            DrawString(to_string(inv->items[i*10+j]->quantity),x+frame+j*TILESIZE+TILESIZE-(inv->items[i*10+j]->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,y+frame+i*TILESIZE+TILESIZE-FONTSIZE,-1,"font2");
                    }


                }

            for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
            {
                DrawImage(images[ids["itemtile"]],x+frame+i*TILESIZE,y+h-frame-TILESIZE,TILESIZE,TILESIZE);
                if(inv->toolbar[i])
                {
                    DrawImage(inv->toolbar[i]->img,x+frame+i*TILESIZE,y+h-frame-TILESIZE,TILESIZE,TILESIZE);
                    if(inv->toolbar[i]->quantity > 1)
                        DrawString(to_string(inv->toolbar[i]->quantity),x+frame+i*TILESIZE+TILESIZE-(inv->toolbar[i]->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,y+h-frame-FONTSIZE,-1,"font2");

                }
            }

            if(holdingItem)
                if(hold)
                {
                    DrawImage(hold->img,KeyData.MouseX,KeyData.MouseY,TILESIZE,TILESIZE);
                    if(hold->quantity > 1)
                        DrawString(to_string(hold->quantity),KeyData.MouseX+TILESIZE-(hold->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,KeyData.MouseY+TILESIZE-FONTSIZE,-1,"font2");

                }


        }

        void Exit()
        {
            if(holdingItem)
            if(hold)
            {
                int q = hold->quantity;
                hold->quantity = 1;
                inv->Add(*hold,q);
                hold = NULL;
            }
        }
        Item* GetClick(int mX, int mY)
        {
            if(mX >= x+frame && mX < x+w-frame && mY >= y+frame && mY < y+h-frame-TILESIZE-frame)
            {
                int item_i = (mX-x-frame)/TILESIZE;
                int item_j = (mY-y-frame)/TILESIZE;
                if(inv->items[item_j*10+item_i])
                {
                    return inv->items[item_j*10+item_i];


                }

            }
            if(mX >= x+frame && mX < x+w-frame && mY >= y+h-frame-TILESIZE && mY < y+h-frame)
            {
                int item_i = (mX-x-frame)/TILESIZE;
                if(inv->toolbar[item_i])
                {
                    return inv->toolbar[item_i];
                }

            }

            return NULL;
        }
        int HandleClick(int mX, int mY)
        {
            if(!holdingItem)
            {
                if(mX >= x+frame && mX < x+w-frame && mY >= y+frame && mY < y+h-frame-TILESIZE-frame)
                {
                    int item_i = (mX-x-frame)/TILESIZE;
                    int item_j = (mY-y-frame)/TILESIZE;
                    if(inv->items[item_j*10+item_i])
                    {
                        hold = inv->items[item_j*10+item_i];
                        holdingItem = true;
                        inv->items[item_j*10+item_i] = NULL;


                    }
                }
                if(mX >= x+frame && mX < x+w-frame && mY >= y+h-frame-TILESIZE && mY < y+h-frame)
                {
                    int item_i = (mX-x-frame)/TILESIZE;
                    if(inv->toolbar[item_i])
                    {
                        hold = inv->toolbar[item_i];
                        holdingItem = true;
                        inv->toolbar[item_i] = NULL;


                    }
                }
            }
            else
            {
                if(mX >= x+frame && mX < x+w-frame && mY >= y+frame && mY < y+h-frame-TILESIZE-frame)
                {
                    int item_i = (mX-x-frame)/TILESIZE;
                    int item_j = (mY-y-frame)/TILESIZE;
                    if(!inv->items[item_j*10+item_i])
                    {
                        inv->items[item_j*10+item_i] = hold;
                        hold = NULL;
                        holdingItem = false;
                    }
                    else
                    {
                        Item* temp = hold;
                        hold = inv->items[item_j*10+item_i];
                        inv->items[item_j*10+item_i] = temp;
                    }
                }
                if(mX >= x+frame && mX < x+w-frame && mY >= y+h-frame-TILESIZE && mY < y+h-frame)
                {
                    int item_i = (mX-x-frame)/TILESIZE;
                    if(!inv->toolbar[item_i])
                    {
                        inv->toolbar[item_i] = hold;
                        hold = NULL;
                        holdingItem = false;


                    }
                    else
                    {
                        Item* temp = hold;
                        hold = inv->toolbar[item_i];
                        inv->toolbar[item_i] = temp;
                    }

                }
            }
        }
};

class GUIBuyItems : public GUI
{
public:

    vector<Item> item_list;
    Inventory * inv;
    int scrollPosition = 0;

    void Scroll(int dir)
    {
        if(item_list.size()>g("shop-shown-items"))
        {
            if(scrollPosition + dir >= 0 && scrollPosition+dir <= item_list.size()-g("shop-shown-items"))
                scrollPosition += dir;
        }
    }
    GUIBuyItems(Inventory * inv, string store_inventory = "general-store-inventory")
    {

        for(auto& e : allWord(s(store_inventory)))
            item_list.push_back(item_templates[e]);

        type = "shop";
        w = 400;
        h = std::min(g("shop-shown-items")*TILESIZE,(int)item_list.size()*TILESIZE+TILESIZE);
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
        this->inv = inv;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        int i = 0;
        for(int ii = scrollPosition; ii< scrollPosition+std::min(g("shop-shown-items"),(int)item_list.size()); ii++)
        {
            Item e = item_list[ii];
            i = ii-scrollPosition;
            DrawSeparator(x,y+TILESIZE+i*TILESIZE,w,TILESIZE);
            DrawImage(e.img,x,y+TILESIZE+i*TILESIZE,TILESIZE,TILESIZE);
            DrawString(e.formal,x+TILESIZE,y+TILESIZE+i*TILESIZE);
            string pricestring = to_string(e.int_attribs["price"])+"" + CURRENCY;
            DrawString(pricestring,x+w-lineWidth(pricestring)-FONTSIZE,y+TILESIZE+i*TILESIZE);

        }

        if(item_list.size()>g("shop-shown-items"))
        {
            DrawImage(im("itemtile"),x+w-5,y,5,h);
            int nodeh = g("shop-shown-items")/item_list.size()*h;

            DrawImage(im("button"),x+w-5,y+(float)scrollPosition*(nodeh)/item_list.size(),5,5);
        }
    }
    int HandleClick(int mX, int mY)
    {
        int clickY = mY - y - TILESIZE;
        if(clickY < 0)
            return NULL;
        clickY /= TILESIZE;
        clickY += scrollPosition;

        if(clickY < item_list.size())
        {
            if(item_list[clickY].int_attribs["price"] <= funds)
            {
                funds -= item_list[clickY].int_attribs["price"];
                inv->Add(item_list[clickY]);
                return 1;
            }
        }

        return 0;
    }
    Item *GetClick(int mX, int mY, int& id)
    {
        int clickY = mY - y - TILESIZE;
        if(clickY < 0)
            return NULL;
        clickY /= TILESIZE;

        if(clickY < item_list.size())
            return &item_list[clickY];

        return NULL;
    }
};

class GUIBuyScreen : public GUIRowContainer
{
public:
    GUIBuyItems * buyItems;
    GUIInventoryItems * inventoryItems;

    GUIBuyScreen(Inventory * inv, string shop_inventory)
    {
        buyItems = new GUIBuyItems(inv,shop_inventory);
        inventoryItems = new GUIInventoryItems(inv);

        AddElem(buyItems);
        AddElem(inventoryItems);
    }

    void Sell(Item * item)
    {
        if(item->int_attribs.count("sell"))
            funds += item->int_attribs["sell"]*item->quantity;
        inventoryItems->inv->Remove(item->name,item->quantity);
    }
    int HandleClick(int mX, int mY)
    {
        if(Contains(inventoryItems,mX,mY,0,0))
        {
            Item * clickItem = inventoryItems->GetClick(mX,mY);
            if(clickItem)
                Sell(clickItem);
        }
        else if(Contains(buyItems,mX,mY,0,0))
        {
            buyItems->HandleClick(mX,mY);
        }
    }
};
class GUISellItems : public GUI
{
public:
    Inventory * inv;
    GUISellItems(){};
    GUISellItems(Inventory * _inv)
    {
        type = "sell";
        inv = _inv;
        w = INVENTORY_TOOLBAR_SIZE*TILESIZE;
        h = 2*TILESIZE;
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        DrawString("funds: " + to_string(funds) + " " + CURRENCY,x,y);
        //DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        for(int i = 0; i < INVENTORY_TOOLBAR_SIZE; i++)
        {
            DrawImage(images[ids["itemtile"]],x+i*TILESIZE,y+h-TILESIZE,TILESIZE,TILESIZE);
            if(inv->toolbar[i])
            {
                DrawImage(inv->toolbar[i]->img,x+i*TILESIZE,y+h-TILESIZE,TILESIZE,TILESIZE);
                if(inv->toolbar[i]->quantity > 1)
                    DrawString(to_string(inv->toolbar[i]->quantity),x+i*TILESIZE+TILESIZE-(inv->toolbar[i]->quantity >= 10 ? 2 : 1 ) * DIGITWIDTH,y+h-FONTSIZE,-1,"font2");
            }
        }
    }
    Item *GetClick(int mX, int mY, int& id)
    {
        int clickX = mX - x;
        int clickY = mY - y;
        clickX /= TILESIZE;

        if(clickY > TILESIZE)
        {
            id = clickX;
            return inv->toolbar[clickX];

        }
        int i;
        return NULL;
    }
};

class GUISellScreen : public GUIRowContainer
{
  public:

    GUISellItems * sellItems;
    GUIInventoryItems * items;

    bool holdingItem = false;
    Item selectedItem;

    GUISellScreen(Inventory * inv)
    {
        sellItems = new GUISellItems(inv);
        items = new GUIInventoryItems(inv);

        AddElem(sellItems);
        AddElem(items);
    }

    void SwapWithSelected(int id)
    {
    }
    int HandleClick(int mX, int mY)
    {
        if(Contains(items,mX,mY,0,0))
        {
            int id;
            if(items->GetClick(mX,mY))
            {
            }
        }
        else
        {

        }
    }
};
class GUIElemSkills : public GUI
{
    public:
    vector<string> skill_names = {"Skills"};
    vector<string> skill_levels = {""};
    int maxwidth = 0;

    GUIElemSkills(int _x, int _y)
    {
        w = 2*globals["gui-border"]+globals["gui-width"]*3/4;
        h = 2*globals["gui-border"]+FONTSIZE;

        for(auto skill : skills)
        {
            h += FONTSIZE + globals["gui-margin"];
            skill_names.push_back(skill.first);
            skill_levels.push_back(to_string(skill.second));
            if(lineWidth(skill.first + ":") > maxwidth)
                maxwidth = lineWidth(skill.first + ":");

        }
        x = _x;
        y = _y;

    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        int pY = y+globals["gui-border"];//y+h/2-skill_names.size()*(FONTSIZE+globals["gui-margin"]);
        for(int i = 0; i < skill_names.size(); i++)
        {
            DrawString(skill_names[i]+":",x+globals["gui-margin"],pY);
            if(i > 0)
            {

                int maximumw = x+w-globals["gui-margin"]-FONTSIZE-globals["gui-margin"]-x-maxwidth-globals["gui-margin"];
                DrawImage(images[ids["fade"]],x+maxwidth+globals["gui-margin"],pY,maximumw,FONTSIZE);
                DrawImage(images[ids["dark"]],x+maxwidth+globals["gui-margin"],pY,xps[skill_names[i]]*maximumw/levelcap(skills[skill_names[i]]),FONTSIZE);
                DrawString(skill_levels[i],x+w-globals["gui-margin"]-FONTSIZE,pY);

            }
            pY += FONTSIZE+globals["gui-margin"];
        }
    }
};

class GUIShopScreen : public GUITabContainer
{
public:

    GUIShopScreen(Inventory * inv, string shop_inventory) : GUITabContainer()
    {
        type = "shop";

        AddElem(new GUIBuyScreen(inv, shop_inventory));
        AddElem(new GUISellScreen(inv));

        SwitchTab(0);
    }
};

class GUIToolSelect : public GUI
{

public:
    string * tool;

    GUIToolSelect(string * tool)
    {
        x = y = 0;
        w = 2*TILESIZE;
        h = 2*TILESIZE;
        this->tool = tool;
    }
    int HandleClick(int mX, int mY)
    {
        mX -= x;
        mY -= y;
        if(mY < TILESIZE)
        {
            if(mX < TILESIZE)
                *tool = "terraform";
            else
                *tool = "object";
        }
        else
        {
            if(mX < TILESIZE)
                *tool = "delete";
        }

    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        DrawImage(im("itemtile"),x,y,TILESIZE,TILESIZE);
        DrawImage(im("hoe"),x,y,TILESIZE,TILESIZE);
        DrawImage(im("itemtile"),x+TILESIZE,y,TILESIZE,TILESIZE);
        DrawImage(im("chair"),x+TILESIZE,y,TILESIZE,TILESIZE);
        DrawImage(im("itemtile"),x,y+TILESIZE,TILESIZE,TILESIZE);
        DrawImage(im("staff-red"),x,y+TILESIZE,TILESIZE,TILESIZE);
    }
};

class GUISaveLoadEditor : public GUI
{
public:
    string * commands;

    GUISaveLoadEditor(string * commands)
    {
        this->commands = commands;
        x=y=0;
        w = 2 * TILESIZE;
        h = TILESIZE;

    }
    int HandleClick(int mX, int mY)
    {
        mX -= x;
        mY -= y;
        if(mX < TILESIZE)
        {
            *commands = "save";
        }
        else
        {
            *commands = "load";
        }
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        DrawImage(im("coffee"),x,y,TILESIZE,TILESIZE);
        DrawImage(im("chest"),x+TILESIZE,y,TILESIZE,TILESIZE);
    }

};
class GUIEditorInterface : public GUIRowContainer
{
public:
    NumberSelector * widthSetter,  *heightSetter;
    Stage * targetStage;


    GUIEditorInterface(Stage * targetStage, string * tool, string * commands)
    {
        x = y = w = h = 0;
        w = 0;

        this->targetStage = targetStage;

        widthSetter = new NumberSelector(0,0,100,10);
        heightSetter = new NumberSelector(0,0,100,10);

        AddElem(widthSetter);
        AddElem(heightSetter);
        AddElem(new GUIToolSelect(tool));
        AddElem(new GUISaveLoadEditor(commands));

        x = SCREEN_WIDTH-w;

        for(auto g : rows)
            g->x = SCREEN_WIDTH-w;

    }

    int Update()
    {
        targetStage->Resize(widthSetter->number,heightSetter->number);
    }
};
class GUIElemEquips : public GUI
{
    public:
    GUIElemEquips(int _x, int _y)
    {
        x = _x;
        y = _y;
        w = globals["gui-width"]/4;
        h = 2*globals["gui-border"]+FONTSIZE+skills.size()*(FONTSIZE + globals["gui-margin"]);
    }
    void DrawSlot(int &pY, string str)
    {
        DrawString(str,x+w/2-lineWidth(str)/2,pY);
        pY+=FONTSIZE;
        DrawImage(im("itemtile"),x+w/2-TILESIZE/2,pY,TILESIZE,TILESIZE);
        pY+=TILESIZE;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        int pY = y+g("gui-border");
        DrawSlot(pY,"Hat");
        DrawSlot(pY,"Ring");
        DrawSlot(pY,"Shoes");
    }
};

class GUIElemStats : public GUI
{
    public:
    vector<string> lines;
    vector<int> modifications;

    int spacing = 5;
    Player * pl;

    GUIElemStats(Player * player)
    {
        w = 20;
        h = 20;
        pl = player;
        lines = allWord(s("player-stats"));
        h = lines.size()*(FONTSIZE+spacing);
        x = 0;
        y = SCREEN_HEIGHT/2-h/2;
        for(auto &l : lines)
        {
            if(lineWidth(Capitalize(l))>w)
                w = lineWidth(Capitalize(l));

            int currentModification = 0;
            for(int i = 0; i < pl->modifiers.size(); i++)
            {
                if(pl->modifiers[i].stat == l)
                    currentModification += pl->modifiers[i].points;
            }
            modifications.push_back(currentModification);
        }
        w+=5 + TILESIZE*3;

    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);
        int pY = y+h/2-lines.size()*(FONTSIZE+spacing)/2.0;
        for(int i = 0; i < lines.size(); i++)
        {
            string l = lines[i];
            DrawString(Capitalize(l)+":",x,pY);
            string statNumberString = to_string(pl->stat_levels[l])+(modifications[i]>0?"+"+to_string(modifications[i]):"")+(modifications[i]<0?"-"+to_string(-modifications[i]):"");
            DrawString(statNumberString,x+w-lineWidth(statNumberString)-TILESIZE/2,pY);
            pY += FONTSIZE+spacing;
        }
    }
};
class GUICharacterScreen : public GUIRowContainer
{
    public:
    GUIElemSkills *skills;
    GUIElemEquips *equips;
    Player * pl;

    GUICharacterScreen(Player * player)
    {
        pl = player;
        w = 0;//2*globals["gui-border"]+1.5*globals["gui-width"];
        h = 2*globals["gui-border"]+FONTSIZE;
        skills = new GUIElemSkills(SCREEN_WIDTH/2-w/2+globals["gui-width"]/2,SCREEN_HEIGHT/2-h/2);
        equips = new GUIElemEquips(SCREEN_WIDTH/2-w/2,SCREEN_HEIGHT/2-h/2);

        AddElem(equips);
        AddElem(skills);
        AddElem(new GUIElemStats(pl));

    }



};

class GUIItemDescription : public GUI
{
public:
    string focus = "";

    GUIItemDescription()
    {
        w = 300;
        h = 200;
    }

    void OnNewItemSelected(string new_item)
    {
        focus = new_item;


    }
    void DeselectItem()
    {
        focus = "";
    }

    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(im("gui"),x,y,w,h);

        DrawCentered(im("itemtile"),x+w/2,y+h/4,TILESIZE,TILESIZE);
        DrawCentered(im("gui2"),x+w/2,y+2*h/3,w*3/4,h/2);
        if(focus != "")
        {
            DrawCentered(item_templates[focus].img,x+w/2,y+h/4,TILESIZE,TILESIZE);
            DrawStringCentered(item_templates[focus].formal,x+w/2,y+h/4-TILESIZE);
            if(item_templates[focus].desc != "")
                DrawWrapped(item_templates[focus].desc,x+w/2-w*3/8,y+2*h/3-h/4,w*3/4);
        }
    }
};
class GUIInventoryScreen : public GUIElemContainer
{
    public:

    GUIInventoryItems *inv_items;
    GUIItemDescription *item_description;
    Inventory * inv;

    GUIInventoryScreen()
    {

    }
    GUIInventoryScreen(Inventory * inv)
    {
        w = 0;
        h = 0;
        type = "inventory";
        inv_items = new GUIInventoryItems(inv);
        item_description = new GUIItemDescription();
        this->inv = inv;

        AddElem(inv_items);
        AddElem(item_description);
    }

    int HandleMouseMove(int mX, int mY)
    {
        if(Contains(inv_items,mX,mY,0,0))
        {
            if(inv_items->GetClick(mX,mY))
            {
                item_description->OnNewItemSelected(inv_items->GetClick(mX,mY)->name);
            }
            else
                item_description->DeselectItem();
        }
    }

};
class GUIEscapeScreen : public GUI
{
public:
    GUIEscapeScreen()
    {

    }
    void Draw(float cX = 0, float cY = 0)
    {

    }
};
class GUICraftingScreen : public GUI
{
public:
    Inventory * inv;
    NumberSelector numSel;
    vector<CraftingRecipe> recipe_list;
    string medium;
    int selected = 0;
    bool can_craft = false;

    int bx,by,bw,bh;

    GUICraftingScreen()
    {

    }
    GUICraftingScreen(Inventory * _inv, string _medium)
    {
        type = "crafting";
        inv = _inv;
        medium = _medium;

        for(auto& e : recipes)
        {
            if(e.second.medium == medium)
                if(skills[e.second.skill] >= e.second.level)
                    if(item_templates.count(e.second.item))
                        recipe_list.push_back(e.second);
        }
        w = 500;
        h = 300;//recipe_list.size() * TILESIZE;
        x = SCREEN_WIDTH/2 - w/2;
        y = SCREEN_HEIGHT/2 - h/2;

        bw = 75;
        bh = 50;
        bx = x + w*3/4 - bw/2;
        by = y + h*3/4 - bh/2;

        numSel = NumberSelector(x+w*3/4-g("number-selector-w")/2,y+h/2);

        TestCraftability();
        RefreshSize();
    }
    void RefreshSize()
    {
        w = 500;
        h = 300;
        numSel.y = y+h/2;
        by = y + h*3/4 - bh/2;
        if(selected < recipe_list.size())
        {
            int slide = linesCount(item_templates[recipe_list[selected].item].desc, w/2-TILESIZE*2)*FONTSIZE - TILESIZE;
            if(slide > 0)
            {
                h+= slide;
                numSel.y += slide;
                by += slide;

            }


        }

    }
    void Scroll(int dir)
    {
        selected += dir;
        if(selected < 0)
            selected = recipe_list.size()-1;
        else if(selected >= recipe_list.size())
            selected = 0;

        RefreshSize();
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        //DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        for(int i = 0;  i < recipe_list.size(); i++)
        {
            DrawImage(images[ids["itemtile"]],x,y+i*TILESIZE,TILESIZE,TILESIZE);
            if(item_templates.count(recipe_list[i].item))
                DrawImage(item_templates[recipe_list[i].item].img,x,y+i*TILESIZE,TILESIZE,TILESIZE);
            if(i == selected)
                //DrawImage(images[ids["frame"]],x,y+i*TILESIZE,TILESIZE,TILESIZE);
                DrawSeparator(x,y+i*TILESIZE,w/2,TILESIZE);

            DrawString(item_templates[recipe_list[i].item].formal,x+TILESIZE,y+i*TILESIZE);
        }

        if(selected < recipe_list.size())
        {
            int width = (2 + recipe_list[selected].ingredients.size())*TILESIZE;

            DrawImage(images[ids["itemtile"]],x + w*3/4 - width/2,y+TILESIZE,TILESIZE,TILESIZE);
            if(item_templates.count(recipe_list[selected].item))
                DrawImage(item_templates[recipe_list[selected].item].img,x + w*3/4 -width/2,y+TILESIZE,TILESIZE,TILESIZE);
            if(recipe_list[selected].count > 1)
                DrawString(to_string(recipe_list[selected].count),x + w*3/4 - width/2 + TILESIZE - (recipe_list[selected].count > 9 ? 2 : 1) * DIGITWIDTH,y+TILESIZE+TILESIZE-FONTSIZE,-1,"font2");

            for(int i = 0 ; i < recipe_list[selected].ingredients.size(); i++)
            {
                DrawImage(images[ids["itemtile"]],x + w*3/4 - width/2 + (2+i)*TILESIZE,y+TILESIZE,TILESIZE,TILESIZE);
                if(item_templates.count(recipe_list[selected].ingredients[i]))
                    DrawImage(item_templates[recipe_list[selected].ingredients[i]].img,x + w*3/4 - width/2 + (2+i)*TILESIZE,y+TILESIZE,TILESIZE,TILESIZE);
                if(recipe_list[selected].counts[i] > 1)
                    DrawString(to_string(recipe_list[selected].counts[i]),x + w*3/4 - width/2 + (2+i)*TILESIZE + TILESIZE - (recipe_list[selected].counts[i] > 9 ? 2 : 1) * DIGITWIDTH,y+TILESIZE+TILESIZE-FONTSIZE,-1,"font2");

            }

            DrawImage(im("gui2"),x + w/2+TILESIZE,y+TILESIZE*3-5, w/2-TILESIZE*2,linesCount(item_templates[recipe_list[selected].item].desc,w/2-TILESIZE*2)*FONTSIZE);
            DrawWrapped(item_templates[recipe_list[selected].item].desc, x + w/2 + TILESIZE, y + TILESIZE*3 - 5, w/2-TILESIZE*2);

            numSel.Draw();
            //if(can_craft)
                DrawImage(im("create"),bx,by,bw,bh);
            if(!can_craft)
                DrawImage(im("fade"),bx,by,bw,bh,0.9);
        }
    }

    void TestCraftability()
    {
        can_craft = true;
        for(int i = 0; i < recipe_list[selected].ingredients.size(); i++)
        {
            if(inv->Count(recipe_list[selected].ingredients[i]) < numSel.number*recipe_list[selected].counts[i])
            {
                //cout << inv->Count(recipe_list[selected].ingredients[i]) << " " << recipe_list[selected].counts[i] << endl;
                can_craft = false;
            }
        }

    }
    int HandleClick(int mX, int mY)
    {
        if(Contains(&numSel,mX,mY,0,0))
        {
            numSel.HandleClick(mX,mY);
            TestCraftability();

        }
        if(mX < x + w/2)
        {
            if((mY-y)/TILESIZE < recipe_list.size())
            {
                if(selected != (mY-y)/TILESIZE)
                {
                    selected = (mY-y)/TILESIZE;
                    numSel.Reset();
                    TestCraftability();
                    RefreshSize();
                }

            }

            return 0;
        }
        else if(mX >= bx && mX <= bx + bw && mY >= by && mY <= by + bh)
        {

            if(can_craft)
                if(item_templates.count(recipe_list[selected].item))
                {
                    for(int j = 0; j < numSel.number; j++)
                        for(int i = 0; i < recipe_list[selected].ingredients.size(); i++)
                            inv->Remove(recipe_list[selected].ingredients[i],recipe_list[selected].counts[i]);

                    inv->Add(item_templates[recipe_list[selected].item], numSel.number*recipe_list[selected].count);

                    xps[recipe_list[selected].skill] += numSel.number*CRAFTING_XP;
                    while(xps[recipe_list[selected].skill] >= levelcap(skills[recipe_list[selected].skill]))
                    {
                        xps[recipe_list[selected].skill] -= levelcap(skills[recipe_list[selected].skill]);
                        skills[recipe_list[selected].skill]++;
                        alerts.push("LEVELUP! " + recipe_list[selected].skill + " lvl " + to_string(skills[recipe_list[selected].skill]));

                    }

                    TestCraftability();

                    return 1;
                }


            return 0;
        }
    }
};

#endif // OBJECTS_H
