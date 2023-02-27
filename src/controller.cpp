
#include <iostream>
#include "controller.h"
#include "game.h"

// Function that launches controller thread and cal run() method. 
void Controller::launch(){
  _threads.emplace_back(&Controller::run, this);
}

// Function that calls HandleInput() in a while loop that runs until game is exited. 
void Controller::run(){
  while(control_running){
    HandleInput();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

// Helper function that sets direction of Snake class instance according to keyboard input. 
void Controller::ChangeDirection(Snake::Direction input, Snake::Direction opposite){

  if (_snake->direction != opposite || _snake->size == 1) _snake->direction = input;
  return;
}

// Function that handles input from keyboard. 
void Controller::HandleInput(){

      // Function call to Game class method GetKeyPress witch returns value of key press event. 
      _Key = _game->GetKeypress(_nPlayer);

  
      // if and else if black that calls ChangeDirection() depending on value of key press. 
      if(_Key == _Up)
        ChangeDirection(Snake::Direction::kUp, Snake::Direction::kDown);          

      else if(_Key == _Down)
        ChangeDirection(Snake::Direction::kDown, Snake::Direction::kUp);

      else if(_Key == _Left)
        ChangeDirection(Snake::Direction::kLeft, Snake::Direction::kRight);

      else if(_Key == _Right) 
        ChangeDirection(Snake::Direction::kRight, Snake::Direction::kLeft);
   
}

// Function that sets handle of _game member variable to instance of Game class that is created when game is started. 
void Controller::setGameHandle(Game *game){
  _game = game;
}

// Destructor that joins controller threads, 
Controller::~Controller(){
  std::for_each(_threads.begin(), _threads.end(), [](std::thread &thr){thr.join();});
}
        