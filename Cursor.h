#ifndef CURSOR_H
#define CURSOR_H
#include "Visible.h"
#include "keydata.h"


class Cursor : public Visible
{
    public:
        enum
        {
            NORMAL,
            WAIT,
            PICK,
            INTERACT,
            GIVE,
            TALK,
            INSPECT,
            ADD,
        };

        int state = 0;
        int states = 8;
        String text;
        image preview;
        bool isPreview = false;

        Cursor();

        virtual ~Cursor();

        int Update();

        void Draw(float cX = 0, float cY = 0);

    protected:

    private:
};

#endif // CURSOR_H
