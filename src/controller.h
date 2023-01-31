#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Controller {
 public:
  Controller(SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right)
    :_Up(up),
     _Down(down),
     _Left(left),
     _Right(right){};

  void HandleInput(bool &running, std::shared_ptr<Snake> snake) const;
  SDL_Keycode Getcode(){return _Up;}


 private:
  void ChangeDirection(Snake &snake, Snake::Direction input, Snake::Direction opposite) const;
    
  SDL_Keycode _Up;
  SDL_Keycode _Down;
  SDL_Keycode _Left;
  SDL_Keycode _Right;
};

#endif