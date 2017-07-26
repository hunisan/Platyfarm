#pragma once
#include "gamestate.h"
#include "objects.h"
#include "keydata.h"
#include "tinyxml2.h"
#include <future>

class Ingame : public GameState
{
private:

    Player * player;
    Entity * ptr_to_player = player;
    Stage * currentstage;
    Stage player_lot, player_house, forest, general_store, limbo;
    Stage * fade_stage;
    map<string,Stage*> stageList;
    vector<string> stageNames;

    LightingSystem lightsys;
    WeatherSystem weathersys;
    ParticleSystem particlesys;
    Entity * entity;
    Entity * ent2;
    Inventory * inventory;
    EventSystem * eventSystem;
    DialogSystem * dialogSystem;
    Cursor * cursor;
    TimeDisplay * timedisplay;
    Preview * preview;

    Gui * gui;

    bool busy = false;
    int targetState = NOTHING;
    //vector<Entity**> drawables;
    vector<Entity*> draw_list;
public:
    //vector<int> used_ids;

    bool reserved(int id)
    {
        for(auto& e : draw_list)
            if(e)
                if(e->id == id)
                    return true;

        return false;
    }
    int newid()
    {
        int id;
        do
        {
            id = rand()%MAXIDS;
        }
        while(reserved(id));

        return id;

    }

    void remove_entity(Entity * &en)
    {
        for(auto &e : draw_list)
        {
            if(e)
                if(e->id == en->id)
                {
                    draw_list.erase(std::find(draw_list.begin(),draw_list.end(),e));
                    break;
                }
        }
        currentstage->Remove(en);
        en = NULL;
    }

    void change_entity(Entity * &_old, Entity * _new)
    {
        int id = _old->id;
        for(auto &e : draw_list)
        {
            if(e)
                if(e->id == id)
                {
                    e=_new;
                    break;
                }
        }
        currentstage->Remove(_old);
        currentstage->Add(_new);
        _old = _new;
        _new->id = id;

        //cout << _new->leftover << endl;
    }
    //vector<Entity *> objects;
    //vector<NPC *> npcs;

    //map<string, int> object_ids;
    //map<string, int> item_ids;
    //map<string, int> npc_ids;

    vector<Dialog> dialogs;

    enum {
        CONTROL,
        DIALOG,
        GUI,
        FADE,
    };

    int gamephase;

    void AddXp(string skill, int xp)
    {
        xps[skill] += xp;
        while(xps[skill] >= levelcap(skills[skill]))
        {
            xps[skill] -= levelcap(skills[skill]);
            skills[skill]++;
            alerts.push("Levelup! Reached " + skill + " lvl " + to_string(skills[skill]));
            if(unlocks.count(skill) && unlocks[skill].size()>=skills[skill]-2)
                alerts.push(unlocks[skill][skills[skill]-2]);
        }
    }
    void Action(Entity * e, string a)
    {
        if(a == "nothing")
            return;

        if(FirstWord(a) == "move")
        {
            float angle = atof(SecondWord(a).c_str());

            int savex = e->x, savey = e->y;

            bool canMove = true;

            e->x += cos(angle);

            for(auto o : currentstage->objects)
            if(o)
            if(o->id != e->id && Intersect(o,e))
                canMove = false;

            if(!canMove)
                e->x = savex;

            savex = e->x, savey = e->y;

            e->y += sin(angle);

            canMove = true;

            /*for(int i = 0; i <= 1; i++)
            for(int j = 0; j <= 1; j++)
            if(!currentstage->IsFree(e->x/TILESIZE+i*TILESIZE,e->y/TILESIZE+j*TILESIZE))
            */
            for(auto o : currentstage->objects)
            if(o)
            if(o->id != e->id && Intersect(o,e))
                canMove = false;

            if(!canMove)
            {
                e->y = savey;
            }
        }
    }
    void RunScript(string s, Entity **e = NULL,string i="")
    {
        string fw = FirstWord(s);
        string sw = SecondWord(s);
        string tw = nWord(3,s);
        cout << s << endl;

        if(fw == "script")
        {
            for(auto sc : split(s,"script"))
                RunScript(sc,e,i);
        }
        else if(fw == "gui")
        {
            gamephase = GUI;
            string sw = SecondWord(s);
            if(sw == "shop")
                gui = new ShopGui();
            else if(sw == "sell")
                gui = new SellGui(inventory);

        }
        else if(fw == "nextday")
        {
            NextDay();
            busy = true;
            //static std::future<void> future = std::async(std::launch::async,[this]{Save();});
            //
            Save();
            //auto ff = std::async(std::launch::async,[this]{busy = false;printf("kek\n");});

        }
        else if(fw == "xp")
        {
            AddXp(sw,atoi(tw.c_str()));
        }
        else if(fw == "skill")
        {
            //string tw = nWord(3,s);
            AddXp(sw,levelcap(skills[sw]));
        }
        else if(fw == "item")
        {
            int amount = 1;
            if(tw != "")
                if(atoi(tw.c_str())!=0)
                    amount = atoi(tw.c_str());

            //cout << amount << endl;

            cout << amount << endl;
            inventory->Add(item_templates[sw],amount);
        }
        else if(fw == "consume")
        {
            inventory->Remove(i,1);
        }
        else if(fw == "delete")
        {
            if(e)
            {
                particlesys.Add(**e);
                remove_entity(*e);

            }
        }
        else if(fw == "change")
        {
            if(e)
            {
                Entity * _new = new Entity(object_templates[sw],(*e)->x,(*e)->y);
                change_entity(*e,_new);
            }
        }
        else if(fw == "harvest")
        {
            RunScript("script item " + sw + " " + tw + " script delete",e);
        }
        else if(fw == "attrib")
        {
            (*e)->string_attribs[sw]=tw;
        }
        else if(fw == "spawn")
        {
            NPC* npc = new NPC(npc_templates[sw],KeyData.MouseX+camera_x,KeyData.MouseY+camera_y);

            currentstage->npcs.push_back(npc);
            AddDrawable(npc);
        }
        else if(fw == "dialog")
        {
            gamephase = DIALOG;
            dialogSystem->Add(dialogs[atoi(sw.c_str())]);
        }
        else if(fw == "sound")
        {
            audioCommands.push(s);
        }
        else if(fw == "exit")
        {
            targetState = MENU;
        }
    }
    Ingame()
    {
        gamephase = CONTROL;
    }

    bool PlaceObject(Entity * e, int x, int y)
    {
        int w = e->w/TILESIZE;
        int h = e->h/TILESIZE;

        bool canPlace = true;

        for(int i = 0 ; i < w; i++)
            for(int j = 0; j < h; j++)
            {
                if(x+i < currentstage->w && y+j < currentstage->h)
                {
                    if(currentstage->grid[x+i][y+j])
                    {
                        canPlace = false;
                        break;
                    }
                }
                else
                {
//                    if(e->name == "house")
//                    {
//                        cout << i << " " << w << " " << currentstage->w << endl;
//                        cout << j << " " << h << endl;
//                    }
                    canPlace = false;
                    break;
                }

            }

        if(canPlace)
        {
            e->x = x*TILESIZE;
            e->y = y*TILESIZE;

            currentstage->objects.push_back(e);
            for(int i = 0 ; i < w; i++)
                for(int j = 0; j < h; j++)
                {
                    currentstage->grid[x+i][y+j]=e;
                }
        }

        return canPlace;

    }

    void Alert(string msg)
    {
        gamephase = DIALOG;
        dialogSystem->Add(Dialog(msg));
    }
    void PlaceOverride(Entity* e, int x, int y)
    {
        e->x = x*TILESIZE;
        e->y = y*TILESIZE;

        for(auto &o : currentstage->objects)
            if(o)
                if(o->x == e->x && o->y == e->y)
                {
                    remove_entity(o);
                }

        currentstage->objects.push_back(e);


        currentstage->grid[x][y]=e;
    }
    bool SpawnObject(Entity * e, int min_x, int min_y, int max_x, int max_y)
    {
        //cout << "spawning" << endl;
        vector<std::pair<int,int> > random_box;

        for(int i = min_x; i < max_x; i++)
            for(int j = min_y; j < max_y; j++)
        {
            random_box.push_back(std::make_pair(i,j));
        }

        bool sikerult = false;

        while(!sikerult && random_box.size() > 0)
        {
            //cout << "randoming" << endl;
            int randindex = rand()%random_box.size();

            e->x = random_box[randindex].first*TILESIZE;
            e->y = random_box[randindex].second*TILESIZE;

            sikerult = PlaceObject(e,random_box[randindex].first,random_box[randindex].second);

            random_box.erase(random_box.begin()+randindex);
        };

        return sikerult;
    }

