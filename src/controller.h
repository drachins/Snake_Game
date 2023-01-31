#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Game;

class Controller {
 public:
  Controller(SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right, std::shared_ptr<Snake> snake)
    :_Up(up),
     _Down(down),
     _Left(left),
     _Right(right),
     _snake(snake){};
  
  ~Controller();

  void launch();
  void run();
  void HandleInput();
  void setGameHandle(Game *game);

  bool control_running;


 private:
  void ChangeDirection(Snake::Direction input, Snake::Direction opposite);
    
  SDL_Keycode _Up;
  SDL_Keycode _Down;
  SDL_Keycode _Left;
  SDL_Keycode _Right;
  SDL_Keycode _Key;

  Game *_game;
  std::vector<std::thread> _threads;
  std::shared_ptr<Snake> _snake;
   
};

#endif