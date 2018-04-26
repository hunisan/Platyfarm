#include "Creature.h"

Creature::Creature()
{
    movement_speed=1;
    //ctor
}
Creature::Creature(image _img, float _x, float _y, float _w, float _h, float _alt) : Entity(_img,_x,_y,_w,_h)
{
    movement_speed=3;
    alt = _alt;
}
Creature::~Creature()
{
    //dtor
}
void Creature::Damage(int damage)
{
    int_attribs["hp"] -= damage;
    setCooldown(500/13);
    cout << "Creature damaged!" << endl;
}
void Creature::recalculateStats()
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
void Creature::increaseStat(String targetStat, int points, int duration)
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
void Creature::setCooldown(int cd)
{
    cooldown = cd;
}
int Creature::Update()
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