    void SpawnObjects(string e, int min_x, int min_y, int max_x, int max_y, int _count)
    {
        int c = 0;
        while(c < _count && SpawnObject(new Entity(ent(e)),min_x,min_y,max_x,max_y))
        {
            c++;
        }
    }
    void Save()
    {
        using namespace tinyxml2;

        XMLDocument saveFile;

        XMLNode * pRoot = saveFile.NewElement("root");

        saveFile.InsertFirstChild(pRoot);

        XMLElement * pElement;

        auto InsertInt = [&pElement, &pRoot, &saveFile](string name, int value, XMLNode* elem)
        {
            pElement = saveFile.NewElement(name.c_str());
            pElement->SetText(value);
            elem->InsertEndChild(pElement);

        };
        auto InsertString = [&pElement, &pRoot, &saveFile](string name, string value, XMLNode* elem )
        {
            pElement = saveFile.NewElement(name.c_str());
            pElement->SetText(value.c_str());
            elem->InsertEndChild(pElement);

        };
        InsertInt("year",current_year,pRoot);

        InsertInt("season",current_season,pRoot);

        InsertInt("day",current_day,pRoot);

        InsertInt("funds",funds,pRoot);

        InsertString("player",to_string(player->x) + " " + to_string(player->y),pRoot);

        for(auto& i : inventory->toolbar)
        {
            if(i)
                InsertString("toolbar",i->name+ " " + to_string(i->quantity),pRoot);
            else
                InsertString("toolbar","blank",pRoot);
        }
        for(auto& i : inventory->items)
        {
            if(i)
                InsertString("item",i->name+ " " + to_string(i->quantity),pRoot);
            else
                InsertString("item","blank",pRoot);
        }

        ///Save skills
        XMLElement* skillsElement = saveFile.NewElement("skills");

        for(auto skill : skills)
            InsertInt(skill.first,skill.second,skillsElement);

        pRoot->InsertEndChild(skillsElement);


        for(auto stage : stageList)
        {

            XMLElement * stageElement = saveFile.NewElement("stage");

            InsertString("name",stage.second->name,stageElement);

            InsertString("size",to_string(stage.second->w)+" "+to_string(stage.second->h),stageElement);

            if(stage.second->indoors)
                stageElement->InsertEndChild(saveFile.NewElement("indoors"));

            fr(0,stage.second->h)
            {
                string row;
                for(int j = 0; j < stage.second->w; j++)
                    row.append(to_string(stage.second->tilemap.tile[j][i]) + " ");

                pElement = saveFile.NewElement("row");

                pElement->SetText(row.c_str());

                stageElement->InsertEndChild(pElement);
            }
            for(auto e : stage.second->objects)
            if(e && e->x < stage.second->w*TILESIZE && e->y < stage.second->h*TILESIZE)
            {
                pElement = saveFile.NewElement("object");
                auto InsertChildInt = [&pElement, &pRoot, &saveFile](string name, int value)
                {
                    XMLElement * subElement = saveFile.NewElement(name.c_str());
                    subElement->SetText(value);
                    pElement->InsertEndChild(subElement);

                };
                auto InsertChildString = [&pElement, &pRoot, &saveFile](string name, string value)
                {
                    XMLElement * subElement = saveFile.NewElement(name.c_str());
                    subElement->SetText(value.c_str());
                    pElement->InsertEndChild(subElement);

                };

                if(e->name != "")
                    InsertChildString("name",e->name);
                else
                    InsertChildString("name","door");

                InsertChildString("pos",to_string(e->x)+" "+to_string(e->y));

                XMLElement* attributes = saveFile.NewElement("attributes");

                for(auto attrib : e->int_attribs)
                {
                    XMLElement * attribute = saveFile.NewElement(attrib.first.c_str());
                    attribute->SetText(to_string(attrib.second).c_str());
                    attributes->InsertEndChild(attribute);
                }
                for(auto attrib : e->string_attribs)
                {
                    XMLElement * attribute = saveFile.NewElement(attrib.first.c_str());
                    attribute->SetText(to_string(attrib.second).c_str());
                    attributes->InsertEndChild(attribute);
                }
                if(attributes->FirstChildElement() != NULL)
                    pElement->InsertEndChild(attributes);

                stageElement->InsertEndChild(pElement);
            }

            for(auto e : stage.second->npcs)
            if(e)
            {
                printf("Saving npc..\n");
                pElement = saveFile.NewElement("npc");
                auto InsertChildInt = [&pElement, &pRoot, &saveFile](string name, int value)
                {
                    XMLElement * subElement = saveFile.NewElement(name.c_str());
                    subElement->SetText(value);
                    pElement->InsertEndChild(subElement);

                };
                auto InsertChildString = [&pElement, &pRoot, &saveFile](string name, string value)
                {
                    XMLElement * subElement = saveFile.NewElement(name.c_str());
                    subElement->SetText(value.c_str());
                    pElement->InsertEndChild(subElement);

                };

                if(e->met)
                    pElement->InsertEndChild(saveFile.NewElement("met"));

                InsertChildString("name",e->name);

                InsertChildString("pos",to_string(e->x)+" "+to_string(e->y));

                stageElement->InsertEndChild(pElement);
            }

            pRoot->InsertEndChild(stageElement);
        }
        saveFile.SaveFile("data/save/save.xml");

        busy = false;
        //saveFile.
    }
    bool LoadDialogs()
    {
        printf("Loading dialogs from XML list...\n");
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/xml/dialog.xml");

        XMLCheckResult(result);
        printf("XML file loaded..\n");

        XMLElement * pRoot = xmlDoc.FirstChildElement();
        if(pRoot == NULL)
        {
            printf("XML read error (root)\n");
            return 0;
        }
        XMLElement * pElement = pRoot->FirstChildElement();
        if(pElement == NULL)
        {
            printf("XML read error!\n");
            return 0;
        }
        int id;

        dialogs.push_back(Dialog());

        while (pElement != NULL)
        {
            printf("Loading dialog\n");

            pElement->QueryIntAttribute("id", &id);

            Dialog newitem;

            if(pElement->FirstChildElement("speaker"))
                newitem.speaker = pElement->FirstChildElement("speaker")->GetText();
            else
                newitem.speaker = "";

            XMLElement * chainElement = pElement->FirstChildElement("chain")->FirstChildElement("text");

            while(chainElement != NULL)
            {
                newitem.chain.push_back(chainElement->GetText());

                chainElement = chainElement->NextSiblingElement("text");
            }
            if(pElement->FirstChildElement("question")!=NULL)
            {
                newitem.hasQuestion = true;
                newitem.question = pElement->FirstChildElement("question")->FirstChildElement("text")->GetText();
                chainElement = pElement->FirstChildElement("question")->FirstChildElement("response");

                while(chainElement != NULL)
                {
                    newitem.answers.push_back(chainElement->GetText());
                    int attribute;
                    chainElement->QueryIntAttribute("goto",&attribute);
                    newitem.gotos.push_back(attribute);
                    if(chainElement->Attribute("outcome")!=NULL)
                        newitem.outcomes.push_back(chainElement->Attribute("outcome"));
                    else
                    {
                        newitem.outcomes.push_back("");
                    }
                    if(chainElement->Attribute("condition")!=NULL)
                        newitem.conditions.push_back(chainElement->Attribute("condition"));
                    else
                    {
                        newitem.conditions.push_back("");
                    }
                    chainElement = chainElement->NextSiblingElement("response");
                }
            }
            dialogs.push_back(newitem);

            //item_ids[newitem.name]=id;

            pElement = pElement->NextSiblingElement("dialog");
        };

        printf("Finished loading dialogs!\n");
        return 1;
    }
    bool LoadObjects()
    {
        printf("Loading objects from XML list...\n");
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/xml/object.xml");

        XMLCheckResult(result);
        printf("XML file loaded..\n");

        XMLElement * pRoot = xmlDoc.FirstChildElement();
        if(pRoot == NULL)
        {
            printf("XML read error (root)\n");
            return 0;
        }
        XMLElement * pElement = pRoot->FirstChildElement();
        if(pElement == NULL)
        {
            printf("XML read error!\n");
            return 0;
        }
        int id;

        //object_templates.push_back(Entity());

        while (pElement != NULL)
        {
            printf("Loading object\n");

            //pElement->QueryIntAttribute("id", &id);

            Entity newobject;

            newobject.name = pElement->FirstChildElement("name")->GetText();

            object_names.push_back(newobject.name);

            newobject.img = images[ids[pElement->FirstChildElement("img")->GetText()]];
            //newobject.max_stack = atoi(pElement->FirstChildElement("stack")->GetText());
            newobject.category = pElement->FirstChildElement("category")->GetText();
            newobject.type = pElement->FirstChildElement("type")->GetText();
            newobject.w = (pElement->FirstChildElement("width")?atoi(pElement->FirstChildElement("width")->GetText())*TILESIZE:TILESIZE);
            newobject.h = (pElement->FirstChildElement("height")?atoi(pElement->FirstChildElement("height")->GetText())*TILESIZE:TILESIZE);

            if(pElement->FirstChildElement("rigid") != NULL)
                newobject.rigid = true;

            if(pElement->FirstChildElement("pick") != NULL)
            {
                newobject.pickable = true;
                newobject.pick = pElement->FirstChildElement("pick")->GetText();
            }
            if(pElement->FirstChildElement("leftover") != NULL)
            {
                newobject.leftover = pElement->FirstChildElement("leftover")->GetText();
            }
            if(pElement->FirstChildElement("growth")!= NULL)
            {
                newobject.int_attribs["growth"] = 0;
                newobject.int_attribs["cycle"] = atoi(pElement->FirstChildElement("growth")->GetText());
                newobject.string_attribs["ripe"] = pElement->FirstChildElement("ripe")->GetText();
            }
            else
            {
                //newobject.int_attribs["growth"] = 0;
                //newobject.int_attribs["cycle"] = 1;

            }

            if(pElement->FirstChildElement("destroy") != NULL)
                newobject.destroy = pElement->FirstChildElement("destroy")->GetText();

            if(pElement->FirstChildElement("hits") != NULL)
                newobject.hits = atoi(pElement->FirstChildElement("hits")->GetText());
            else
                newobject.hits = 1;

            newobject.health = newobject.hits;

            if(pElement->FirstChildElement("alt_img") != NULL)
                newobject.alts["damaged"] = images[ids[pElement->FirstChildElement("alt_img")->GetText()]];

            if(pElement->FirstChildElement("fade") != NULL)
                newobject.alts["fade"] = images[ids[pElement->FirstChildElement("fade")->GetText()]];

            if(pElement->FirstChildElement("damaged_fade") != NULL)
                newobject.alts["damaged_fade"] = images[ids[pElement->FirstChildElement("damaged_fade")->GetText()]];

            if(pElement->FirstChildElement("alt") != NULL)
            {
                newobject.alt = atoi(pElement->FirstChildElement("alt")->GetText())*TILESIZE;
            }

            if(pElement->FirstChildElement("offset") != NULL)
            {
                newobject.offset = atoi(pElement->FirstChildElement("offset")->GetText())*TILESIZE;
            }

            if(pElement->FirstChildElement("imgw") != NULL)
                newobject.imgw = atoi(pElement->FirstChildElement("imgw")->GetText())*TILESIZE;
            else
                newobject.imgw = TILESIZE;

            if(pElement->FirstChildElement("imgh") != NULL)
                newobject.imgh = atoi(pElement->FirstChildElement("imgh")->GetText())*TILESIZE;
            else
                newobject.imgh = TILESIZE;

            if(pElement->FirstChildElement("clipw") != NULL)
                newobject.clipw = atoi(pElement->FirstChildElement("clipw")->GetText())*TILESIZE;
            else
                newobject.clipw = newobject.imgw;

            if(pElement->FirstChildElement("cliph") != NULL)
                newobject.cliph = atoi(pElement->FirstChildElement("cliph")->GetText())*TILESIZE;
            else
                newobject.cliph = newobject.imgh;

            if(pElement->FirstChildElement("portal")!= NULL)
            {
                newobject.string_attribs["portal"] = pElement->FirstChildElement("portal")->GetText();
            }
            if(pElement->FirstChildElement("attributes")!=NULL)
            {
                XMLElement * listElement = pElement->FirstChildElement("attributes")->FirstChildElement();

                while(listElement)
                {
                    if(listElement->GetText())
                    {
                        if(is_number(listElement->GetText()))
                        {
                            /*if(listElement->Name()=="consume")
                                printf("nem stringet kap meg\n");
                            else
                                cout << listElement->Name() << endl;*/
                            newobject.int_attribs[listElement->Name()] = atoi(listElement->GetText());

                        }
                        else
                        {
                            //printf("megkapja a stringet\n");
                            newobject.string_attribs[listElement->Name()] = listElement->GetText();

                        }
                    }
                    else
                        newobject.int_attribs[listElement->Name()] = 1;
                    listElement = listElement->NextSiblingElement();
                }
            }
            if(pElement->FirstChildElement("drops")!=NULL)
            {
                XMLElement * listElement = pElement->FirstChildElement("drops")->FirstChildElement();

                while(listElement)
                {
                    newobject.drops.push_back(listElement->FirstChildElement("item")->GetText());
                    newobject.mins.push_back(atoi(listElement->FirstChildElement("min")->GetText()));
                    newobject.maxs.push_back(atoi(listElement->FirstChildElement("max")->GetText()));
                    listElement = listElement->NextSiblingElement();
                }
            }
            if(pElement->FirstChildElement("interactions"))
            {
                XMLElement* interaction = pElement->FirstChildElement("interactions")->FirstChildElement("i");

                while(interaction)
                {
                    Entity::Interaction intr;
                    intr.obj = interaction->FirstChildElement("obj")->GetText();
                    intr.script = interaction->FirstChildElement("script")->GetText();
                    newobject.interactions.push_back(intr);
                    interaction = interaction->NextSiblingElement();
                }
            }
            object_templates[newobject.name]=newobject;

            //object_ids[newobject.name]=id;

            pElement = pElement->NextSiblingElement("object");
        };

        printf("Finished loading objects!\n");
        return 1;
    }

