#pragma once
#include <SDL.h>

class Rubberband
{
    SDL_Rect rect;
    bool isSelecting = false;
    bool isMouseDown = false;
public:
    void mouseDown(int x, int y);
    bool mouseUp(SDL_Rect& in_rect);
    void mouseMove(int x, int y);
    int draw(SDL_Surface* surface);
};
