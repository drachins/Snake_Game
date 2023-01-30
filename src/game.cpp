
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include "SDL.h"
#include "game.h"

Game::Game(std::size_t grid_width, std::size_t grid_height, int _nPlayers)
    : engine(dev()),
      nPlayers(_nPlayers),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  
  for(int i = 0; i < nPlayers; i++){
    _snakes.push_back(std::make_shared<Snake>(grid_width, grid_height, i));
    score.push_back(0);
    snake_sizes.push_back(0);
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


  Controller controller;

  while (running) {

    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    if(nPlayers < 2){
      controller.HandleInput1Player(running, _snakes);
    }
    else{
      controller.HandleInput2Players(running, _snakes);
    }
    Update();
    renderer.Render(_snakes, _food, _obstacles);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

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
    int x = static_cast<int>(_snakes.at(i)->head_x);
    int y = static_cast<int>(_snakes.at(i)->head_y);
    if(nPlayers > 1){
      if(i < 1){
        if(std::any_of(_snakes.at(i+1)->body.begin(), _snakes.at(i+1)->body.end(),[x, y](SDL_Point itr){return (itr.x == x && itr.y == y);}) || (x == static_cast<int>(_snakes.at(i+1)->head_x) && y == static_cast<int>(_snakes.at(i+1)->head_y))){
          _snakes.at(i)->alive = false;
          _snakes.at(i+1)->alive = false;
        }
      }
      else{
        if(std::any_of(_snakes.at(i-1)->body.begin(), _snakes.at(i-1)->body.end(),[x, y](SDL_Point itr){return (itr.x == x && itr.y == y);}) || (x == static_cast<int>(_snakes.at(i-1)->head_x) && y == static_cast<int>(_snakes.at(i-1)->head_y))){
          _snakes.at(i)->alive = false;
          _snakes.at(i-1)->alive = false;
        } 
      }
    }
    
    int t = 0;
    for(auto fdr : _food){
      if(fdr.x == x && fdr.y == y){
        _food.erase(_food.begin() + t);
        score.at(i)++;
        _snakes.at(i)->GrowBody();
        _snakes.at(i)->speed += 0.02;
        snake_sizes.at(i) = _snakes.at(i)->size;
        PlaceFood();
        break;
      }
      t++;
    }
  }
}


 Game::~Game(){

   if(snake_sizes.size() > 1){
    std::cout << "Score for Player 1: " << score.at(0) << "  " << "Score for Player 2: " << score.at(1) << std::endl;
    std::cout << "Size of Snake 1: " << snake_sizes.at(0) << "  " << "Size of Snake 2: " << snake_sizes.at(1) << std::endl;
   }
   else{
    std::cout << "Score for Player 1: " << score.at(0) << std::endl;
    std::cout << "Size of Snake 1: " << snake_sizes.at(0) <<  std::endl;
   }

   std::cout << "Thank you for playing!" << std::endl;


 }