    bool LoadRecipes()
    {
        printf("Loading recipes from XML list...\n");
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/xml/crafting.xml");

        XMLCheckResult(result);
        printf("XML file loaded..\n");

        XMLElement * pRoot = xmlDoc.FirstChildElement();
        if(pRoot == NULL)
        {
            printf("XML read error (root)\n");
            return 0;
        }
        XMLElement * pElement = pRoot->FirstChildElement();
        if(pElement == NULL)
        {
            printf("XML read error!\n");
            return 0;
        }
        int id;

        //item_templates.push_back(Item());

        while (pElement != NULL)
        {
            printf("Loading recipe\n");

            pElement->QueryIntAttribute("id", &id);

            CraftingRecipe newrecipe;

            newrecipe.item = pElement->FirstChildElement("item")->GetText();

            if(pElement->FirstChildElement("medium")!=NULL)
            {
                newrecipe.medium = pElement->FirstChildElement("medium")->GetText();
            }
            else
                newrecipe.medium = "";

            if(pElement->FirstChildElement("count")!=NULL)
                newrecipe.count = atoi(pElement->FirstChildElement("count")->GetText());
            else
                newrecipe.count = 1;

            if(pElement->FirstChildElement("level"))
            {
                newrecipe.level = atoi(pElement->FirstChildElement("level")->GetText());
            }
            else
            {
                newrecipe.level = 1;
            }
            if(pElement->FirstChildElement("unlock")!=NULL)
            {
                newrecipe.unlock = pElement->FirstChildElement("unlock")->GetText();
                newrecipe.unlocked = false;
            }
            else
            {
                newrecipe.unlock = "";
                newrecipe.unlocked = true;
            }
            if(pElement->FirstChildElement("skill")!=NULL)
                newrecipe.skill = pElement->FirstChildElement("skill")->GetText();
            else
                newrecipe.skill = "crafting";

            XMLElement * ingredient = pElement->FirstChildElement("ingredient");

            while(ingredient)
            {
                int _count=1;
                newrecipe.ingredients.push_back(ingredient->GetText());
                ingredient->QueryIntAttribute("count", &_count );
                if(_count)// != NULL)
                {
                    newrecipe.counts.push_back(_count);
                }
                else
                    newrecipe.counts.push_back(1);

                ingredient = ingredient->NextSiblingElement("ingredient");
            }

            recipes[newrecipe.item] = newrecipe;

            pElement = pElement->NextSiblingElement("recipe");
        };

        return 1;
    }



