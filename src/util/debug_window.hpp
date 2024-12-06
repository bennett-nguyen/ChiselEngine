#ifndef DEBUG_WINDOW_HPP
#define DEBUG_WINDOW_HPP

#include "imgui.h"
#include "player.hpp"
#include <SDL2/SDL.h>

class DebugWindow {
public:
    DebugWindow(Player *p_player);
    void show();

private:
    Player *mp_player;
};

#endif