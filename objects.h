#pragma once
#include "public.h"
#include "keydata.h"

class Object
{
public:
    virtual int Update()
    {
        return 1;
    }
};

class Visible : public Object
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
bool Contains(Entity * e, int mX, int mY, int cx, int cy)
{
    return (mX+cx >= e->x-e->offset/2 && mX+cx <= e->x + e->w + e->offset/2 &&
       mY+cy >= e->y - e->alt && mY+cy <= e->y + e->h);

}
/*bool PixelPerfect(Entity * ent1, Entity * ent2)
{
    printf("checking for collisions\n");
    int lowTop, highBottom, highLeft, lowRight;

    if (ent1->y < ent2->y) lowTop = ent2->y;
    else lowTop = ent1->y;

    if (ent1->y + ent1->h<ent2->y + ent2->h) highBottom = ent1->y + ent1->h;
    else highBottom  = ent2->y + ent2->h;

    if (ent1->x>ent2->x) highLeft = ent1->x;
    else highLeft = ent2->x;

    if (ent1->x + ent1->w>ent2->x + ent2->w) lowRight = ent2->x + ent2->w;
    else lowRight = ent1->x + ent1->w;
    cout << highBottom - ent2->y << " " << lowTop - ent2->y << endl;
    //if(alpha_maps[ids["bush"]].alpha[(int)ent1->w/2][(int)ent1->h/2])
    //    cout << "alpha1" << endl;

    for (int h = highLeft; h <= lowRight; h++) { //horizontal
	for (int v = lowTop; v <= highBottom; v++) { //vertical
		//Do Tests
		if(v-(int)ent1->y >= 0 && v-(int)ent1->y < 32 &&
        v-(int)ent2->y >= 0 && v-(int)ent2->y < 32)
		if(alpha_maps[std::distance(images.begin(),std::find(images.begin(),images.end(),ent1->img))].alpha[h-(int)ent1->x][v-(int)ent1->y]
     &&    alpha_maps[std::distance(images.begin(),std::find(images.begin(),images.end(),ent2->img))].alpha[h-(int)ent2->x][v-(int)ent2->y])
     //if(alpha_maps[ids["bush"]].alpha[h-(int)ent1->x][v-(int)ent1->y]
     //&&    alpha_maps[ids["human"]].alpha[h-(int)ent2->x][v-(int)ent2->y])
        {
            printf("collision detected!\n");
            return true;
        }
	}
    printf("none found\n");

	return false;
}
}*/
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
    float movement_speed;
    vector<string> stats;
    map<string, int> stat_levels;

    Creature()
    {
        movement_speed=1;
    }

    Creature(image _img, float _x, float _y, float _w, float _h, float _alt = 0) : Entity(_img,_x,_y,_w,_h)
    {
        movement_speed=3;
        alt = _alt;
    }

    int Update()
    {
        /*if(abs(x-destx)+abs(y-desty) < movement_speed)
        {
            x = destx;
            y = desty;
        }
        if(x < destx)
        {
            x += movement_speed;
        }
        else if(x > destx)
        {
            x -= movement_speed;
        }
        else if(y < desty)
        {
            y += movement_speed;
        }
        else if(y > desty)
        {
            y -= movement_speed;
        }
        */
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
                    DrawClip(images[ids["tileset"]],i*TILESIZE-cX,j*TILESIZE-cY,TILESIZE,TILESIZE,tile[i][j]*TILESIZE,0,TILESIZE,TILESIZE,g("tiles")*TILESIZE,TILESIZE);
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

    void Add(Entity e)
    {
        particles.push_back(Entity(e.img,e.x*2,e.y*2,e.w*2,e.h*2));
        particles[particles.size()-1].int_attribs["speed"]=1;
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
    ParticleSystem * ps = NULL;
    bool active = false;
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
    int Update()
    {
        if(active && weather!="none")
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
        if(active && weather != "none")
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

class Stage : public Object
{
public:
    bool indoors = false;
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
    void Forest()
    {
        for(int i = 0; i < 100; i++)
            for(int j = 0; j < 100; j++)
        {
            tilemap.tile[i][j]=0;
        }
        for(int i = 0; i < 30; i++)
        {
            objects.push_back(new Entity(object_templates["tree"],(rand()%((SCREEN_WIDTH-TILESIZE)/TILESIZE))*TILESIZE+TILESIZE, (rand()%((SCREEN_HEIGHT-TILESIZE)/TILESIZE))*TILESIZE+TILESIZE));
        }

        objects.push_back(new Entity(Door("lot",4,8,16,2)));
    }
    void House()
    {
        tilemap = Map(SCREEN_HEIGHT/TILESIZE/2);
        w = h = tilemap.w;

        objects.push_back(new Entity(Door("lot",tilemap.w/2*TILESIZE,(tilemap.h-2)*TILESIZE)));

        objects.push_back(new Entity(object_templates["bed"],tilemap.w/2*TILESIZE, TILESIZE));
        objects.push_back(new Entity(object_templates["chair"],tilemap.w/2*TILESIZE+TILESIZE*2, TILESIZE));
        objects.push_back(new Entity(object_templates["stove"],tilemap.w/2*TILESIZE+TILESIZE*3, TILESIZE*2));

        ClearGrid();
    }
    void Shop()
    {
        tilemap = Map(SCREEN_HEIGHT/TILESIZE*2/3);
        w = h = tilemap.w;

        npcs.push_back(new NPC(npc_templates["shopkeeper"], 3*TILESIZE,4*TILESIZE));
        objects.push_back(new Entity(object_templates["chair"],tilemap.w/2*TILESIZE+TILESIZE*2, TILESIZE));
        objects.push_back(new Entity(Door("lot",tilemap.w/2*TILESIZE,(tilemap.h-2)*TILESIZE)));

        ClearGrid();
    }
    void Limbo()
    {
        tilemap = Map(SCREEN_HEIGHT/TILESIZE);

        w = h = tilemap.w;
        npcs.push_back(new NPC(npc_templates["???"], 3*TILESIZE,4*TILESIZE));

        ClearGrid();
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
class TimeDisplay : public Entity
{
public:
    string displaystring;
    string timestring;
    string taskstring = "Main Task:";
    string dailystring = "Daily Task:";

    EventSystem * eventsys;

    TimeDisplay(EventSystem * sys)
    {
        displaystring = "Year " + to_string(current_year) + ", " + seasons[current_season-1] + ", Day " + to_string(current_day);

        timestring = "Time:" + (current_hour < 10 ? string(" 0") : string(" ")) + to_string(current_hour) + (current_minute < 10 ? string(":0") : string(":")) + to_string(current_minute);

        w = displaystring.size()*FONTSIZE;
        h = FONTSIZE;
        y = 0;
        x = SCREEN_WIDTH-lineWidth(displaystring);

        eventsys = sys;
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
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["fade"]],x,y,w,h);
        DrawString(displaystring,x,y,-1,"font2");

        DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(timestring),y+FONTSIZE,w,h);
        DrawString(timestring,SCREEN_WIDTH-lineWidth(timestring),y+FONTSIZE,-1,"font2");

        DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(hunger_levels[(int)floor(hunger/HUNGER_SCALE)]),y+FONTSIZE*2,lineWidth(hunger_levels[(int)floor(hunger/HUNGER_SCALE)]),FONTSIZE);
        DrawString(hunger_levels[(int)floor(hunger/HUNGER_SCALE)],SCREEN_WIDTH-lineWidth(hunger_levels[(int)floor(hunger/HUNGER_SCALE)]),y+FONTSIZE*2,-1,"font2");

        DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(taskstring)-3,y+FONTSIZE*3,lineWidth(taskstring),FONTSIZE);
        DrawString(taskstring,SCREEN_WIDTH-lineWidth(taskstring)-3,y+FONTSIZE*3,-1,"font2");

        if(!eventsys->mainTask.complete)
        {
        DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(eventsys->mainTask.text)-3,y+FONTSIZE*4,lineWidth(eventsys->mainTask.text),FONTSIZE);
        DrawString(eventsys->mainTask.text,SCREEN_WIDTH-lineWidth(eventsys->mainTask.text)-3,y+FONTSIZE*4,-1,"font2");

        }
        else
        {

        }


        DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(dailystring)-3,y+FONTSIZE*5,lineWidth(dailystring),FONTSIZE);
        DrawString(dailystring,SCREEN_WIDTH-lineWidth(dailystring)-3,y+FONTSIZE*5,-1,"font2");

        if(!eventsys->dailyTask.complete)
        {
            DrawImage(images[ids["fade"]],SCREEN_WIDTH-lineWidth(eventsys->dailyTask.text)-3,y+FONTSIZE*6,lineWidth(eventsys->dailyTask.text),FONTSIZE);
            DrawString(eventsys->dailyTask.text,SCREEN_WIDTH-lineWidth(eventsys->dailyTask.text)-3,y+FONTSIZE*6,-1,"font2");

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

class DialogSystem : public Dialog
{
public:
    int base_h;
    int x,y,w,h;
    int portrait_size;
    int textLength, currentLength;
    int lineLength, ptSize;

    string default_speaker;

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
            if(skills[SecondWord(s)]>=atoi(nWord(3,s).c_str()))
                return true;
        }

        return false;
    }
    void Add(Dialog _dialog, string default_sp = "narrator")
    {
        if(speaker != "")
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
            //printf("meanwhile\n");
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

class Gui : public Entity
{
public:
    string type;

    Gui()
    {

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
    virtual void Exit()
    {

    }
};

class NumberSelector : public Gui
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
        DrawImage(im("itemtile"),x+w-TILESIZE,y+h/2,TILESIZE,h/2);

        DrawString(to_string(number),x,y+h/2-FONTSIZE/2);

    }
};



