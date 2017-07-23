#pragma once
#include "public.h"

class GameState
{
public:
    virtual void Init()
    {

    };
    virtual int Update()
    {
        return NOTHING;
    };
    virtual void Draw()
    {

    };
};