    bool LoadNPCs()
    {
        printf("Loading items from XML list...\n");
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/xml/npc.xml");

        XMLCheckResult(result);
        printf("XML file loaded..\n");

        XMLElement * pRoot = xmlDoc.FirstChildElement();
        if(pRoot == NULL)
        {
            printf("XML read error (root)\n");
            return 0;
        }
        XMLElement * pElement = pRoot->FirstChildElement();
        if(pElement == NULL)
        {
            printf("XML read error!\n");
            return 0;
        }
        int id;

        //npc_templates.push_back(NPC());

        while (pElement != NULL)
        {
            printf("Loading npc\n");

            pElement->QueryIntAttribute("id", &id);

            NPC newnpc;

            newnpc.name = pElement->FirstChildElement("name")->GetText();
            newnpc.fullname = pElement->FirstChildElement("fullname")->GetText();
            newnpc.img = images[ids[pElement->FirstChildElement("img")->GetText()]];
            newnpc.on_meet = atoi(pElement->FirstChildElement("onmeet")->GetText());
            newnpc.on_greet = atoi(pElement->FirstChildElement("ongreet")->GetText());
            newnpc.portrait = images[ids[pElement->FirstChildElement("portrait")->GetText()]];

            if(pElement->FirstChildElement("clipw") != NULL)
                newnpc.clipw = atoi(pElement->FirstChildElement("clipw")->GetText())*TILESIZE;
            else
                newnpc.clipw = TILESIZE;

            if(pElement->FirstChildElement("cliph") != NULL)
                newnpc.cliph = atoi(pElement->FirstChildElement("cliph")->GetText())*TILESIZE;
            else
                newnpc.cliph = TILESIZE;
            if(pElement->FirstChildElement("imgw") != NULL)
                newnpc.imgw = atoi(pElement->FirstChildElement("imgw")->GetText())*TILESIZE;
            else
                newnpc.imgw = TILESIZE;

            if(pElement->FirstChildElement("imgh") != NULL)
                newnpc.imgh = atoi(pElement->FirstChildElement("imgh")->GetText())*TILESIZE;
            else
                newnpc.imgh = TILESIZE;

            if(pElement->FirstChildElement("behaviour"))
                newnpc.behaviour=pElement->FirstChildElement("behaviour")->GetText();

            if(pElement->FirstChildElement("attributes"))
            {
                XMLElement * attrib = pElement->FirstChildElement("attributes")->FirstChildElement();
                while(attrib)
                {
                    if(attrib->GetText())
                    {
                        newnpc.attributes[attrib->Name()] = attrib->GetText();
                    }
                    else
                        newnpc.attributes[attrib->Name()] = "1";
                    attrib = attrib->NextSiblingElement();
                }
            }

            if(pElement->FirstChildElement("animations"))
            {
                XMLElement * anim = pElement->FirstChildElement("animations")->FirstChildElement();

                while(anim)
                {
                    newnpc.alts[anim->Name()]=images[ids[anim->GetText()]];

                    anim = anim->NextSiblingElement();
                }
            }

            if(pElement->FirstChildElement("width"))
                newnpc.w = atof(pElement->FirstChildElement()->GetText())*TILESIZE;
            else
                newnpc.w = TILESIZE;

            if(pElement->FirstChildElement("height"))
                newnpc.h = atof(pElement->FirstChildElement()->GetText())*TILESIZE;
            else
                newnpc.h = TILESIZE;


            if(pElement->FirstChildElement("alt"))
                newnpc.alt = atof(pElement->FirstChildElement("alt")->GetText())*TILESIZE;
            npc_templates[newnpc.name] = newnpc;

            //npc_ids[newnpc.name]=id;

            pElement = pElement->NextSiblingElement("npc");
        };

        return 1;
    }
    bool LoadItems()
    {
        printf("Loading items from XML list...\n");
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/xml/item.xml");

        XMLCheckResult(result);
        printf("XML file loaded..\n");

        XMLElement * pRoot = xmlDoc.FirstChildElement();
        if(pRoot == NULL)
        {
            printf("XML read error (root)\n");
            return 0;
        }
        XMLElement * pElement = pRoot->FirstChildElement();
        if(pElement == NULL)
        {
            printf("XML read error!\n");
            return 0;
        }
        int id;

        //item_templates.push_back(Item());

        while (pElement != NULL)
        {
            printf("Loading item\n");

            pElement->QueryIntAttribute("id", &id);

            Item newitem;

            newitem.name = pElement->FirstChildElement("name")->GetText();
            newitem.formal = pElement->FirstChildElement("formal")->GetText();
            newitem.img = images[ids[pElement->FirstChildElement("img")->GetText()]];
            newitem.max_stack = atoi(pElement->FirstChildElement("stack")->GetText());
            newitem.category = pElement->FirstChildElement("category")->GetText();
            newitem.type = pElement->FirstChildElement("type")->GetText();
            if(pElement->FirstChildElement("desc")!=NULL)
                newitem.desc = pElement->FirstChildElement("desc")->GetText();
            else
                newitem.desc = "";
            if(pElement->FirstChildElement("place")!=NULL)
            {
                newitem.string_attribs["place"] = pElement->FirstChildElement("place")->GetText();
            }

            if(pElement->FirstChildElement("range")!=NULL)
                newitem.int_attribs["range"] = atoi(pElement->FirstChildElement("range")->GetText())*TILESIZE;
            else if(newitem.category == "tool")
                newitem.int_attribs["range"] = 2*TILESIZE;

            if(pElement->FirstChildElement("price")!=NULL)
                newitem.int_attribs["price"] = atoi(pElement->FirstChildElement("price")->GetText());
            if(pElement->FirstChildElement("sell")!=NULL)
                newitem.int_attribs["sell"] = atoi(pElement->FirstChildElement("sell")->GetText());
            if(pElement->FirstChildElement("attributes")!=NULL)
            {
                XMLElement * listElement = pElement->FirstChildElement("attributes")->FirstChildElement();

                while(listElement)
                {
                    if(listElement->GetText())
                    {
                        if(is_number(listElement->GetText()))
                        {
                            /*if(listElement->Name()=="consume")
                                printf("nem stringet kap meg\n");
                            else
                                cout << listElement->Name() << endl;*/
                            newitem.int_attribs[listElement->Name()] = atoi(listElement->GetText());

                        }
                        else
                        {
                            //printf("megkapja a stringet\n");
                            newitem.string_attribs[listElement->Name()] = listElement->GetText();

                        }
                    }
                    else
                        newitem.int_attribs[listElement->Name()] = 1;
                    listElement = listElement->NextSiblingElement();
                }
            }
            //item_ids[newitem.name]=id;

            item_templates[newitem.name] = newitem;

            pElement = pElement->NextSiblingElement("item");
        };

        return 1;
    }
    void AddDrawable(Entity * e)
    {
        int id = newid();
        e->id = id;
        draw_list.push_back(e);

    }
    void SwitchStage(Stage * newstage)
    {
        currentstage = newstage;

        currentstage->ScanGrid();

        draw_list.clear();

        for(auto& e : currentstage->objects)
            if(e)
                AddDrawable(e);

        //drawables.push_back(&ptr_to_player);
        AddDrawable(player);
        AddDrawable(preview);

        /*for(int i = 0; i < currentstage->npcs.size(); i++)
        {
            currentstage->ptr_to_npcs.push_back(currentstage->npcs[i]);
            drawables.push_back(&currentstage->ptr_to_npcs[i]);

        }*/
        for(auto& e : currentstage->npcs)
            if(e)
                AddDrawable(e);

        std::sort(draw_list.begin(), draw_list.end(), Sorter);

        FocusCamera();

        if(!newstage->indoors)
            weathersys.active = true;
        else
            weathersys.active = false;

    }
    virtual void Init()
    {
    }
    void CreateNew()
    {

        //Load();

        LoadItems();
        LoadDialogs();
        LoadObjects();
        LoadNPCs();
        LoadRecipes();

        player = new Player(Creature(im("girl"),8*TILESIZE,6*TILESIZE,TILESIZE,TILESIZE,TILESIZE));
        ptr_to_player = player;

        player_lot.name = "lot";
        currentstage = &player_lot;
        player_house.House();
        player_house.name = "house";
        forest.Forest();
        forest.name = "forest";
        general_store.Shop();
        general_store.name = "shop";

        limbo.Limbo();
        limbo.name = "limbo";

        cursor = new Cursor();
        preview = new Preview();
        eventSystem = new EventSystem;
        timedisplay = new TimeDisplay(eventSystem);

        lightsys = LightingSystem();
        particlesys = ParticleSystem();
        weathersys = WeatherSystem(&particlesys);

        inventory = new Inventory();

        gui = new InventoryGui();

        //currentstage->objects.push_back(new Entity(object_templates["house"],TILESIZE/2,TILESIZE*2));

        PlaceObject(new Entity(object_templates["house"]),0,2);
        PlaceObject(new Entity(object_templates["shop"]),7,2);

        for(int i = 0; i < 100; i++)
            SpawnObject(new Entity(object_templates["tree"],0,0),0,0,100,100);//currentstage->objects.push_back(new Entity(object_templates["tree"],(rand()%(100)*TILESIZE+TILESIZE), (rand()%(100)*TILESIZE+TILESIZE)));

        for(int i = 0; i < 10; i++)
            SpawnObject(new Entity(object_templates["lily"],0,0),0,0,SCREEN_WIDTH/TILESIZE,SCREEN_HEIGHT/TILESIZE);//currentstage->objects.push_back(new Entity(object_templates["lily"], rand()%(SCREEN_WIDTH-TILESIZE)+TILESIZE, rand()%(SCREEN_HEIGHT-TILESIZE)+TILESIZE));

        for(int i = 0; i < 10; i++)
            SpawnObject(new Entity(object_templates["grass"],0,0),0,0,SCREEN_WIDTH/TILESIZE,SCREEN_HEIGHT/TILESIZE);//currentstage->objects.push_back(new Entity(object_templates["grass"], rand()%(SCREEN_WIDTH-TILESIZE)+TILESIZE, rand()%(SCREEN_HEIGHT-TILESIZE)+TILESIZE));

        for(int i = 0; i < 5; i++)
            SpawnObject(new Entity(object_templates["tree"],0,0),0,0,SCREEN_WIDTH/TILESIZE,SCREEN_HEIGHT/TILESIZE);//currentstage->objects.push_back(new Entity(object_templates["tree"],(rand()%((SCREEN_WIDTH-TILESIZE)/TILESIZE))*TILESIZE+TILESIZE, (rand()%((SCREEN_HEIGHT-TILESIZE)/TILESIZE))*TILESIZE+TILESIZE));

        for(int i = 0; i < 5; i++)
            SpawnObject(new Entity(object_templates["stone"],0,0),0,0,SCREEN_WIDTH/TILESIZE,SCREEN_HEIGHT/TILESIZE);
        for(int i = 0; i < 100; i++)
            SpawnObject(new Entity(object_templates["grass"],0,0),0,0,100,100);//currentstage->objects.push_back(new Entity(object_templates["grass"],(rand()%(100)*TILESIZE+TILESIZE), (rand()%(100)*TILESIZE+TILESIZE)));
        for(int i = 0; i < 120; i++)
        {
            SpawnObject(new Entity(object_templates["poppy"],0,0),0,0,100,100);//currentstage->objects.push_back(new Entity(object_templates["flower"],(rand()%(100)*TILESIZE+TILESIZE), (rand()%(100)*TILESIZE+TILESIZE)));
            //currentstage->objects[currentstage->objects.size()-1]->img = images[ids["flower" + to_string(2+rand()%4)]];//rand()%2 == 1 ? "flower" : "flower2"]];
        }
        SpawnObjects("lily",0,0,100,100,80);
        SpawnObjects("tulip",0,0,100,100,80);
        SpawnObjects("rose",0,0,100,100,80);


        PlaceObject(new Entity(Door("forest",10,2,8,4)),16,2);


        //currentstage->objects.push_back(new Entity(object_templates["distiller"],TILESIZE*4,TILESIZE*6));
        PlaceOverride(new Entity(object_templates["distiller"]),4,6);

        for(auto x : currentstage->objects)
            if(Intersect(x,player))
            {
                remove_entity(x);

            }

        currentstage->npcs.push_back(new NPC(npc_templates["spock"],18*TILESIZE,12*TILESIZE));

        currentstage->npcs.push_back(new NPC(npc_templates["crypto"],14*TILESIZE,10*TILESIZE));

        currentstage->npcs.push_back(new NPC(npc_templates["platypus"],10*TILESIZE,6*TILESIZE));


        dialogSystem = new DialogSystem();

        gamephase = DIALOG;
        dialogSystem->Add(dialogs[22]);

        inventory->Add(item_templates["axe"]);
        inventory->Add(item_templates["pickaxe"]);
        inventory->Add(item_templates["hoe"]);
        inventory->Add(item_templates["scythe"]);
        inventory->Add(item_templates["can"]);
        //inventory->Add(item_templates["apple"],30);
        //inventory->Add(item_templates["platty"]);
        //inventory->Add(item_templates["jar"],30);
        //inventory->Add(item_templates["skillbook"]);

        /*for(int i = 0; i < 99; i++)
            inventory->Add(item_templates["seed"]);*/

        stageList[to_string("lot")] = &player_lot;
        stageList[to_string("house")] = &player_house;
        stageList["forest"] = &forest;
        stageList["shop"] = &general_store;
        stageList["limbo"] = &limbo;

        currentstage = (stageList["lot"]);

        SwitchStage(stageList["lot"]);

    }

