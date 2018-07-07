#pragma once

#include "Creature.h"


class NPC : public Creature
{

    Creature * player;

    void Follow(String behaviour);
    String Roam();
    void Attack();

    public:
    int attackRange=TILESIZE;
    int sightRange = 5*TILESIZE;


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

    void Draw(float cX = 0, float cY = 0);

    void Damage(int damage);
    
    String AI();

    virtual ~NPC();

    protected:

    private:
};
