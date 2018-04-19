#include "Cursor.h"

Cursor::Cursor(): Visible(images[ids["cursor"]],0,0,24,24)
{
    //state = "cursor";
}
Cursor::~Cursor()
{
    //dtor
}

int Cursor::Update()
{
    x = KeyData.MouseX;
    y = KeyData.MouseY;

    return 1;
}

void Cursor::Draw(float cX, float cY)
{
    DrawClip(img,x,y,w,h,state*64,0,64,64,states*64,2*64);
    if(isPreview)
    {
        int place_x = std::floor((x+camera_x)/TILESIZE);
        int place_y = std::floor((y+camera_y)/TILESIZE);
        DrawImage(preview,place_x*TILESIZE-camera_x,place_y*TILESIZE-camera_y,TILESIZE,TILESIZE);

    }
    DrawImage(images[ids["fade"]],x+w,y,lineWidth(text),FONTSIZE);
    DrawString(text,x+w,y,-1,"font2");
}
