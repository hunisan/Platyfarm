#pragma once
#include "public.h"


class Object
{
    public:
        map<String, int> int_attribs;
        map<String, String> string_attribs;
        Object();
        virtual ~Object();
        virtual int Update();
    protected:

    private:
};