class GUIShopScreen : public Gui
{
public:

    vector<Item> item_list;
    GUIShopScreen()
    {
        type = "shop";
        w = 400;
        h = std::min(SCREEN_HEIGHT*3/4,(int)item_templates.size()*TILESIZE+TILESIZE);
        x = SCREEN_WIDTH/2-w/2;
        y = SCREEN_HEIGHT/2-h/2;
        for(auto& e : allWord(s("general-store-inventory")))
            item_list.push_back(item_templates[e]);
    }
    void Draw(float cX = 0, float cY = 0)
    {
        DrawImage(images[ids["gui"]],x,y,w,h);
        DrawString("funds: " + to_string(funds) + " " + CURRENCY,x+TILESIZE,y);

        int i = 0;
        for(auto& e : item_list)
        {
            DrawSeparator(x,y+TILESIZE+i*TILESIZE,w,TILESIZE);
            DrawImage(e.img,x,y+TILESIZE+i*TILESIZE,TILESIZE,TILESIZE);
            DrawString(e.formal,x+TILESIZE,y+TILESIZE+i*TILESIZE);
            string pricestring = to_string(e.int_attribs["price"])+"" + CURRENCY;
            DrawString(pricestring,x+w-lineWidth(pricestring)-FONTSIZE,y+TILESIZE+i*TILESIZE);
            i++;

        }
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
class GUISellScreen : public Gui
{
public:
    Inventory * inv;
    GUISellScreen(){};
    GUISellScreen(Inventory * _inv)
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

        return NULL;
    }
};

class GUIElemSkills : public Gui
{
    public:
    vector<string> skill_names = {"Skills"};
    vector<string> skill_levels = {""};
    int maxwidth = 0;

