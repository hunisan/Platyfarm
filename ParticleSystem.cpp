#include "ParticleSystem.h"

void ParticleSystem::Add(int x, int y, int w, int h, image img, bool overr, int speed)
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
void ParticleSystem::AddPlayerProjectile(int x, int y, int w, int h, image img, float angle, float speed)
{
    Entity projectile = Entity(img,x,y,w,h);

    projectile.int_attribs["angle"]=angle*100;
    projectile.int_attribs["speed"]=speed*100;
    projectile.int_attribs["time"]=0;

    playerProjectiles.push_back(projectile);

    cout << "There are " << playerProjectiles.size() << " projectiles" << endl;

}


void ParticleSystem::Add(Entity e, int speed)
{
    particles.push_back(Entity(e.img,e.x*2,e.y*2,e.w*2,e.h*2));
    particles[particles.size()-1].int_attribs["speed"]=speed;
    particles[particles.size()-1].int_attribs["transparency"]=globals["particle_opacity"];

}
int ParticleSystem::Update()
{
    for(int i = 0; i < particles.size(); i++)
    if(particles[i].int_attribs["speed"]>0)
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

    for(int i = 0; i < playerProjectiles.size();i++)
    {
        float xSpeed, ySpeed;
        float angle = playerProjectiles[i].int_attribs["angle"]/100.0;
        float speed = playerProjectiles[i].int_attribs["speed"]/100.0;

        xSpeed = cos(angle)*speed;
        ySpeed = sin(angle)*speed;

        playerProjectiles[i].addX(xSpeed);
        playerProjectiles[i].addY(ySpeed);

        playerProjectiles[i].int_attribs["time"]++;
        if(playerProjectiles[i].int_attribs["time"] > 5*SCREEN_FPS)
        {
            playerProjectiles.erase(playerProjectiles.begin()+i);
            i--;
        }
    }

    return NOTHING;
}
void ParticleSystem::Drop()
{
    particles.clear();
    playerProjectiles.clear();
}
void ParticleSystem::Draw(float cX, float cY)
{
    for(auto p : particles)
        if(p.x/2>0 && p.y/2>0)
        DrawImage(p.img,p.x/2-cX,p.y/2-cY,p.w/2,p.h/2,p.int_attribs["transparency"]/100.0);

    for(auto p : playerProjectiles)
        DrawImage(p.img,p.x-cX,p.y-cY,p.w,p.h);
}
