#ifndef DEBUG_WINDOW_HPP
#define DEBUG_WINDOW_HPP

#include <SDL2/SDL.h>
#include "imgui.h"
#include "player.hpp"

class DebugWindow {
public:
    DebugWindow(Player *p_player);
    void show();

private:
    Player *mp_player;
};

#endif