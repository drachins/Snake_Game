#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

// Forwards declaration of Game class. 
class Game;

 // Controller class declaration. 
class Controller {
 public:

  // Controller class constructor and destructor declarations. 
  Controller(SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right, std::shared_ptr<Snake> snake, int nPlayer)
    :_Up(up),
     _Down(down),
     _Left(left),
     _Right(right),
     _snake(snake),
     _nPlayer(nPlayer){};
  
  ~Controller();

  // Public function declarations 
  void launch();
  void run();
  void HandleInput();

  // Setter function declaration. 
  void setGameHandle(Game *game);

  bool control_running{true};
  int _nPlayer;

 private:

  // Private function declarations 
  void ChangeDirection(Snake::Direction input, Snake::Direction opposite);
  
  // Private member variable declarations. 
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