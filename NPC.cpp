#include "NPC.h"

NPC::NPC()
{
    //ctor
}
NPC::NPC(NPC _npc, int _x, int _y) : NPC(_npc)
{
    x = _x;
    y = _y;
}

NPC::~NPC()
{
    //dtor
}

void NPC::Attack()
{
    if(!cooldown)
    {
        player->Damage(1);
        player->KnockBack(x,y,8);
        setCooldown(500/13);
    }
}
void NPC::Follow(String behaviour)
{
    float distance;
    float dx = x-player->x;
    float dy = y-player->y;

    distance = sqrt(dx*dx+dy*dy);

    if(distance < sightRange && distance > attackRange)
    {
        float angle = atan2(dy,dx);
        float speed = movement_speed* ((FirstWord(behaviour)=="follow") ? -1 : 1);
        this->addX(cos(angle)*speed);
        this->addY(sin(angle)*speed);


    }
    else if(distance <= attackRange)
        if(this->attributes.count("enemy"))
            Attack();

}

String NPC::Roam()
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

    return "nothing";
}
String NPC::AI()
{
    this->Update();

    if(behaviour == "passive")
    {

    }
    else if(FirstWord(behaviour)=="follow" || FirstWord(behaviour)=="evade")
    {
        this->Follow(behaviour);
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
        return this->Roam();
    }


    return "nothing";
}

void NPC::setPlayer(Creature * player)
{
    this->player = player;
}
