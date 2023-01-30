#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}


void Controller::HandleInput(bool &running, std::vector<std::shared_ptr<Snake>> snakes) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {

    if(e.type == SDL_QUIT){
      running = false;
    }

    else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {

        case SDLK_UP:
          ChangeDirection(*snakes.at(0), Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(*snakes.at(0), Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(*snakes.at(0), Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(*snakes.at(0), Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;

        case SDLK_w:
          ChangeDirection(*snakes.at(1), Snake::Direction::kUp,
                            Snake::Direction::kDown);
          break;
          
        case SDLK_s:
          ChangeDirection(*snakes.at(1), Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_a:
          ChangeDirection(*snakes.at(1), Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_d:
          ChangeDirection(*snakes.at(1), Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
      }
    }
  }
}

