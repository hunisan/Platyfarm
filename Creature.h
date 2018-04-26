#ifndef CREATURE_H
#define CREATURE_H

#include "Entity.h"


class Creature : public Entity
{
    public:
        struct Modifier
        {
            String stat;
            int points;
            int duration;
            Modifier(String s, int p, int d) : stat(s),points(p),duration(d)
            {

            }
        };
        float movement_speed;
        int cooldown = 0;
        vector<String> stats;
        map<String, int> stat_levels;
        vector<Modifier> modifiers;

        Creature();
        Creature(image _img, float _x, float _y, float _w, float _h, float _alt = 0);

        virtual void Damage(int damage);

        void setCooldown(int cd);

        virtual ~Creature();

        void recalculateStats();
        void increaseStat(String targetStat, int points, int duration);

        int Update();

    protected:

    private:
};

#endif // CREATURE_H
