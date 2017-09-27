#pragma once
#include "public.h"

/*namespace content
{
    image platypus = NULL;
    image grass = NULL;
    image grass2 = NULL;
    image cursor = NULL;
    image dark = NULL;
    image bush = NULL;
    image platy32 = NULL;
    image itembar = NULL;
    image frame = NULL;
    image axe = NULL;
    image apple = NULL;
    image trunk = NULL;
};*/

vector<image> images;
image gif[8];
map<string, int> ids;


image loadImage(string path)
{
    image TextureID = 0;
    string imgpath = "data/img/" + path;
    SDL_Surface * img = IMG_Load(imgpath.c_str());

    if(!img)
        return 0;

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    int Mode = GL_RGB;

    if(img->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Mode, img->w, img->h, 0, Mode, GL_UNSIGNED_BYTE, img->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    return TextureID;
}
void DrawImage(image img, float x, float y, float w, float h, float alpha=1.0)
{
    glBindTexture(GL_TEXTURE_2D, img);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);

        glColor4f(1,1,1,alpha);

        glBegin( GL_QUADS );
            glTexCoord2f(0, 1); glVertex2f( x, y+h );
            glTexCoord2f(1, 1); glVertex2f( x+w, y+h );
            glTexCoord2f(1, 0); glVertex2f( x+w, y );
            glTexCoord2f(0, 0); glVertex2f( x, y );
        glEnd();

        //glColor4f(1,1,1,1);

        glLoadIdentity();
}

void DrawCentered(image img, float x, float y, float w, float h, float alpha=1.0)
{
    DrawImage(img,x-w/2,y-h/2,w,h,alpha);
}

void DrawClip(image img, float x, float y, float w, float h, float sx, float sy, float sw, float sh, float iw, float ih, float alph = 1)
{
    glBindTexture(GL_TEXTURE_2D, img);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);

        glColor4f(1,1,1,alph);

        glBegin( GL_QUADS );
            glTexCoord2f(sx/iw, (sy+sh)/ih); glVertex2f( x, y+h );
            glTexCoord2f((sx+sw)/iw, (sy+sh)/ih); glVertex2f( x+w, y+h );
            glTexCoord2f((sx+sw)/iw, sy/ih); glVertex2f( x+w, y );
            glTexCoord2f(sx/iw, sy/ih); glVertex2f( x, y );
        glEnd();

        glColor4f(1,1,1,1);

        glLoadIdentity();
}

void DrawRect(float x, float y, float w, float h, float alpha)
{
        glBindTexture(GL_TEXTURE_2D, images[ids["dark"]]);

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();
        glOrtho(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);

        glPushMatrix();



        glBegin( GL_QUADS );
            glColor4f(1.f,1.f,1.f,alpha);
            glVertex2f( x, y+h );
            glVertex2f( x+w, y+h );
            glVertex2f( x+w, y );
            glVertex2f( x, y );
        glEnd();
        //glColor4f(1,1,1,1);

        glPopMatrix();
        glLoadIdentity();

}

void DrawSeparator(float x, float y, float w, float h)
{
    DrawImage(images[ids["separator_left"]],x,y,TILESIZE,TILESIZE);

    for(int i = 0; i < std::floor(w/TILESIZE); i++)
    {
        DrawImage(images[ids["separator_v"]],x+i*TILESIZE,y,TILESIZE,TILESIZE);

    }
    DrawImage(images[ids["separator_right"]],x+w-TILESIZE,y,TILESIZE,TILESIZE);

}
void DrawLetter(char c, int x, int y,int ptsize = -1, string font = "font")
{
    int cy = (int)c/16;
    int cx = (int)c%16;
    if(ptsize == -1)
        ptsize = FONTSIZE;

    DrawClip(images[ids[font]],x,y,(float)characterWidth(c)*ptsize/g("character-scale"),ptsize,cx*32+16-characterWidth(c)/2,cy*32,characterWidth(c),32,512,512);

}

void DrawString(string c, int x, int y, int ptsize = -1, string font = "font")
{
    int dx=x;
    if(ptsize == -1)
        ptsize = FONTSIZE;
    for(int i = 0; i < c.size(); i++)
    {
        if(c[i]==32)
        {
            x += 10;
        }
        else
        {
            DrawLetter(c[i],x,y,ptsize, font);
            x += characterWidth(c[i])*ptsize/g("character-scale");

        }
    }
}

void DrawStringCentered(string c, int x, int y, int ptsize = -1, string font = "font")
{
    if(ptsize == -1)
        ptsize = FONTSIZE;
    DrawString(c,x-lineWidth(c,ptsize)/2,y,ptsize,font);
}
void DrawWrapped(string c, int x, int y, float linelen, int ptsize = -1, string font = "font")
{
    //string a = c;
    int dx = x;
    int dy = y;
    if(ptsize == -1)
        ptsize = FONTSIZE;


    ///EZT AT KELL IRNI
    ///MOSTMAR NEM KELL ATIRNI
    std::stringstream ss;
    vector<string> words;
    ss << c;
    string newword;
    while(ss >> newword){words.push_back(newword);};

    int i = 0;
    int current_length = 0;
    string line = "";
    int lines = 0;
    while(i < words.size())
    {
        if(current_length + lineWidth(words[i] + " ",ptsize) + 1 > linelen)
        {
            DrawString(line,x,y+lines*FONTSIZE,ptsize,font);
            lines++;
            line = "";
            current_length = 0;
        }
        else
        {
            line.append(words[i]+ " ");
            current_length += lineWidth(words[i] + " ",ptsize);
            i++;
        }
    }
    if(line.size()>0)
        DrawString(line,x,y+lines*FONTSIZE,ptsize,font);

    ///regi sortordeles
    /*for(int j = 0; j < ceil(c.size()/linelen); j++)
    {
        for(int i = 0; i < std::min((int)a.size(),(int)linelen); i++)
        {
            DrawLetter(a[i],dx,dy,ptsize, font);
            dx += characterWidth(a[i]) * ptsize / FONTSIZE;
        }
        dy += ptsize;
        dx = x;
        if(a.size()<linelen)
            break;
        else
            a.erase(a.begin(),a.begin()+linelen);//std::min((int)a.size(),30));
    }*/
}
