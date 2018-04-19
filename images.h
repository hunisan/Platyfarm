#ifndef IMAGES_H
#define IMAGES_H

#include "public.h"



extern vector<image> images;
extern image gif[8];
extern map<String, int> ids;


image loadImage(String path);

void DrawImage(image img, float x, float y, float w, float h, float alpha=1.0);


void DrawCentered(image img, float x, float y, float w, float h, float alpha=1.0);


void DrawClip(image img, float x, float y, float w, float h, float sx, float sy, float sw, float sh, float iw, float ih, float alph = 1);


void DrawRect(float x, float y, float w, float h, float alpha);


void DrawSeparator(float x, float y, float w, float h);

void DrawLetter(char c, int x, int y,int ptsize = -1, String font = "font");


void DrawString(String c, int x, int y, int ptsize = -1, String font = "font");

void DrawStringCentered(String c, int x, int y, int ptsize = -1, String font = "font");

void DrawWrapped(String c, int x, int y, float linelen, int ptsize = -1, String font = "font");

#endif
