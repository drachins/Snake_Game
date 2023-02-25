
#include <iostream>
#include <algorithm>
#include <functional>
#include <cmath>
#include "SDL.h"
#include "game.h"
#include "controller.h"


template <typename T>
T CycleNotify<T>::recieve(){

  std::unique_lock<std::mutex> _rLock(_mtx);
  _cond.wait(_rLock, [this] {return !_queue.empty();});
  
  T message = _queue.back();
  _queue.pop_back();

  return message;

}

template <typename T>
void CycleNotify<T>::send(T &&msg){

  std::lock_guard<std::mutex> _sLock(_mtx);

  _queue.push_back(msg);

  _cond.notify_one();

}


Game::Game(std::size_t grid_width, std::size_t grid_height, int _nPlayers)
    : _grid_width(grid_width),
      _grid_height(grid_height),
      engine(dev()),
      nPlayers(_nPlayers),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  
  
  score.resize(nPlayers + 1);
  snake_sizes.resize(nPlayers + 1);
  std::fill(score.begin(), score.end(), 0);
  std::fill(snake_sizes.begin(), snake_sizes.end(), 0);

  std::vector<AI_Snake::State> column (_grid_width+1, AI_Snake::State::kEmpty);
  for(size_t x = 0; x < _grid_width+1; x++){
    _states.push_back(column);
  }

  PlaceSnakes();
  PlaceFood();
  PlaceObstacle();
}

SDL_Keycode Game::GetKeypress(int ind){

  return result.at(ind);

}



void Game::Run(Renderer &renderer, std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running{true};

  for(int i = 0; i < nPlayers; i++){
    if(i == 0){
      _controllers.push_back(std::make_unique<Controller>(SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, _snakes.at(i), i));
      _controllers.at(i)->setGameHandle(this);
    }
    else{
      _controllers.push_back(std::make_unique<Controller>(SDLK_w, SDLK_s, SDLK_a, SDLK_d, _snakes.at(i), i));
      _controllers.at(i)->setGameHandle(this);      
    }
  }



  std::for_each(_snakes.begin(), _snakes.end(), [](std::shared_ptr<Snake> &itr){itr->launch();});
  std::for_each(_controllers.begin(), _controllers.end(), [](std::unique_ptr<Controller> &ctr){ctr->launch();});
  _ai_snake->launch_ai_snake();
  
  
  while (running) {

    SDL_Keycode key_code;
    SDL_Event e;
    while(SDL_PollEvent(&e)){
      if(e.type == SDL_QUIT){
        running = false;
      }
      else if(e.type == SDL_KEYDOWN){
        key_code = e.key.keysym.sym;

        if(any_of(key_codes_controller1.begin(), key_codes_controller1.end(), [key_code](SDL_Keycode itr){return itr == key_code;}))
          result.at(0) = key_code;

        else if(any_of(key_codes_controller2.begin(), key_codes_controller2.end(), [key_code](SDL_Keycode itr){return itr == key_code;}))
          result.at(1) = key_code;
        
        }
      }
  

    _ai_snake->setRunning(running);

    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.

    Update();
    renderer.Render(_snakes, _ai_snake, _food, _obstacles);

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
  std::for_each(_controllers.begin(), _controllers.end(), [](std::unique_ptr<Controller> &ctr){ctr->control_running = false;});
  _ai_snake->alive = false;
}

void Game::PlaceSnakes() {

  for(int i = 0; i < nPlayers; i++){
    int x = random_w(engine);
    int y = random_h(engine);
    _snakes.push_back(std::make_shared<Snake>(_grid_width, _grid_height, i, x, y));
  }

  int x, y;
  x = random_w(engine);
  y = random_h(engine);
  _ai_snake = std::make_shared<AI_Snake>(_grid_width, _grid_height, 0, x, y);
  _ai_snake->setGameHandle(this);

}

void Game::PlaceFood() {
  if(_food.empty()){
    for(int i = 0; i < nFood; i++){
      int x, y;
      SDL_Point fd;
      while(true){
        x = random_w(engine);
        y = random_h(engine);
        if(std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr){return !itr->SnakeCell(x,y);}) && !_ai_snake->SnakeCell(x,y)){
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
      if(std::any_of(_obstacles.begin(), _obstacles.end(), [x, y](std::shared_ptr<Obstacle> &itr){return !(itr->GetObstacleXCoord() == x && itr->GetObstacleYCoord() == y);}) && std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr) {return !itr->SnakeCell(x,y);}) && !_ai_snake->SnakeCell(x,y)){
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
      if(std::any_of(_food.cbegin(), _food.cend(), [x,y](SDL_Point itr){return !(itr.x == x && itr.y == y);}) && std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr){return !itr->SnakeCell(x,y);}) && !_ai_snake->SnakeCell(x,y)){
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

std::vector<std::vector<AI_Snake::State>> Game::getGrid(){

  for(size_t x = 0; x < _grid_width+1; x++){
    for(size_t y = 0; y < _grid_height+1; y++){
      if(std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &snk){return snk->SnakeCell(x,y);})){
        _states.at(x).at(y) = AI_Snake::State::kObstacle;
      }
      else if(_ai_snake->SnakeCell(x,y)){
        _states.at(x).at(y) = AI_Snake::State::kBody;
      }
      else if(std::any_of(_obstacles.begin(), _obstacles.end(), [x,y](std::shared_ptr<Obstacle> &obs){return(obs->GetObstacleXCoord() == x && obs->GetObstacleYCoord() == y);})){
        _states.at(x).at(y) = AI_Snake::State::kObstacle;
      }
      else if(std::any_of(_food.begin(), _food.end(), [x,y](SDL_Point fdr){return(fdr.x == x && fdr.y == y);})){
        _states.at(x).at(y) = AI_Snake::State::kFood;
      }
      else{
        _states.at(x).at(y) = AI_Snake::State::kEmpty;
      }
    }
  }

  return _states;

}

