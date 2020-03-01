#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// image IDs for the game objects

const int IID_PLAYER                =  0;
const int IID_SALMONELLA            =  1;
const int IID_FOOD                  =  2;
const int IID_ECOLI                 =  3;
const int IID_SPRAY                 =  4;
const int IID_FLAME                 =  5;
const int IID_PIT                   =  6;
const int IID_DIRT                  =  7;
const int IID_FLAME_THROWER_GOODIE  =  8;
const int IID_RESTORE_HEALTH_GOODIE =  9;
const int IID_EXTRA_LIFE_GOODIE     = 10;
const int IID_FUNGUS                = 11;

// sounds

const int SOUND_PLAYER_DIE      =  0;
const int SOUND_SALMONELLA_DIE  =  1;
const int SOUND_SALMONELLA_HURT =  2;
const int SOUND_PLAYER_FIRE     =  3;
const int SOUND_PLAYER_SPRAY    =  4;
const int SOUND_GOT_GOODIE      =  5;
const int SOUND_FINISHED_LEVEL  =  6;
const int SOUND_ECOLI_DIE       =  7;
const int SOUND_ECOLI_HURT      =  8;
const int SOUND_PLAYER_HURT     =  9;
const int SOUND_THEME           = 10;
const int SOUND_BACTERIUM_BORN  = 11;

const int SOUND_NONE            = -1;


// keys the user can hit

const int KEY_PRESS_LEFT   = 1000;
const int KEY_PRESS_RIGHT  = 1001;
const int KEY_PRESS_UP     = 1002;
const int KEY_PRESS_DOWN   = 1003;
const int KEY_PRESS_SPACE  = ' ';
const int KEY_PRESS_TAB    = '\t';
const int KEY_PRESS_ENTER  = '\r';
const int KEY_PRESS_ESCAPE = '\x1b';

// view and sprite dimensions

const int VIEW_WIDTH = 256;
const int VIEW_HEIGHT = 256;
const int VIEW_RADIUS = 128;

const int SPRITE_WIDTH = 8;
const int SPRITE_HEIGHT = 8;
const int SPRITE_RADIUS = 4;

const double SPRITE_WIDTH_GL = .3; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .25; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming


// status of each tick (did the player die?)

const int GWSTATUS_PLAYER_DIED    = 0;
const int GWSTATUS_CONTINUE_GAME  = 1;
const int GWSTATUS_PLAYER_WON     = 2;
const int GWSTATUS_FINISHED_LEVEL = 3;
const int GWSTATUS_LEVEL_ERROR    = 4;


  // Return a uniformly distributed random int from min to max, inclusive

inline
int randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

#endif // GAMECONSTANTS_H_
