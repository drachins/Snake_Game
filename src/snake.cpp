#include "snake.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <future>
#include <functional>




void Snake::launch(){
  threads.emplace_back(&Snake::run, this);
}

void Snake::run(){
  while(alive){
    Update();    
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
  }
}

void Snake::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // We first capture the head's cell before updating.
  
  Controller controller;
  
  if(playerN == 0){
    std::future<void> dir = std::async(std::launch::async, &Controller::RightController, std::ref(controller), this);
    dir.wait();
 
  }
  else{
    std::future<void> dir = std::async(std::launch::async, &Controller::LeftController, std::ref(controller), this);
    dir.wait();
  }

  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Check if snake is still alive
  CheckObstacle(current_cell);

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= speed;
      break;

    case Direction::kDown:
      head_y += speed;
      break;

    case Direction::kLeft:
      head_x -= speed;
      break;

    case Direction::kRight:
      head_x += speed;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);

}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }
 

  // Check if the snake has died.
  for (auto const &item : body) {
    if ((current_head_cell.x == item.x && current_head_cell.y == item.y)) {
      alive = false;
    }
  }
}

void Snake::GetObstacles(std::vector<std::shared_ptr<Obstacle>> obstacles){
  _obstacles = obstacles; 
}

void Snake::GrowBody() { growing = true; }

void Snake::CheckObstacle(SDL_Point curr_cell){
  int x = curr_cell.x;
  int y = curr_cell.y;
  if(std::any_of(_obstacles.begin(), _obstacles.end(), [x, y](std::shared_ptr<Obstacle> &itr){ return (itr->GetObstacleXCoord() == x && itr->GetObstacleYCoord() == y);})){
    alive = false;
  }
}

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}

Snake::~Snake(){
  std::for_each(threads.begin(), threads.end(), [](std::thread &t){t.join();});
}

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,Snake::Direction opposite) const {

  if (snake.direction != opposite || snake.size == 1) snake.direction = input;


}


void Controller::RightController(Snake &snake) const {

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
        ChangeDirection(snake, Snake::Direction::kUp, Snake::Direction::kDown);
        break;

        case SDLK_DOWN:
        ChangeDirection(snake, Snake::Direction::kDown, Snake::Direction::kUp);
        break;

        case SDLK_LEFT:
        ChangeDirection(snake, Snake::Direction::kLeft, Snake::Direction::kRight);
        break;

        case SDLK_RIGHT:
        ChangeDirection(snake, Snake::Direction::kRight,Snake::Direction::kLeft);
        break;
      }
    }   
  }
}


void Controller::LeftController(Snake &snake) const {

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_KEYDOWN){
      switch(e.key.keysym.sym){
        case SDLK_w:
        ChangeDirection(snake, Snake::Direction::kUp, Snake::Direction::kDown);
        break;

        case SDLK_s:
        ChangeDirection(snake, Snake::Direction::kDown, Snake::Direction::kUp);
        break;

        case SDLK_d:
        ChangeDirection(snake, Snake::Direction::kLeft, Snake::Direction::kRight);
        break;

        case SDLK_a:
        ChangeDirection(snake, Snake::Direction::kRight, Snake::Direction::kLeft);
        break;
      }
    }           
  }
} 