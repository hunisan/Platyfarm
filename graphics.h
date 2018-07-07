#pragma once
#include "images.h"

class Graphics
{
public:
    SDL_Window* window;

    Graphics()
    {

    }

    ///this will never work
    /*bool GetBoundingBox()
    {
        //while(1)
        alpha_maps.push_back(alpha_map());
        for(int _i = 1; _i < images.size(); _i++)
        {
            unsigned char pixel[4];

            DrawImage(images[ids["fuchsia"]],0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
            if(_i == ids["human"])
            {
                //printf("ezigen\n");
                DrawClip(images[_i],0,0,32,32,0,0,32,32,96,128);

            }
            else
                DrawImage(images[_i],0,0,32,32);//,0,0,32,32,96,128);

            //Finalize();

            alpha_map bbox;
            for(int i = 0; i < 32; i++)
                for(int j = 0; j < 32; j++)
                {

                    glReadPixels(i, SCREEN_HEIGHT - j, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
                `std::cout << "R: " << (int)pixel[0] << std::endl;
                    std::cout << "G: " << (int)pixel[1] << std::endl;
                    std::cout << "B: " << (int)pixel[2] << std::endl;
                    std::cout << "A: " << (int)pixel[3] << std::endl;
                    std::cout << std::endl;

                    bbox.alpha[i][j]= ((int)pixel[0]==255 && (int)pixel[1]==0 && (int)pixel[2] == 255) ? 0 : 1 ;


                }

            alpha_maps.push_back(bbox);

            if(_i == ids["bush"])
                for(int i = 0; i < 32; i++)
                {
                    for(int j = 0; j < 32; j++)
                    {
                        std::cout << bbox.alpha[i][j] << " ";
                    }
                    std::cout << std::endl;
                }
            //if(_i == ids["human"])
            //while(1){Finalize();};
            //Finalize();
            //std::cout << std::endl;
        }

    }*/
    bool InitGL()
    {
        bool success = true;
        GLenum error = GL_NO_ERROR;

        //Initialize Projection Matrix
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        //Check for error
        error = glGetError();
        if( error != GL_NO_ERROR )
        {
            printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
            success = false;
        }

        //Initialize Modelview Matrix
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        //Check for error
        error = glGetError();
        if( error != GL_NO_ERROR )
        {
            printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
            success = false;
        }

        glClearColor( 0.f, 0.f, 0.f, 1.f );

        //Check for error
        error = glGetError();
        if( error != GL_NO_ERROR )
        {
            printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
            success = false;
        }
        glEnable(GL_TEXTURE_2D);

        glEnable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glMatrixMode( GL_PROJECTION );
        glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 );
        glLoadIdentity();

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        return success;
    }
    bool Init()
    {
        printf( "Initializing graphics engine... ");

        if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
        {
            printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );

            return false;
        }
        else
        {
                //Use OpenGL 2.1
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

            window = SDL_CreateWindow( s("title").c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
            if( window == NULL )
            {
                printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
                return false;
            }
            else
            {
                gContext = SDL_GL_CreateContext( window );
                if( gContext == NULL )
                {
                    printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
                    return false;
                }
                else
                {
                    //Initialize renderer color
                    //SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    if( SDL_GL_SetSwapInterval( 1 ) < 0 )
                    {
                        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
                    }

                    //Initialize PNG loading
                    int imgFlags = IMG_INIT_PNG;
                    if( !( IMG_Init( imgFlags ) & imgFlags ) )
                    {
                        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                        return false;
                    }

                    if( TTF_Init() == -1 )
                    {
                        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                        return false;
                    }

                    if( !InitGL() )
                    {
                        printf( "Unable to initialize OpenGL!\n" );
                        return false;
                    }
                }

            }
        }

        SDL_ShowCursor(SDL_DISABLE);

        SDL_StartTextInput();
        
        printf("Success!\n");
        return true;
    }

    bool LoadImages()
    {
        printf("Loading images from XML list...\n");
        using namespace tinyxml2;

        XMLDocument xmlDoc;
        XMLError result;

        result = xmlDoc.LoadFile("data/xml/img.xml");

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
        String name,path;

        images.push_back(NULL);

        int i = 1;
        while (pElement != NULL)
        {
            printf("Loading file..\n");

            name = pElement->FirstChildElement("name")->GetText();
            path = pElement->FirstChildElement("path")->GetText();

            images.push_back(loadImage(path));

            ids[name]=i;

            pElement = pElement->NextSiblingElement("img");
            i++;
        };


        return 1;
    }



    /*void LoadContent()
    {
        content::platypus = loadImage("platypus.png");
        content::grass = loadImage("grass.png");
        content::grass2 = loadImage("grass2.png");
        content::cursor = loadImage("cursor.png");
        content::dark = loadImage("dark.png");
        content::bush = loadImage("Bokor_1.png");
        content::platy32 = loadImage("Platti32.png");
        content::itembar = loadImage("itembar.png");
        content::frame = loadImage("frame.png");
        content::axe = loadImage("axe.png");
        content::apple = loadImage("apple.png");
        content::trunk = loadImage("Tonk_1.png");
    }*/
    void Clear()
    {
        glClear( GL_COLOR_BUFFER_BIT );
    };

    void Finalize()
    {
        SDL_GL_SwapWindow( window );
    };
};
