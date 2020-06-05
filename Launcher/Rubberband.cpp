#include "Rubberband.h"

#include <cmath>

void Rubberband::mouseDown(int x, int y)
{
	rect.x = x;
	rect.y = y;
  rect.w = 0;
  rect.h = 0;
	isMouseDown = true;
}

bool Rubberband::mouseUp(SDL_Rect& in_rect)
{
  bool retval = isSelecting;
  isSelecting = false;
	isMouseDown = false;
  if (retval) in_rect = rect;
  return retval;
}

void Rubberband::mouseMove(int x, int y)
{
	if (isMouseDown)
	{
		rect.w = x - rect.x;
		rect.h = y - rect.y;

    if (!isSelecting)
    {
      if (std::abs(rect.w) >= 4 || std::abs(rect.h) >= 4)
      {
        isSelecting = true;
      }
    }
	}
}

int Rubberband::draw(SDL_Surface* surface)
{
  if (!isSelecting) return 0;

  Uint32 white = 0xFFFFFFFF;

  int retval = 0;

  SDL_Rect top;
  top.x = rect.w >= 0 ? rect.x : rect.x + rect.w;
  top.y = rect.y;
  top.w = std::abs(rect.w) + 1;
  top.h = 1;
  retval |= SDL_FillRect(surface, &top, white);

  SDL_Rect right;
  right.x = rect.x + rect.w;
  right.y = rect.h >= 0 ? rect.y : rect.y + rect.h;
  right.w = 1;
  right.h = std::abs(rect.h) + 1;
  retval |= SDL_FillRect(surface, &right, white);

  SDL_Rect bottom;
  bottom.x = rect.w >= 0 ? rect.x : rect.x + rect.w;
  bottom.y = rect.y + rect.h;
  bottom.w = std::abs(rect.w) + 1;
  bottom.h = 1;
  retval |= SDL_FillRect(surface, &bottom, white);

  SDL_Rect left;
  left.x = rect.x;
  left.y = rect.h >= 0 ? rect.y : rect.y + rect.h;
  left.w = 1;
  left.h = std::abs(rect.h) + 1;
  retval |= SDL_FillRect(surface, &left, white);

  return retval;
}
