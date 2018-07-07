#ifndef PUBLIC_H
#define PUBLIC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>
#include <GL/GLU.h>
#include <time.h>
#include <String>
#include <cmath>
#include <String>
#include <vector>
#include <queue>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <String>
#include <sstream>
#include <algorithm>
#include "tinyxml2.h"

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return false; }
#endif

#define frt(j,n,t) for(int i = j; i < n; i += t)
#define fr(j,n) for(int i = j; i < n; i++)
#define im(i) images[ids[i]]
#define g(i) globals[i]
#define s(i) strings[i]
#define ent(i) object_templates[i]
#define it(i) item_templates[i]
#define String std::string
#define vector std::vector
#define cout std::cout
#define endl std::endl
#define ifstream std::ifstream
#define queue std::queue
#define map std::map
#define image GLuint

template < typename T > String to_string( const T& n )
{
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}

int atoi(String s);

tinyxml2::XMLElement * getElementByName(tinyxml2::XMLDocument & doc, String const & elemt_value);

bool is_number(const String& s);

int GetInt(String name);

int GetIntElement(String node, String name);

int randomer();

int levelcap(int lvl);

int characterWidth(char c);
int lineWidth(String s, int ptSize = -1);
int linesCount(String c, int linelen);

String Decapitalize(String s);
String Capitalize(String s);
String FirstWord(String s);
String SecondWord(String s);
String nWord(int n,String s);

vector<String> allWord(String s);
vector<String> split(String s, String delimiter);
float GetDistance(float x1, float y1, float x2, float y2);

bool LoadGlobals();
bool SaveFileExists();
///functions end here/////////////////////////////////////////////
///constants begin here///////////////////////////////////////////

const int SCREEN_WIDTH = GetIntElement("screen","width");//800;//1366;
const int SCREEN_HEIGHT = GetIntElement("screen","height");//600;//768;
const int SCREEN_FPS = GetIntElement("screen","fps");//100;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

const int TILESIZE = GetInt("tilesize");//32;
const int LIGHTSIZE = GetInt("lightsize");//8;

const int FONTSIZE = GetInt("fontsize");
const int CHARACTER_SCALE = GetInt("character-scale");

const int DIGITWIDTH = 14;


const int MAXIDS = 60000;

const int MAP_MAX_SIZE = 100;

const int INVENTORY_TOOLBAR_SIZE = 10;
const int INVENTORY_SIZE = 10*4;

const int PLAYER_STARTING_FUNDS = 200;

const int CRAFTING_XP = 10;

const int HUNGER_SCALE = 15;
const int MAX_HUNGER_LEVEL = 4;


enum
{
    DOWN,
    LEFT,
    RIGHT,
    UP,
};


enum {
    FADE_IN,
    FADE_OUT,
};

enum
{
    EXIT,
    START,
    MENU,
    GAME,
    LOAD,
    CREATE,
    EDITOR,
    NOTHING,
};
///constants end here///////////////////////////////////////////
///variable declarations begin here//////


extern SDL_Renderer* gRenderer;// = NULL;

extern SDL_GLContext gContext;


extern int audioFiles;// = 0;
extern queue<String> audioCommands;


extern String seasons[4];// = {"Spring","Summer","Fall","Winter"};

extern String particle;// = "rain"; ///snow cat

extern int current_day;// = 1;
extern int current_season;// = 1;
extern int current_year;// = 1;

extern int current_hour;// = 9;
extern int current_minute;// = 0;
extern int current_seconds;// = 0;

static bool dead = false;

extern int funds; //= PLAYER_STARTING_FUNDS;

extern String CURRENCY; //= "G";

extern int hunger; //= 0;

extern String hunger_levels[MAX_HUNGER_LEVEL]; //= {"Well sated","Slightly hungry","Hungry","Starving"};

extern String task; //= "Cut down a tree";

extern int main_progress; //= 0;

extern vector<String> tasks;// = {"Eat a fruit","Talk to a person", "Buy something", "Cook something"};
extern vector<String> events;//= {"EAT","TALK","BUY","CRAFT"};
extern vector<vector<String> > event_string_parameters; //= {{"fruit"},{},{},{"cooking"}};

extern queue<String> alerts;

extern int selectedEntity; //= 0;


extern String daily; //= tasks[randomer()+rand()%tasks.size()];

extern map<String, int> skills; //= {{"crafting",1},{"cooking",1},{"woodcutting",1},{"fishing",1},{"charisma",1}};

extern map<String, int> xps; //= {{"crafting",0},{"cooking",0},{"woodcutting",0},{"fishing",0},{"charisma",0}};

extern map<String, vector<String> > unlocks; //= {{"crafting",{"You can now craft more stuff"}}};

extern map<String, int> globals;
extern map<String, String> strings;


extern float camera_x; //= 0,
extern float camera_y; //= 0;
extern float camera_xvel; //= 0,
extern float camera_yvel; // = 0;
extern bool GUI_ENABLED; // = true;

extern int fade_x; // = 0; //
extern int fade_y; // = 0;
extern int fade_direction; // = FADE_IN;
extern int fade_progress; // = 0;


#endif