    bool Load()
    {
        LoadItems();
        LoadDialogs();
        LoadObjects();
        LoadNPCs();
        LoadRecipes();
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/save/save.xml");

        XMLCheckResult(result);
        printf("XML file loaded..\n");

        XMLElement * pRoot = xmlDoc.FirstChildElement();
        if(pRoot == NULL)
        {
            printf("XML read error (root)\n");
            return 0;
        }
        XMLElement * pElement = pRoot->FirstChildElement("year");
        if(pElement == NULL)
        {
            printf("XML read error!\n");
            return 0;
        }

        current_year = atoi(pElement->GetText());
        current_season = atoi(pRoot->FirstChildElement("season")->GetText());
        current_day = atoi(pRoot->FirstChildElement("day")->GetText());
        funds = atoi(pRoot->FirstChildElement("funds")->GetText());
        string player_pos = pRoot->FirstChildElement("player")->GetText();

        player = new Player(Creature(im("girl"),8*TILESIZE,6*TILESIZE,TILESIZE,TILESIZE));
        player->x = atoi(FirstWord(player_pos).c_str());
        player->y = atoi(SecondWord(player_pos).c_str());

        pElement = pRoot->FirstChildElement("toolbar");

        int item_index = 0;

        inventory = new Inventory();

        printf("Loading inventory\n");
        while(pElement)
        {
            printf("Loading toolbar item.. \n");
            string item = pElement->GetText();

            if(item == "blank")
                inventory->toolbar[item_index] = NULL;
            else
            {
                //cout << sdsds->name << endl;

                inventory->toolbar[item_index] = new Item( item_templates[FirstWord(item)]);

                inventory->toolbar[item_index]->quantity = atoi(SecondWord(item).c_str());

            }

            pElement = pElement->NextSiblingElement("toolbar");
            item_index++;
        }
        pElement = pRoot->FirstChildElement("item");

        item_index = 0;
        while(pElement)
        {
            printf("Loading inventory item.. \n");
            string item = pElement->GetText();
            if(item == "blank")
                inventory->items[item_index] = NULL;
            else
            {
                inventory->items[item_index] = new Item(item_templates[FirstWord(item)]);

                inventory->items[item_index]->quantity = atoi(SecondWord(item).c_str());

            }


            pElement = pElement->NextSiblingElement("item");
            item_index++;
        }
        printf("Finished loading inventory \n");

        pElement = pRoot->FirstChildElement("skills");
        if(pElement)
            pElement = pElement->FirstChildElement();

        while(pElement)
        {
            skills[pElement->Name()]=atoi(pElement->GetText());
            pElement = pElement->NextSiblingElement();
        }


        pElement = pRoot->FirstChildElement("stage");
        printf("Loading stages\n");
        while(pElement)
        {
            printf("Loading stage.. \n");
            Stage * new_stage = new Stage();

            new_stage->name = pElement->FirstChildElement("name")->GetText();
            stageNames.push_back(new_stage->name);

            string sizes = pElement->FirstChildElement("size")->GetText();
            new_stage->w = new_stage->tilemap.w = atoi(FirstWord(sizes).c_str());
            new_stage->h = new_stage->tilemap.h =atoi(SecondWord(sizes).c_str());

            if(pElement->FirstChildElement("indoors"))
                new_stage->indoors = true;

            XMLElement * rowElement = pElement->FirstChildElement("row");
            int height = 0;
            while(rowElement)
            {
                std::stringstream ss;

                string row = rowElement->GetText();

                ss << row;

                fr(0,new_stage->w)
                    {
                        string current;
                        ss >> current;
                        new_stage->tilemap.tile[i][height] = atoi(current.c_str());
                    }

                rowElement = rowElement->NextSiblingElement("row");
                height++;
            }

            XMLElement * objectElement = pElement->FirstChildElement("object");

            while(objectElement)
            {
                Entity * new_object;

                string name = objectElement->FirstChildElement("name")->GetText();
                string position = objectElement->FirstChildElement("pos")->GetText();

                if(name != "door")
                {
                    new_object = new Entity(object_templates[name],atoi(FirstWord(position).c_str()),atoi(SecondWord(position).c_str()));


                }
                else
                {
                    new_object = new Entity(Door("lot",atoi(FirstWord(position).c_str()),atoi(SecondWord(position).c_str())));

                }


                XMLElement* attributeElement = NULL;
                if(objectElement->FirstChildElement("attributes") != NULL)
                    attributeElement = objectElement->FirstChildElement("attributes")->FirstChildElement();

                while(attributeElement)
                {
                    string name = attributeElement->Name();
                    string value = attributeElement->GetText();

                    if(is_number(value))
                        new_object->int_attribs[name] = atoi(value.c_str());
                    else
                        new_object->string_attribs[name] = value;

                    attributeElement = attributeElement->NextSiblingElement();
                }


                new_stage->Add(new_object);
                new_stage->objects.push_back(new_object);

                objectElement = objectElement->NextSiblingElement("object");
            }
            XMLElement * npcElement = pElement->FirstChildElement("npc");

            while(npcElement)
            {
                NPC * new_object;

                string name = npcElement->FirstChildElement("name")->GetText();
                string position = npcElement->FirstChildElement("pos")->GetText();

                new_object = new NPC(npc_templates[name],atoi(FirstWord(position).c_str()),atoi(SecondWord(position).c_str()));

                if(npcElement->FirstChildElement("met"))
                    new_object->met = true;

                new_stage->npcs.push_back(new_object);


                npcElement = npcElement->NextSiblingElement("npc");
            }

            stageList[new_stage->name] = new_stage;
            pElement = pElement->NextSiblingElement("stage");

        }

        currentstage = stageList["house"];


        cursor = new Cursor();

        eventSystem = new EventSystem;
        timedisplay = new TimeDisplay(eventSystem);

        lightsys = LightingSystem();
        particlesys = ParticleSystem();
        weathersys = WeatherSystem(&particlesys);


        gui = new InventoryGui();

        dialogSystem = new DialogSystem();

        preview = new Preview();

        SwitchStage(stageList["house"]);

        //gamephase = CONTROL;
        gamephase = DIALOG;
        dialogSystem->Add(dialogs[1]);

        fade_progress = FADE_MAX;
        fade_direction = FADE_OUT;

        printf("Done loading!\n");
        cout << "Loaded " << images.size() << " images\n";
        cout << "Loaded " << audioFiles << " audio files\n";
        cout << "Counted " << object_templates.size() << " objects\n";
        cout << "Counted " << item_templates.size() << " items\n";
        cout << "Counted " << npc_templates.size() << " NPCs\n";
        cout << "Counted " << recipes.size() << " crafting recipes\n";
        cout << "Counted " << dialogs.size() << " dialogs\n";
    }

