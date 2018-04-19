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

String NPC::AI()
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
