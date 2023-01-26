#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"



Snake::Direction Controller::ChangeDirection(Snake &snake, Snake::Direction input,Snake::Direction opposite) const {

  if (snake.direction != opposite || snake.size == 1){
    return input;
  }
  else{ return opposite;}

}


Snake::Direction Controller::RightController(Snake &snake) const {
  Snake::Direction dir;
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
        dir = ChangeDirection(snake, Snake::Direction::kUp,
                              Snake::Direction::kDown);
        break;

        case SDLK_DOWN:
        dir = ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp);
        break;

        case SDLK_LEFT:
        dir = ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight);
        break;

        case SDLK_RIGHT:
        dir = ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft);
        break;
      }

    return dir;
    }   
  }
  return snake.direction;
}


Snake::Direction Controller::LeftController(Snake &snake) const {
  Snake::Direction dir;
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN){
      switch(e.key.keysym.sym){
        case SDLK_w:
        dir = ChangeDirection(snake, Snake::Direction::kUp,
                              Snake::Direction::kDown);
        break;

        case SDLK_s:
        dir = ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp);
        break;

        case SDLK_d:
        dir = ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight);
        break;

        case SDLK_a:
        dir = ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft);
        break;
      }
    return dir;
    }           
  }
  return snake.direction;
} 



  
        

  


