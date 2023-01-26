
#include <iostream>
#include <iterator>
#include <algorithm>
#include <functional>
#include "SDL.h"
#include "game.h"

Game::Game(std::size_t grid_width, std::size_t grid_height, int _nPlayers)
    : engine(dev()),
      nPlayers(_nPlayers),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  
  for(int i = 0; i < nPlayers; i++){
    _snakes.push_back(std::make_shared<Snake>(grid_width, grid_height, i));
  }
  PlaceFood();
  PlaceObstacle();
}


void Game::Run(Renderer &renderer, std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  std::for_each(_snakes.begin(), _snakes.end(), [](std::shared_ptr<Snake> &itr){itr->launch();});
  
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    Update();
    renderer.Render(_snakes, _food, _obstacles);

    CheckForQuit(running);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;
    std::cout << "echo4" << std::endl;
    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
    

  }
  std::for_each(_snakes.begin(), _snakes.end(), [](std::shared_ptr<Snake> &itr){itr->alive = false;});
}

void Game::PlaceFood() {
  if(_food.empty()){
    for(int i = 0; i < nFood; i++){
      int x, y;
      SDL_Point fd;
      while(true){
        x = random_w(engine);
        y = random_h(engine);
        if(std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr){return !itr->SnakeCell(x,y);})){
          fd.x = x;
          fd.y = y;
          _food.push_back(fd);
          break;
        }
      }
    }
  }
  else{
    int x, y;
    SDL_Point fd;
    while(true){
      x = random_w(engine);
      y = random_h(engine);
      if(std::any_of(_obstacles.begin(), _obstacles.end(), [x, y](std::shared_ptr<Obstacle> &itr){return !(itr->GetObstacleXCoord() == x && itr->GetObstacleYCoord() == y);}) && std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr) {return !itr->SnakeCell(x,y);})){
        fd.x = x;
        fd.y = y;
        _food.push_back(fd);
        break;
      }
    }
  }
}



void Game::PlaceObstacle() {
  for(int i = 0; i < nObstacles; i++){
    Obstacle obstacle;
    int x, y;
    while(true){
      x = random_w(engine);
      y = random_h(engine);
      if(std::any_of(_food.cbegin(), _food.cend(), [x,y](SDL_Point itr){return !(itr.x == x && itr.y == y);}) && std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr){return !itr->SnakeCell(x,y);})){
        obstacle.SetObstacleCoords(x,y);
        _obstacles.push_back(std::make_shared<Obstacle>(obstacle));
        break;
      }
    }
  }
  for(auto itr : _snakes){
    itr->GetObstacles(_obstacles);
    }  
}


void Game::Update() {

  
  for(int i = 0; i < nPlayers; i++){
    int x_head = static_cast<int>(_snakes[i]->head_x);
    int y_head = static_cast<int>(_snakes[i]->head_y);
    if(std::any_of(_food.begin(), _food.end(), [x_head, y_head](SDL_Point itr){return (itr.x == x_head && itr.y == y_head);})){
      score[i]++;
      _food.erase(_food.begin() + i);
      _snakes[i]->GrowBody();
      _snakes[i]->speed += 0.02;
      PlaceFood();
    }
  }
  

}

void Game::CheckForQuit(bool &running){
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
    running = false;}
  }
}

std::vector<int> Game::GetScore() const { return score; }

std::vector<int> Game::GetSize() { 
  std::vector<int> sizes;
  std::for_each(_snakes.begin(), _snakes.end(), [sizes](std::shared_ptr<Snake> &itr) mutable {sizes.push_back(itr->size);});
  return sizes;
 }
