#pragma once

#include "Creature.h"


class NPC : public Creature
{

    Creature * player;
    int attackRange=TILESIZE;
    int sightRange = 5*TILESIZE;

    void Follow(String behaviour);
    String Roam();
    void Attack();

    public:
    image portrait;
    int on_meet;
    int on_greet;
    map<String,String> attributes;

    String behaviour="passive";

    bool met = false;
    String fullname;

    map<String,int> AIprams;

    void setPlayer(Creature * player);

    NPC(NPC _npc, int _x, int _y);

    NPC();

    String AI();

    virtual ~NPC();

    protected:

    private:
};