    void HandleRightClick()
    {
        bool interacted = false;

            for(auto &n : currentstage->npcs)
            {
                if(n)
                {
                    if(KeyData.MouseX+camera_x >= n->x && KeyData.MouseX+camera_x <= n->x + n->w &&
                        KeyData.MouseY+camera_y >= n->y && KeyData.MouseY+camera_y <= n->y + n->h  )
                        {
                            /*float dx = player->x - n->x;
                            float dy = player->y - n->y;
                            float distance = sqrt(dx*dx+dy*dy);*/

                            if(GetDistance(player->x,player->y,n->x,n->y) < globals["interact-range"])
                            {
                                eventSystem->Event("TALK");

                                gamephase = DIALOG;

                                if(n->attributes.count("behaviour-talk"))
                                {
                                    if(n->attributes["behaviour-talk"]=="random")
                                    {
                                        vector<string> lines = allWord(n->attributes["lines"]);

                                        int randomLineIndex = rand()%lines.size();

                                        dialogSystem->Add(dialogs[atoi(lines[randomLineIndex].c_str())],n->name);

                                        cout << atoi(lines[randomLineIndex].c_str()) << endl;
                                    }
                                }
                                else
                                {
                                    if(!n->met)
                                        dialogSystem->Add(dialogs[n->on_meet],n->name);
                                    else
                                        dialogSystem->Add(dialogs[n->on_greet],n->name);

                                }

                                dialogSystem->portrait = n->portrait;
                                n->met = true;
                                interacted = true;
                                break;
                            }
                        }
                }
            }
            if(!interacted)
            for(auto& e : currentstage->objects)
            {
                if(e)
                {
                    if(e->int_attribs.count("interact"))
                        if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y)&& GetDistance(player->x,player->y,e->x,e->y) < globals["interact-range"])

                    {
                        gamephase = DIALOG;

                        dialogSystem->Add(dialogs[e->int_attribs["interact"]]);

                        interacted = true;
                        break;
                    }
                    if(e->pickable)
                    {
                        if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y)&& GetDistance(player->x,player->y,e->x,e->y) < globals["interact-range"])
                        {
                            /*float dx = player->x - e->x;
                            float dy = player->y - e->y;
                            float distance = sqrt(dx*dx+dy*dy);
                            */
                                inventory->Add(item_templates[e->pick]);
                                //e = NULL;
                                interacted = true;
                                particlesys.Add(e->x,e->y,e->w,e->h,e->img,1,g("particle-decay"));
                                remove_entity(e);
                                break;

                        }
                    }
                    else if(e->type == "crafting")
                    {
                     if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y) && GetDistance(player->x,player->y,e->x,e->y) < globals["interact-range"])
                        {

                            gamephase = GUI;
                            gui = new CraftingGui(inventory,e->name);
                            interacted = true;
                            break;
                        }

                    }
                    else if(e->string_attribs.count("portal"))
                    {
                        if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y) && GetDistance(player->x,player->y,e->x+e->w/2,e->y+e->h/2) < globals["interact-range"])
                        {

                            if(e->string_attribs.count("where"))
                            {
                                fade_x = atoi(FirstWord(e->string_attribs["where"]).c_str());
                                fade_y = atoi(SecondWord(e->string_attribs["where"]).c_str());
                            }
                            else
                                fade_x = fade_y = 0;

                            /*if(e->string_attribs["portal"] == "house")
                                SwitchStage(stageList["house"]);
                            else if(e->string_attribs["portal"] == "shop")
                            {
                                SwitchStage(stageList["shop"]);
                                FocusCamera();
                            }
                            else if(e->string_attribs["portal"] == "lot")
                                SwitchStage(stageList["lot"]);
                            else
                                SwitchStage(stageList["limbo"]);*/

                            gamephase = FADE;
                            fade_stage = stageList[e->string_attribs["portal"]];
                            fade_direction = FADE_IN;


                            interacted = true;
                            break;
                        }
                    }

                }
            }


            if(!interacted)
                if(inventory->toolbar[inventory->selected])
                    if(inventory->toolbar[inventory->selected]->int_attribs.count("edible") || inventory->toolbar[inventory->selected]->int_attribs.count("drinkable"))
                    {
                        eventSystem->Event("EAT");
                        inventory->Remove(inventory->selected);
                        hunger -= HUNGER_SCALE;
                        if(hunger < 0)
                            hunger = 0;
                    }
                    else if(inventory->toolbar[inventory->selected]->string_attribs.count("consume"))
                    {
                        RunScript(inventory->toolbar[inventory->selected]->string_attribs["consume"]);
                        inventory->Remove(inventory->selected);
                    }
                    else if(inventory->toolbar[inventory->selected]->name == "god")
                    {
                        inventory->toolbar[inventory->selected]->string_attribs["place"] = getNextEntityName();
                        preview->Add(object_templates[object_names[selectedEntity]]);
                    }
    }

    void HandleClick()
    {
        int mx = KeyData.MouseX, my = KeyData.MouseY;

        if(Contains(inventory,mx,my,0,0))
        {
            inventory->HandleClick(mx,my);
            return;
        }

        int click_x = std::floor((KeyData.MouseX+camera_x)/TILESIZE);
        int click_y = std::floor((KeyData.MouseY+camera_y)/TILESIZE);
        Item * sel = inventory->toolbar[inventory->selected];

        if(sel)
        for(auto& e : currentstage->objects)
        if(e)
        if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y))
        if(e->Interacts(sel)!=-1)
            RunScript(e->interactions[e->Interacts(sel)].script,&e,sel->name);

        if(sel)
        if(sel->string_attribs.count("place"))//category == "organic")
                {
                    //if(inventory->toolbar[inventory->selected]->type == "seed")
                    int place_x = std::floor((KeyData.MouseX+camera_x)/TILESIZE);
                    int place_y = std::floor((KeyData.MouseY+camera_y)/TILESIZE);
                    Entity * newe = new Entity(object_templates[inventory->toolbar[inventory->selected]->string_attribs["place"]],place_x*TILESIZE,place_y*TILESIZE);
                    if(GetDistance(player->x+player->w/2,player->y+player->h/2,newe->x + newe->w/2,newe->y + newe->h/2) < sel->int_attribs["range"] + newe->w/2)
                    {

                        //currentstage->objects.push_back(newe);
                        if(inventory->toolbar[inventory->selected]->category != "organic" || ((!inventory->toolbar[inventory->selected]->int_attribs.count("needs_soil") && currentstage->IsGround(place_x,place_y)) || currentstage->CanPlant(place_x, place_y)))
                        if((!newe->rigid || !Intersect(newe,player)) && PlaceObject(newe,place_x,place_y))
                        {
                            particlesys.Add(newe->x,newe->y,newe->w,newe->h,images[ids["dust"]],1,globals["particle-decay"]);
                            eventSystem->Event("PLACE",{newe->category});

                            AddDrawable(newe);

                            if(sel->name != "god")
                                inventory->Remove(inventory->selected);

                        }
                    }
                }
        else if(sel->category == "tool")
        {
            for(auto& e : currentstage->objects)
            //auto &e = currentstage->grid[click_x][click_y];
                if(e)
                    //if(KeyData.MouseX+camera_x >= e->x && KeyData.MouseX+camera_x <= e->x + e->w &&
                    //       KeyData.MouseY+camera_y >= e->y && KeyData.MouseY+camera_y <= e->y + e->h  )
                    if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y)){
                        if(sel->type == "watering")
                        {
                            if(e->category == "plant" && e->int_attribs.count("needs_water"))
                            {
                                e->int_attribs["watered"] = 1;
                                particlesys.Add(e->x,e->y,e->w,e->h,images[ids["water"]],1,globals["particle-decay"]);
                                e->Shake();
                            }

                        }
                        else
                        {
                            if((e->destroy != "" || sel->name == "delete")
                               && (sel->type == e->destroy || sel->type == "omni" || e->destroy == "any"))
                               //&& (e->drops.size()>0 )
                                {
                                    float dx = player->x - e->x;
                                    float dy = player->y - e->y;
                                    float distance = sqrt(dx*dx+dy*dy);

                                    int strength = 1;
                                    if(sel->int_attribs.count("strength"))
                                        strength = sel->int_attribs["strength"];

                                    if(distance < sel->int_attribs["range"])//TILESIZE*2)
                                        if(e->health <= strength)
                                        {
                                            eventSystem->Event("DESTROY", {e->name});

                                            if(sel->name != "delete")
                                            for(int i = 0; i < e->drops.size(); i++)
                                            {

                                                int quantity =(e->maxs[i] == e->mins[i] ? e->mins[i] : rand()%(e->maxs[i]-e->mins[i])+e->mins[i]);


                                                inventory->Add(item_templates[e->drops[i]], quantity);

                                            }

                                            if(e->leftover == "" || sel->name == "delete")
                                            {
                                                particlesys.Add(e->x,e->y,e->w,e->h,e->img,1,globals["particle-decay"]);
                                                remove_entity(e);
                                                //cout << e << " " << currentstage->grid[click_x][click_y] << endl;

                                            }
                                            else
                                            {
                                                particlesys.Add(e->x,e->y,e->w,e->h,e->img,1,globals["particle-decay"]);
                                                change_entity(e,new Entity(object_templates[e->leftover],e->x,e->y));
                                                //e = new Entity(object_templates[e->leftover],e->x,e->y);
                                            }

                                            //break;
                                        }
                                        else
                                        {
                                            e->health -= strength;
                                            e->Shake();
                                        }
                                }
                        }
                    }

                if(sel->type == "tilling" && (sel->name == "terra" || (currentstage->IsGround(click_x,click_y) && currentstage->IsFree(click_x,click_y) && GetDistance(player->x + player->w/2,player->y + player->h/2,KeyData.MouseX+camera_x,KeyData.MouseY+camera_y) < TILESIZE*2)))
                {
                    particlesys.Add(click_x*TILESIZE,click_y*TILESIZE,TILESIZE,TILESIZE,images[ids["dust"]],1,globals["particle-decay"]);
                    if(sel->name == "terra")
                        currentstage->Set(click_x,click_y,currentstage->Get(click_x,click_y)+1);
                    else
                        currentstage->Set(click_x,click_y,Stage::TILLED);
                }
        }
        else if(sel->category == "organic")
        {

        }

    }
    void NextDay(Stage* targetStage)
    {

        //cout << "Current day: " << current_day << endl;
        vector<Entity *> spawns;

        for(auto& e : targetStage->objects)
            if(e)
                if(e->category == "plant")
                    //if(e->type == "plant")
                    if(!e->int_attribs.count("needs_water") || (e->int_attribs.count("watered") && e->int_attribs["watered"] == 1))
                    {
                        if(e->int_attribs.count("watered") && e->int_attribs["watered"] == 1)
                            e->int_attribs["watered"]  = 0;
                        //cout << e->int_attribs["cycle"] << endl;

                        if(e->int_attribs.count("growth") && e->string_attribs.count("ripe")  )
                        {
                            e->int_attribs["growth"]++;
                            if(e->int_attribs["growth"] >= e->int_attribs["cycle"]-1)
                            {
                                //e = object_templates[e->string_attribs["ripe"]];
                                if(targetStage == currentstage)
                                    change_entity(e, new Entity(object_templates[e->string_attribs["ripe"]],e->x,e->y));
                                else
                                    e = new Entity(object_templates[e->string_attribs["ripe"]],e->x,e->y);
                            }
                            //printf("sziassss");
                        }
                        /*if(e->name == "grass")
                        {
                            spawns.push_back(new Entity(object_templates["grass"],e->x + 60 - rand()%120, e->y + 60 - rand()%120));
                        }*/
                    }

        //cout << currentstage->objects.size() << endl;
        for(auto& e : spawns)
        {
            targetStage->objects.push_back(e);
            if(targetStage == currentstage)
                AddDrawable(e);
        }


    }
    void NextDay()
    {

        current_day++;
        if(current_day > 30)
        {
            current_day = 1;
            current_season++;
        }
        if(current_season > 4)
        {
            current_season = 1;
            current_year++;
        }

        weathersys.weather = (rand()%100<globals["rain-probability"]?"rain":"none");

        for(auto& e : stageList)
            NextDay(e.second);
    }
    void FocusCamera()
    {
        int oldx = camera_x, oldy = camera_y;

        camera_x = (player->x - SCREEN_WIDTH/2);
        camera_y = (player->y - SCREEN_HEIGHT/2);


        if(currentstage->tilemap.w*TILESIZE > SCREEN_WIDTH && camera_x > currentstage->tilemap.w*TILESIZE-SCREEN_WIDTH/2)
            camera_x = currentstage->tilemap.w*TILESIZE-SCREEN_WIDTH/2;
        if(currentstage->tilemap.h*TILESIZE > SCREEN_HEIGHT && camera_y > currentstage->tilemap.h*TILESIZE-SCREEN_HEIGHT/2)
            camera_y = currentstage->tilemap.h*TILESIZE-SCREEN_HEIGHT/2;

        if(camera_x < -SCREEN_WIDTH/2)
            camera_x = -SCREEN_WIDTH/2;
        if(camera_y < -SCREEN_HEIGHT/2)
            camera_y = -SCREEN_HEIGHT/2;

        ///ha a map kisebb mint a kepernyo a kamera nem mozog
        if(currentstage->tilemap.w*TILESIZE < SCREEN_WIDTH)
            camera_x = currentstage->tilemap.w*TILESIZE/2-SCREEN_WIDTH/2;
        if(currentstage->tilemap.h*TILESIZE < SCREEN_HEIGHT)
            camera_y = currentstage->tilemap.h*TILESIZE/2-SCREEN_HEIGHT/2;

        camera_xvel = camera_x-oldx;
        camera_yvel = camera_y-oldy;
    }

    bool ValidatePlayer(int x, int y)
    {
        //if(currentstage->tilemap.w*TILESIZE > SCREEN_WIDTH && x > currentstage->tilemap.w*TILESIZE-SCREEN_WIDTH)
        if(x > currentstage->tilemap.w*TILESIZE-TILESIZE)
           return false;
        if(y > currentstage->tilemap.h*TILESIZE-TILESIZE)
            return false;

        if(x < 0)
            return false;
        if(y < 0)
            return false;

        return true;
    }
    void ControlPhase()
    {
        if(alerts.size()>0)
        {
            Alert(alerts.front());
            alerts.pop();
            return;
        }

        current_seconds++;
        if(current_seconds > 119)
        {
            current_seconds = 0;
            current_minute++;
            if(hunger < MAX_HUNGER_LEVEL*HUNGER_SCALE-1)
                hunger++;
            if(current_minute > 59)
            {
                current_minute = 0;
                current_hour++;
                if(current_hour > 23)
                    current_hour = 0;
            }
        }

        for(auto& n : currentstage->npcs)
        {
            Action(n,n->AI());
        }
        float tempx = player->x, tempy = player->y;

        if(KeyData.wheel)
        {
            inventory->selected -= KeyData.wheel;
            if(inventory->selected < 0)
                inventory->selected = 9;

            else if(inventory->selected > 9)
                inventory->selected = 0;
            if(inventory->toolbar[inventory->selected])
            {
                if(inventory->toolbar[inventory->selected] && inventory->toolbar[inventory->selected]->string_attribs.count("place"))
                {
                    preview->Add(object_templates[inventory->toolbar[inventory->selected]->string_attribs["place"]]);

                }
                else if(inventory->toolbar[inventory->selected]->type == "watering")
                {
                    //preview = new Preview(ent)
                    //preview->active = true;
                }
                else
                    preview->Disable();
            }
            else
            {
                preview->Disable();

            }

        }
        if(KeyData.MouseMove)
        {
            cursor->text = "";
            cursor->state = Cursor::NORMAL;

            preview->Move(KeyData.MouseX,KeyData.MouseY);

            if(Contains(inventory,KeyData.MouseX,KeyData.MouseY,0,0))
            {
                if((int)floor((KeyData.MouseX-inventory->x)/TILESIZE) < INVENTORY_TOOLBAR_SIZE)
                if(inventory->toolbar[(int)floor((KeyData.MouseX-inventory->x)/TILESIZE)])
                    cursor->text = inventory->toolbar[(int)(KeyData.MouseX-inventory->x)/TILESIZE]->formal;
            }
            else
            {
                for(auto& e : currentstage->objects)
                if(e)
                {
                    if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y))
                    {
                        if(e->pickable)
                        {
                            cursor->state = Cursor::PICK;
                        }
                        else if(e->destroy != "")
                        {
                            cursor->state = Cursor::INTERACT;
                        }
                    }
                }

                for(auto& e : currentstage->npcs)
                    if(e)
                    {
                        if(Contains(e,KeyData.MouseX,KeyData.MouseY,camera_x,camera_y))
                        {
                            cursor->state = Cursor::TALK;
                        }
                    }
            }

        }
        bool moved = false;

        if(KeyData.lastv  && KeyData.getKey(1) && KeyData.lasth && KeyData.getKey(0))
        {
            //player->movement_speed /= 1.5;
        }
        for(int i = 0; i < abs(player->movement_speed)*2; i++)
        {
            tempx = player->x;
            tempy = player->y;

            if(i < abs(player->movement_speed))
            if(KeyData.lasth && KeyData.getKey(0))
            {
                tempx += KeyData.getDir(KeyData.lasth)*1;//player->movement_speed;
                if(KeyData.getDir(KeyData.lasth) < 0)
                    player->lastdir = 1;
                else
                    player->lastdir = 2;
                moved = true;
            }
            if(i >= abs(player->movement_speed))
            if(KeyData.lastv  && KeyData.getKey(1))
            {
                tempy += KeyData.getDir(KeyData.lastv)*1;//player->movement_speed;
                if(KeyData.getDir(KeyData.lastv) < 0)
                    player->lastdir = 3;
                else
                    player->lastdir = 0;
                moved = true;
            }
            if(KeyData.lastv  && KeyData.getKey(1) && KeyData.lasth && KeyData.getKey(0))
            {
                //player->movement_speed *= 1.5;
            }
            bool canMove = true;

            canMove = ValidatePlayer(tempx,tempy);

            if(canMove)
            for(auto& e : currentstage->objects)
            {
                /*for(int i = 0 ; i < 2; i++)
                    for(int j = 0; j < 2; j++)
                {
                    int gridx = std::floor((player->x+i*player->w)/TILESIZE);
                    int gridy = std::floor((player->y+j*player->h)/TILESIZE);

                    Entity * e = NULL;
                    if(gridx < currentstage->w && gridy < currentstage->h)
                        currentstage->grid[gridx][gridy];*/

                    if(e)
                    {
                        int saveX = player->x;
                        int saveY = player->y;

                        player->x = tempx;
                        player->y = tempy;

                        if(Intersect(e,player))
                            //if(PixelPerfect(e,player))
                        {
                            if(e->rigid)
                                canMove = false;
                            else
                            {
                                if(player->x != saveX || player->y != saveY)
                                    if(e->category == "plant")
                                        e->Shake(0);

                            }
                        }


                        player->x = saveX;
                        player->y = saveY;

                    }
                //}

            }
            //if(currentstage->tilemap.tile[tempx/TILESIZE][tempy/TILESIZE] == 0)
                if(canMove)
                {
                    player->x = tempx;
                    player->y = tempy;
                }
        }
        if(moved)
        {
            int psize = globals["particle-size"];
            image targetParticle = images[ids[(currentstage->IsGround(player->x/TILESIZE,player->y/TILESIZE)?"particle_grass":"dust")]];
            particlesys.Add(player->x+player->w/2-psize/2,player->y+player->h-psize/2,psize,psize,targetParticle);
            player->current_frame += 0.2;
            if(player->current_frame >= g("player-anim-frames"))
                player->current_frame = 0;

            FocusCamera();

        }
        if(player->y < -player->h)
        {
            SwitchStage(&forest);
            player->x = player->y = SCREEN_HEIGHT/2;
        }
        for(auto& e : currentstage->objects)
        {
            if(e && e->alt > 0)
            {
                if(e->x - e->offset/2 > player->x + player->w || e->x + e->w + e->offset/2 < player->x
                    || e->y - e->alt > player->y + player->h || e->y < player->y)
                    {
                        e->fade = false;
                    }
                    else
                    {
                        e->fade = true;
                    }
            }
        }
        if(KeyData.EscapePress)
        {
            gamephase = DIALOG;
            dialogSystem->Add(Prompt("Are you sure you want to quit?","exit"));
        }

        for(int i = 0; i < 10; i++)
        {
            if(KeyData.numPress[i] && stageNames.size() > i)
            {
                SwitchStage(stageList[stageNames[i]]);
                player->x = 6*TILESIZE;
                player->y = 4*TILESIZE;
                FocusCamera();
            }
        }
        if(KeyData.KPress)
        {
            NextDay();

        }
        if(KeyData.FPress)
        {
            gamephase = GUI;
            gui = new CraftingGui(inventory, "");

        }
        if(KeyData.UpPress)
            current_hour++;
        if(KeyData.DownPress)
            current_hour--;
        if(KeyData.EPress)
        {
            gamephase = GUI;
            gui = new InventoryGui(inventory);
        }
        if(KeyData.CPress)
        {
            gamephase = GUI;
            gui = new CharacterGui();
        }
        if(KeyData.XPress)
        {
            gamephase = GUI;
            gui = new SellGui(inventory);
        }
        if(KeyData.Click)
        {
            HandleClick();
        }
        else if(KeyData.RightClick)
        {
            HandleRightClick();
        }

    }

    Entity getNextEntity()
    {
        selectedEntity++;
        if(selectedEntity >= object_names.size())
            selectedEntity = 0;

        return object_templates[object_names[selectedEntity]];
    }
    string getNextEntityName()
    {
        selectedEntity++;
        if(selectedEntity >= object_names.size())
            selectedEntity = 0;

        return object_names[selectedEntity];
    }
    void SwitchPhase(string ph)
    {
        cursor->text = "";
        cursor->img = NULL;

    }
    int Update()
    {
        if(busy)
        {
            cursor->Update();
            return NOTHING;
        }

        if(gamephase == FADE || (fade_progress > 0 && gamephase == CONTROL))//gamephase == FADE)
        {
            if(fade_direction == FADE_IN)
            {
                fade_progress++;
                if(fade_progress == FADE_MAX)
                {
                    fade_direction = FADE_OUT;
                    player->x = fade_x;
                    player->y = fade_y;
                    SwitchStage(fade_stage);
                    FocusCamera();
                }
            }
            else
            {
                fade_progress--;
                if(fade_progress == 0)
                {
                    gamephase = CONTROL;

                }
            }
        }
        else if(gamephase == GUI)
        {
            UpdateGUI();
        }
        else if(gamephase == CONTROL)
        {
            ControlPhase();
        }
        else if(gamephase == DIALOG)
        {
            UpdateDialog();
        }

        //player->Update();

        UpdateSystems();

        std::sort(draw_list.begin(), draw_list.end(), Sorter);

        return targetState;
    }
    void UpdateGUI()
    {
        cursor->state = Cursor::NORMAL;

        if(KeyData.wheel != 0)
            gui->Scroll(-KeyData.wheel);
        else if(KeyData.SPress)
            gui->Scroll(1);
        else if(KeyData.WPress)
            gui->Scroll(-1);
        if(KeyData.MouseMove)
        {
            cursor->text = "";
            if(gui->type == "inventory")
            {
                int id;
                Item* it = gui->GetClick(KeyData.MouseX,KeyData.MouseY,id);
                if(it)
                    cursor->text = it->formal;
            }
            if(gui->type == "shop")
            {
                if(Contains(gui,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    int id = -1;
                    Item* i = gui->GetClick(KeyData.MouseX,KeyData.MouseY,id);
                    if(i)
                    {
                        cursor->text = "click to buy";
                    }
                }

            }
            if(gui->type == "sell")
            {
                if(Contains(gui,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    //printf("szia\n");
                    int id = -1;
                    Item* i = gui->GetClick(KeyData.MouseX,KeyData.MouseY,id);
                    if(i)
                    {
                        if(i->int_attribs.count("sell"))
                            cursor->text = to_string(i->int_attribs["sell"]);

                    }
                }

            }
        }
        if(KeyData.Click)
        {
            if(gui->type == "shop")
            {
                if(Contains(gui,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    int id;
                    Item* i = gui->GetClick(KeyData.MouseX,KeyData.MouseY,id);
                    if(i)
                    if(i->int_attribs["price"] <= funds)
                    {
                        eventSystem->Event("BUY");
                        funds -= i->int_attribs["price"];
                        inventory->Add(*i);
                    }
                }

            }
            else if(gui->type == "sell")
            {
                if(Contains(gui,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    //printf("szia\n");
                    int id = -1;
                    Item* i = gui->GetClick(KeyData.MouseX,KeyData.MouseY,id);
                    if(i)
                    {
                        if(i->int_attribs.count("sell"))
                        {
                            eventSystem->Event("SELL");
                            funds += i->int_attribs["sell"];

                            inventory->Remove(id);

                        }


                    }
                }

            }
            else if(gui->type == "crafting")
            {
                if(Contains(gui,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    //printf("szia\n");
                    if(gui->HandleClick(KeyData.MouseX,KeyData.MouseY))
                        eventSystem->Event("CRAFT");


                }
            }
            else if(gui->type == "inventory")
            {
                if(Contains(gui,KeyData.MouseX,KeyData.MouseY,0,0))
                {
                    gui->HandleClick(KeyData.MouseX,KeyData.MouseY);

                }
            }
        }
        if(KeyData.EscapePress)
        {
            gui->Exit();
            gamephase = CONTROL;

        }
    }
    void UpdateDialog()
    {
        if(KeyData.EscapePress)
        {
            gamephase = CONTROL;
        }
        if(!dialogSystem->finished_scroll)
            dialogSystem->Scroll();
        else
        {
            if(KeyData.wheel && dialogSystem->finished_chain)
            {
                dialogSystem->selected_answer -= KeyData.wheel;
                if(dialogSystem->selected_answer < 0)
                    dialogSystem->selected_answer = dialogSystem->answers.size()-1;

                else if(dialogSystem->selected_answer > dialogSystem->answers.size()-1)
                    dialogSystem->selected_answer = 0;
            }
            else if(KeyData.WPress && dialogSystem->finished_chain)
            {
                dialogSystem->selected_answer -= 1;
                if(dialogSystem->selected_answer < 0)
                    dialogSystem->selected_answer = dialogSystem->answers.size()-1;

            }
            else if(KeyData.SPress && dialogSystem->finished_chain)
            {
                dialogSystem->selected_answer += 1;

                if(dialogSystem->selected_answer > dialogSystem->answers.size()-1)
                    dialogSystem->selected_answer = 0;

            }
            if(KeyData.Click || KeyData.EnterPress)
            {
                if(!dialogSystem->finished_chain)
                {
                    if(!dialogSystem->hasQuestion && dialogSystem->currentstring == dialogSystem->chain.size()-1)
                        gamephase = CONTROL;
                    else
                        dialogSystem->Next();
                }
                else
                {
                    if(!dialogSystem->gotos[dialogSystem->selected_answer])
                    {
                        gamephase = CONTROL;
                        RunScript(dialogSystem->outcomes[dialogSystem->selected_answer]);

                    }
                    else
                    {
                        RunScript(dialogSystem->outcomes[dialogSystem->selected_answer]);
                        dialogSystem->Add(dialogs[dialogSystem->gotos[dialogSystem->selected_answer]]);

                    }
                }
            }

        }
    }
    void UpdateSystems()
    {
        cursor->Update();
        timedisplay->Update();
        particlesys.Update();
        lightsys.Update();
        weathersys.Update();

    }
    void DrawSaving()
    {
        DrawImage(images[ids["fade"]],0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
        DrawString("saving game, please wait",15,15,-1,"font2");
    }
    void Draw()
    {

        ///Térkép kirajzolása
        currentstage->Draw(camera_x,camera_y);

        ///Részecskerendszer kirajzolása
        particlesys.Draw(camera_x,camera_y);

        ///Draw entities

        for(auto e : draw_list)
        {
            if(e)
                if(OnScreen(e,camera_x,camera_y))
                    e->Draw(camera_x,camera_y);

        }


        ///Draw weather effects

        weathersys.Draw();
        ///Draw lighting effects

        lightsys.Draw();

        ///Draw fading effect
        DrawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,(float)fade_progress/FADE_MAX);

        ///Draw GUI
        inventory->Draw();

        ///Draw info
        timedisplay->Draw();

        ///Draw dialog interface
        if(gamephase == DIALOG)
            dialogSystem->Draw();
        else if(gamephase == GUI)
        {
            DrawImage(images[ids["fade"]],0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
            gui->Draw();

        }
        if(busy)
            DrawSaving();

        cursor->Draw();

       ///Draw DEBUG
    }
};

class LoadGame : public Ingame
{
public:

    void Init()
    {
        Load();
    }
};
class CreateGame : public Ingame
{
public:

    void Init()
    {
        CreateNew();
    }
};
