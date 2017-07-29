#ifndef public_h
#define public_h
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>
#include <GL/GLU.h>
#include <time.h>
#include <string>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "tinyxml2.h"

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return false; }
#endif

#define fr(j,n,t) for(int i = j; i < n; i += t)
#define fr(j,n) for(int i = j; i < n; i++)
#define im(i) images[ids[i]]
#define g(i) globals[i]
#define s(i) strings[i]
#define ent(i) object_templates[i]
#define it(i) item_templates[i]
#define string std::string
#define vector std::vector
#define cout std::cout
#define endl std::endl
#define ifstream std::ifstream
#define queue std::queue
#define map std::map
#define image GLuint

template < typename T > string to_string( const T& n )
{
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}

int atoi(string s){
    return atoi(s.c_str());}

tinyxml2::XMLElement * getElementByName(tinyxml2::XMLDocument & doc, string const & elemt_value)
{
    using namespace tinyxml2;
    XMLElement * elem = doc.RootElement(); //Tree root

    while (elem)
        {
        if (!string(elem->Value()).compare(elemt_value)) return (elem);
        /*elem = elem->NextSiblingElement();*/
        if (elem->FirstChildElement())
        {
           elem = elem->FirstChildElement();
        }
        else if (elem->NextSiblingElement())
        {
           elem = elem->NextSiblingElement();
        }
        else
        {
          while (!elem->Parent()->NextSiblingElement())
          {
             if (elem->Parent()->ToElement() == doc.RootElement())
             {
                return NULL;
             }
             elem = elem->Parent()->NextSiblingElement();
          }
       }
   }
   return (NULL);
};
bool is_number(const string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

int GetInt(string name)
{
    using namespace tinyxml2;

    XMLDocument doc;
    doc.LoadFile("data/xml/globals.xml");

    int val;
    doc.FirstChildElement()->FirstChildElement(name.c_str())->QueryIntText(&val);
    //getElementByName(doc,name)->QueryIntText(&val);

    return val;
}

int GetIntElement(string node, string name)
{
    using namespace tinyxml2;

    XMLDocument doc;
    doc.LoadFile("data/xml/globals.xml");

    int val;
    doc.FirstChildElement()->FirstChildElement(node.c_str())->FirstChildElement(name.c_str())->QueryIntText(&val);
    //getElementByName(doc,name)->QueryIntText(&val);

    return val;
}

SDL_Renderer* gRenderer = NULL;

SDL_GLContext gContext;

const int SCREEN_WIDTH = GetIntElement("screen","width");//800;//1366;
const int SCREEN_HEIGHT = GetIntElement("screen","height");//600;//768;
const int SCREEN_FPS = GetIntElement("screen","fps");//100;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const int TILESIZE = GetInt("tilesize");//32;
const int LIGHTSIZE = GetInt("lightsize");//8;

const int FONTSIZE = GetInt("fontsize");
const int FONTCLIP = 21;

const int DIGITWIDTH = 14;

int audioFiles = 0;
queue<string> audioCommands;

enum
{
    DOWN,
    LEFT,
    RIGHT,
    UP,
};
int characterWidth(char c)
{
    if(c == 'W')
        return 25;
    else if(c == 'M')
        return 22;
    else if(c == 'C')
        return 22;
    else if(c == 'I')
        return 12;
    else if((int)c >= 65 && (int)c <= 90)
        return 20;


    else if((int)c >= 97 && (int)c <= 122)
    {
        if(c == 'm' || c == 'w')
            return 18;
        else if(c == 'l' || c == 'i')
            return 7;
        else
            return 13;
    }

    else if((int)c >= 48 && (int)c <= 57)
    {
        return DIGITWIDTH;//DIGIT
    }

    else if(c == '.' || c == '!' || c == ',' )
        return 5;
    else if(c == ':')
        return 8;
    else if(c == ' ')
        return 10;

    return 18;
}

int lineWidth(string s, int ptSize = -1)
{
    if(ptSize == -1)
        ptSize = FONTSIZE;
    int sum = 0;
    for(int i = 0; i < s.size(); i++)
        sum += characterWidth(s[i])*ptSize/FONTSIZE;

    return sum;
}
int linesCount(string c, int linelen)
{
    std::stringstream ss;
    vector<string> words;
    ss << c;
    string newword;
    while(ss >> newword){words.push_back(newword);};
    int i = 0;
    int current_length = 0;
    int lines = 1;
    while(i < words.size())
    {
        if(current_length + lineWidth(words[i] + " ") + 1 > linelen)
        {
            lines++;
            current_length = 0;
        }
        else
        {
            current_length += lineWidth(words[i] + " ");
            i++;
        }
    }
    return lines;
}
const int MAXIDS = 60000;

const int MAP_MAX_SIZE = 100;

const int INVENTORY_TOOLBAR_SIZE = 10;
const int INVENTORY_SIZE = 10*4;

string seasons[4] = {"Spring","Summer","Fall","Winter"};

string particle = "rain"; ///snow cat

int current_day = 1;
int current_season = 1;
int current_year = 1;

int current_hour = 9;
int current_minute = 0;
int current_seconds = 0;

const int PLAYER_STARTING_FUNDS = 30000;

const int CRAFTING_XP = 10;

int funds = PLAYER_STARTING_FUNDS;

string CURRENCY = " plotyi";

int hunger = 0;

const int HUNGER_SCALE = 15;
const int MAX_HUNGER_LEVEL = 4;

string hunger_levels[MAX_HUNGER_LEVEL] = {"Well sated","Slightly hungry","Hungry","Starving"};

string task = "Cut down a tree";

int main_progress = 0;

vector<string> tasks = {"Eat a fruit","Talk to a person", "Buy something", "Cook something"};
vector<string> events = {"EAT","TALK","BUY","CRAFT"};
vector<vector<string> > event_string_parameters = {{"fruit"},{},{},{"cooking"}};

queue<string> alerts;

int selectedEntity = 0;
int randomer()
{
    srand(time(0));

    return 0;
}
string daily = tasks[randomer()+rand()%tasks.size()];

map<string, int> skills = {{"crafting",1},{"cooking",1},{"woodcutting",1},{"fishing",1},{"charisma",1}};

map<string, int> xps = {{"crafting",0},{"cooking",0},{"woodcutting",0},{"fishing",0},{"charisma",0}};

map<string, vector<string> > unlocks = {{"crafting",{"You can now craft more stuff"}}};

map<string, int> globals;
map<string, string> strings;

int levelcap(int lvl)
{
    return lvl*globals["xp-scale"];
}

float camera_x = 0, camera_y = 0;
float camera_xvel = 0, camera_yvel = 0;
bool GUI_ENABLED = true;

enum {
    FADE_IN,
    FADE_OUT,
};

int fade_x = 0, fade_y = 0;
int fade_direction = FADE_IN;
int fade_progress = 0;
const int FADE_MAX = 30;


/*struct alpha_map
{
    bool alpha[32][32];
};
vector<alpha_map> alpha_maps;*/

string Decapitalize(string s)
{
    if(s.size()>0)
    {
        if(isalpha(s[0])&&s[0]>='A' && s[0]<='Z')
        {
            s[0] += 'a'-'A';
            return s;
        }
    }
    else
        return s;
}
string Capitalize(string s)
{
    if(s.size()>0)
    {
        if(isalpha(s[0])&&s[0]>='a' && s[0]<='z')
        {
            s[0] -= 'a'-'A';
            return s;
        }
    }
    else
        return s;
}
string FirstWord(string s)
{
    std::stringstream ss(s);

    string w;

    ss >> w;

    return w;
}
string SecondWord(string s)
{
    std::stringstream ss(s);

    string w;

    ss >> w;
    ss >> w;

    return w;
}
string nWord(int n,string s)
{
    std::stringstream ss(s);

    string w;

    for(int i = 0; i < n; i++)
        ss >> w;

    return w;
}

vector<string> allWord(string s)
{

    std::stringstream ss(s);

    vector<string> w;

    while(!ss.eof())
    {
        string a;
        ss >> a;
        w.push_back(a);
    }

    return w;
}
vector<string> split(string s, string delimiter)
{
    vector<string> w;

    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        w.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    if(s!="")
        w.push_back(s);

    return w;
}
float GetDistance(float x1, float y1, float x2, float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt(dx*dx+dy*dy);
}
enum
{
    EXIT,
    START,
    MENU,
    GAME,
    LOAD,
    CREATE,
    NOTHING,
};
bool LoadGlobals()
{
    using namespace tinyxml2;
    XMLDocument xmlDoc;
    XMLError result;

    result = xmlDoc.LoadFile("data/xml/globals.xml");

    XMLCheckResult(result);
    printf("XML file loaded..\n");

    XMLElement* pRoot = xmlDoc.FirstChildElement();
    if(pRoot == NULL)
    {
        printf("XML read error (root)\n");
        return 0;
    }
    XMLElement * pElement = pRoot->FirstChildElement("globals");
    if(pElement == NULL)
    {
        printf("XML read error!\n");
        return 0;
    }
    XMLElement * globalElement = pElement->FirstChildElement();

    while(globalElement)
    {
        globalElement->QueryIntAttribute("value",&globals[globalElement->Name()]);

        globalElement = globalElement->NextSiblingElement();
    }
    globalElement = pRoot->FirstChildElement("strings")->FirstChildElement();

    while(globalElement)
    {

        strings[globalElement->Name()] = globalElement->Attribute("value");

        globalElement = globalElement->NextSiblingElement();
    }
    return 1;
}
bool SaveFileExists()
{
    using namespace tinyxml2;
    XMLDocument doc;
    XMLError err;

    err = doc.LoadFile("data/save/save.xml");

    if(err != XML_SUCCESS){
        printf("No save file found\n");
        return false;
    }

    return true;
}

#endif