std::vector<std::vector<int>> Game::getFoodCoords(){

  std::vector<std::vector<int>> food_vec;

  for(auto fdr : _food){
    food_vec.push_back(std::vector<int>{fdr.x, fdr.y});
  }

  return food_vec;

}

AI_Snake::State Game::WaitforNewCycle(){

  while(true){

      AI_Snake::State cycle = _cycleMsg.recieve();
      if(cycle == AI_Snake::State::kNewCycle){
        return cycle;
      }

  }

}

void Game::CheckForCollision(int i, int x, int y, int ai_x, int ai_y){

  if(std::any_of(_ai_snake->body.begin(), _ai_snake->body.end(), [x, y](SDL_Point itr){return(itr.x == x && itr.y == y);})){
      _snakes.at(i)->alive = false;
      _ai_snake->alive = false;
  }
  else if(std::any_of(_snakes.at(i)->body.begin(), _snakes.at(i)->body.end(), [ai_x, ai_y](SDL_Point itr){return(itr.x == ai_x && itr.y == ai_y);})){
    _snakes.at(i)->alive = false;
    _ai_snake->alive = false;
  }

  if(nPlayers > 1){
    if(i < 1){
      if(std::any_of(_snakes.at(i+1)->body.begin(), _snakes.at(i+1)->body.end(),[x, y](SDL_Point itr){return (itr.x == x && itr.y == y);})){
        _snakes.at(i)->alive = false;
        _snakes.at(i+1)->alive = false;
      }
    }
    else{
      if(std::any_of(_snakes.at(i-1)->body.begin(), _snakes.at(i-1)->body.end(),[x, y](SDL_Point itr){return (itr.x == x && itr.y == y);})) {
        _snakes.at(i)->alive = false;
        _snakes.at(i-1)->alive = false;
      } 
    }
  }

}


void Game::Update() {

  _newCycle = AI_Snake::State::kOldCycle;
  _cycleMsg.send(std::move(_newCycle));

  
  for(int i = 0; i < nPlayers; i++){
    int x = static_cast<int>(_snakes.at(i)->head_x);
    int y = static_cast<int>(_snakes.at(i)->head_y);
    
    int ai_x = static_cast<int>(_ai_snake->head_x);
    int ai_y = static_cast<int>(_ai_snake->head_y);


    CheckForCollision(i, x, y, ai_x, ai_y);
    
    int t = 0;
    for(auto fdr : _food){
      if(fdr.x == x && fdr.y == y){
        _food.erase(_food.begin() + t);
        score[i+1]++;
        _snakes.at(i)->GrowBody();
        _snakes.at(i)->speed += 0.02;
        snake_sizes.at(i+1) = _snakes.at(i)->size;
        PlaceFood();
        _newCycle = AI_Snake::State::kNewCycle;
        _cycleMsg.send(std::move(_newCycle));
        break;
      }
      else if(fdr.x == ai_x && fdr.y == ai_y){
        _food.erase(_food.begin() + t);
        score[0]++;
        _ai_snake->GrowBody();
        if(_ai_snake->speed < 0.3){
          _ai_snake->speed += 0.02;
        }
        snake_sizes.at(0) = _ai_snake->size;
        PlaceFood();
        _newCycle = AI_Snake::State::kNewCycle;
        _cycleMsg.send(std::move(_newCycle));
        break;
      }
      t++;
    }
  }
}



Game::~Game(){


  if(snake_sizes.size() > 2){
    std::cout << "Score for Player 1: " << score.at(1) << " , " << "Score for Player 2: " << score.at(2) << " , " << "Score for Computer: " << score.at(0) << std::endl;
    std::cout << "Size of Snake 1: " << snake_sizes.at(1) << " , " << "Size of Snake 2: " << snake_sizes.at(2) << " , " << "Size of AI Snake: " << snake_sizes.at(0) << std::endl;
  }
  else{
    std::cout << "Score for Player 1: " << score.at(1) << " , " << "Score for Computer: " << score.at(0) << std::endl;
    std::cout << "Size of Snake 1: " << snake_sizes.at(1) << " , " <<  "Size of AI Snake: " << snake_sizes.at(0) << std::endl;
  }

  std::cout << "Thank you for playing!" << std::endl;


}

