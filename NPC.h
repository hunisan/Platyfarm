#pragma once

#include "Creature.h"


class NPC : public Creature
{

    Entity * player;

    public:
    image portrait;
    int on_meet;
    int on_greet;
    map<String,String> attributes;

    String behaviour="passive";

    bool met = false;
    String fullname;

    map<String,int> AIprams;

    NPC(NPC _npc, int _x, int _y);

    NPC();

    String AI();

    virtual ~NPC();

    protected:

    private:
};
