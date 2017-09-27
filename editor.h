#pragma once
#include "gamestate.h"
#include "objects.h"

class Editor : public GameState
{
public:
    Stage * stage;
    GUIEditorInterface * editorInterface;
    Cursor cursor;
    string cursorMode = "delete";
    int current_tile = 0;
    int current_object = 0;
    float scroll_speed = 4;

    Editor()
    {
        LoadObjects();
        camera_x = 0;
        camera_y = 0;


        stage = new Stage(10);

        for(int i = 0; i < 10; i++)
            for(int j = 0; j < 10; j++)
                stage->Set(i,j,0);

        editorInterface = new GUIEditorInterface(stage,&cursorMode);
    }
    bool HoverOverInterface()
    {
        return Contains(editorInterface,KeyData.MouseX,KeyData.MouseY,0,0);
    }
    void OnLeftClick()
    {
        if(HoverOverInterface())
        {
            editorInterface->HandleClick(KeyData.MouseX,KeyData.MouseY);
        }
        else
        {

            if(cursorMode == "object")
            {

                int dX = floor((camera_x+cursor.x)/TILESIZE)*TILESIZE;
                int dY = floor((camera_y+cursor.y)/TILESIZE)*TILESIZE;

                if(dX < stage->w*TILESIZE && dY < stage->h*TILESIZE)
                    stage->objects.push_back(new Entity(object_templates[object_names[current_object]],dX,dY));
            }
            else if(cursorMode == "delete")
            {
                for(int i = 0; i < stage->objects.size(); i++)
                if(stage->objects[i])
                {
                    if(Contains(stage->objects[i],cursor.x,cursor.y,camera_x,camera_y))
                    {
                        stage->objects.erase(stage->objects.begin()+i);
                        break;
                    }
                }
            }
        }

    }
    int Update()
    {
        cursor.Update();

        editorInterface->Update();

        if(KeyData.wheel != 0)
        {
            if(cursorMode == "terraform")
            {
                current_tile += KeyData.wheel;
                if(current_tile<0)
                    current_tile = g("tiles")-1;
                else if(current_tile>= g("tiles"))
                    current_tile = 0;
            }
            else if(cursorMode == "object")
            {
                current_object+= KeyData.wheel;
                if(current_object < 0)
                    current_object = object_names.size()-1;
                else if(current_object >= object_names.size())
                    current_object = 0;
            }

        }

        if(KeyData.ADown)
        {
            camera_x -= scroll_speed;

        }
        if(KeyData.DDown)
        {
            camera_x += scroll_speed;
        }
        if(KeyData.WDown)
        {
            camera_y -= scroll_speed;
        }
        if(KeyData.SDown)
        {
            camera_y += scroll_speed;
        }

        if(KeyData.EnterPress)
        {
            printf("Saving stage..");
            XMLManager::SaveStage(stage,"data/stage/custom.xml");
            printf("Done!\n");
        }


        if(KeyData.Click)
            OnLeftClick();
        if(KeyData.MouseLeftDown)
        {
            if(!HoverOverInterface() && cursorMode == "terraform")
            {
                int dX = floor((camera_x+cursor.x)/TILESIZE);
                int dY = floor((camera_y+cursor.y)/TILESIZE);

                stage->Set(dX,dY,current_tile);
            }
        }

        return NOTHING;
    }

    void DrawCursor()
    {
        if(!HoverOverInterface())
        {
            if(cursorMode == "terraform")
            {
                int dX = floor((camera_x+cursor.x)/TILESIZE)*TILESIZE;
                int dY = floor((camera_y+cursor.y)/TILESIZE)*TILESIZE;

                DrawClip(im("tileset"),dX - camera_x,dY - camera_y,TILESIZE,TILESIZE,(current_tile%g("tiles-horizontal"))*TILESIZE,(current_tile/g("tiles-horizontal"))*TILESIZE,TILESIZE,TILESIZE,g("tiles-horizontal")*TILESIZE,2*TILESIZE,0.5);
            }
            else if(cursorMode == "object")
            {
                int dX = floor((camera_x+cursor.x)/TILESIZE)*TILESIZE;
                int dY = floor((camera_y+cursor.y)/TILESIZE)*TILESIZE;

                Entity(object_templates[object_names[current_object]],dX-camera_x,dY-camera_y).Draw();
            }
            else if(cursorMode == "delete")
            {
                for(auto o : stage->objects)
                if(o)
                {
                    if(Contains(o,cursor.x,cursor.y,camera_x,camera_y))
                    {
                        DrawString(o->name,cursor.x+cursor.w,cursor.y,-1,"font2");
                        break;
                    }
                }
            }
        }

        cursor.Draw();
    }
    void Draw()
    {
        stage->Draw(camera_x,camera_y);

        for(auto e : stage->objects)
        {
            e->Draw(camera_x,camera_y);
        }

        for(auto e : stage->npcs)
        {
            e->Draw(camera_x,camera_y);
        }


        editorInterface->Draw();

        DrawCursor();
    }
};
