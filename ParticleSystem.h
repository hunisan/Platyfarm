#pragma once
#include "Entity.h"

class ParticleSystem : public Object
{
public:
    vector<Entity> particles;
    vector<Entity> playerProjectiles;

    int cooldown = 0;

    void AddPlayerProjectile(int x, int y, int w, int h, image img, float angle, float speed);

    void Drop();
    void Add(int x, int y, int w, int h, image img, bool overr = false, int speed=1);
    void Add(Entity e, int speed=1);
    int Update();
    void Draw(float cX = 0, float cY = 0);
};
