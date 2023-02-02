
#include <iostream>
#include "controller.h"
#include "snake.h"
#include "game.h"


void Controller::launch(){
  _threads.emplace_back(&Controller::run, this);
}

void Controller::run(){
  while(control_running){
    HandleInput();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void Controller::ChangeDirection(Snake::Direction input, Snake::Direction opposite){

  if (_snake->direction != opposite || _snake->size == 1) _snake->direction = input;
  return;
}

void Controller::HandleInput(){
  
      _Key = _game->GetKeypress(_nPlayer);

  
      if(_Key == _Up)
        ChangeDirection(Snake::Direction::kUp, Snake::Direction::kDown);          

      else if(_Key == _Down)
        ChangeDirection(Snake::Direction::kDown, Snake::Direction::kUp);

      else if(_Key == _Left)
        ChangeDirection(Snake::Direction::kLeft, Snake::Direction::kRight);

      else if(_Key == _Right) 
        ChangeDirection(Snake::Direction::kRight, Snake::Direction::kLeft);
   
}

void Controller::setGameHandle(Game *game){
  _game = game;
}


Controller::~Controller(){
  std::for_each(_threads.begin(), _threads.end(), [](std::thread &thr){thr.join();});
}
        