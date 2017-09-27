#pragma once

struct
{
    bool AnyPress = false;
    bool UpDown = false;
    bool UpPress = false;
    bool DownDown = false;
    bool DownPress = false;
    bool LeftDown = false;
    bool LeftPress = false;
    bool RightDown = false;
    bool RightPress = false;
    bool EnterPress = false;
    bool EscapePress = false;
    bool OPress = false;
    bool XPress = false;
    bool KPress = false;
    bool WDown = false;
    bool SDown = false;
    bool ADown = false;
    bool DDown = false;
    bool MouseMove = false;
    bool WPress = false;
    bool SPress = false;
    bool APress = false;
    bool DPress = false;
    bool CPress = false;
    bool FPress = false;
    bool EPress = false;
    bool Click = false;
    bool MouseLeftDown = false;
    bool RightClick = false;
    bool numPress[10];

    map<string,bool> keyInfo;

    int lasth = 0;
    int lastv = 0;
    int wheel = 0;

    bool getKey(int ax)
    {
        if(ax == 0)
        {
            if(lasth == 1)
                return ADown;
            else
                return DDown;
        }
        else
        {
            if(lastv == 1)
                return WDown;
            else
                return SDown;
        }
    }
    int getDir(int d)
    {
        if(d == 1)
            return -1;
        else
            return 1;
    }
    float MouseX = 0;
    float MouseY = 0;

    void Update()
    {
        AnyPress = false;
        DownPress = false;
        UpPress = false;
        EnterPress = false;
        EscapePress = false;
        RightPress = false;
        LeftPress = false;
        KPress = false;
        OPress = false;
        XPress = false;
        MouseMove = false;
        WPress = false;
        SPress = false;
        APress = false;
        DPress = false;
        CPress = false;
        FPress = false;
        EPress = false;
        Click = false;
        RightClick = false;
        for(auto& i : numPress)
            i = false;
        wheel = 0;
    }
} KeyData;