    GUIElemSkills(int _x, int _y)
    {
        w = 2*globals["gui-border"]+globals["gui-width"];
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

class GUIElemContainer
{
    vector<Gui*> GUIElems;

    void Draw(int cX = 0, int cY = 0)
    {
        for(auto g : GUIElems)
            g->Draw();
    }
};


class GUIElemEquips : public Gui
{
    public:
    GUIElemEquips(int _x, int _y)
    {
        x = _x;
        y = _y;
        w = globals["gui-width"]/2;
        h = 2*globals["gui-border"]+FONTSIZE+skills.size()*(FONTSIZE + globals["gui-margin"]);
    }
    void DrawSlot(int &pY, string str)
    {
        DrawString(str,x+w/2,pY);
        pY+=FONTSIZE;
        DrawImage(im("itemtile"),x+w/2,pY,TILESIZE,TILESIZE);
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

class GUIElemStats : public Gui
{

};
class GUICharacterScreen : public GUIElemContainer
{
    public:
    GUIElemSkills *skills;
    GUIElemEquips *equips;
    GUICharacterScreen()
    {
        w = 2*globals["gui-border"]+1.5*globals["gui-width"];
        h = 2*globals["gui-border"]+FONTSIZE;
        skills = new GUIElemSkills(SCREEN_WIDTH/2-w/2+globals["gui-width"]/2,SCREEN_HEIGHT/2-h/2);
        equips = new GUIElemEquips(SCREEN_WIDTH/2-w/2,SCREEN_HEIGHT/2-h/2);

        GUIElems.push_back(skills);
        GUIElems.push_back(equips);
        GUIElems.push_back(new GUIElemStats());

    }



};
class GUIInventoryScreen : public Gui
{
  public:
        Inventory * inv;
        int frame = TILESIZE/2;
        bool holdingItem = false;
        Item * hold;

        GUIInventoryScreen()
        {

        }
        GUIInventoryScreen(Inventory * _inv)
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
        Item* GetClick(int mX, int mY, int& id)
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

class GUIEscapeScreen : public Gui
{
public:
    GUIEscapeScreen()
    {

    }
    void Draw(float cX = 0, float cY = 0)
    {

    }
};
class GUICraftingScreen : public Gui
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
