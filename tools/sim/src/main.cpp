//standard libraries:
#include <stdlib.h>

//SDL libraries:
#include <SDL.h>
#include "SDL_gfxPrimitives.h"

#include "Map.hpp"
#include "Robot.hpp"
#include "GoState.hpp"


//global constants:
const int SCREEN_W = 915;
const int SCREEN_H = 715;
const char* WINDOW_TITLE = "Robot Simulator";

int main(int argc, char **argv) {
    //set up our main SDL objects:
    SDL_Init( SDL_INIT_VIDEO );
    SDL_Surface* screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF );
    SDL_WM_SetCaption( WINDOW_TITLE, 0 );
    SDL_Event event;
    bool gameRunning = true; //lets us exit the game loop
    GoState* goState = new GoState( 1.4, 300, 300 );
    Robot* robot = new Robot( goState, 100, 100 );
    MapDrawer* drawer = new MapDrawer( screen );


    while (gameRunning) { //the game loop
        //event checker:
        if (SDL_PollEvent(&event)) { //SDL_PollEvent will return 1 if the targeted event has occured
            //quit if we made event an SDL_QUIT event.
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            }
        }

        //clear surfaces background:
        SDL_FillRect(screen, NULL, 0x0);
        robot->draw( drawer );
        robot->update();

        SDL_Flip(screen);
    }

    SDL_Quit();
    return 0;
}
