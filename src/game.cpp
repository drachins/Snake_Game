
#include <iostream>
#include <algorithm>
#include <cmath>
#include "SDL.h"
#include "game.h"
#include "controller.h"

// Definition of messanger object recieve(), and send() functions. 
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

// Game class constructor definition. 
Game::Game(std::size_t grid_width, std::size_t grid_height, int _nPlayers)
    : _grid_width(grid_width),
      _grid_height(grid_height),
      engine(dev()),
      nPlayers(_nPlayers),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  

  // Vectors that store scrore and size of each snake is initialized.
  score.resize(nPlayers + 1);
  snake_sizes.resize(nPlayers + 1);
  std::fill(score.begin(), score.end(), 0);
  std::fill(snake_sizes.begin(), snake_sizes.end(), 0);

  // Initilization of state grid with all states being set to kEmoty. 
  std::vector<AI_Snake::State> column (_grid_width+1, AI_Snake::State::kEmpty);
  for(size_t x = 0; x < _grid_width+1; x++){
    _states.push_back(column);
  }

  // Function calls that place starting postion of snakes, food items, and obstacles. 
  PlaceSnakes();
  PlaceFood();
  PlaceObstacle();
}

// Getter function that is used by controller threads to receive value of key press events. 
SDL_Keycode Game::GetKeypress(int ind){

  return result.at(ind);

}


// Defnition od main game loop function. 
void Game::Run(Renderer &renderer, std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running{true};

  // For loop that initializes controller instances. 
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

  // Code that launched threads of the player controlled snakes, controllers, and the AI controlled snake. 
  std::for_each(_snakes.begin(), _snakes.end(), [](std::shared_ptr<Snake> &itr){itr->launch();});
  std::for_each(_controllers.begin(), _controllers.end(), [](std::unique_ptr<Controller> &ctr){ctr->launch();});
  _ai_snake->launch_ai_snake();

  // Main game loop. 
  while (running) {

    // Block of code that polls for keyboard press events and determines which control scheme they correspond to which is either the arrow keys for player 1,
    // or the wasd keys for player 2, and also if the exit button is pressed on the game window. I took the polling for key events code outside of the Controller 
    // class, as it seems that only 1 thread can poll for key presses, any other threads polling wont detect a keypress, it appears to be a limitation of the
    // SDL2 library. 
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
  
    // Updates the ai_snake running variable to running. 
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

  // Code that kills the thread loops of the player conrolled snakes, thier respective controllers, and the AI snake. 
  std::for_each(_snakes.begin(), _snakes.end(), [](std::shared_ptr<Snake> &itr){itr->alive = false;});
  std::for_each(_controllers.begin(), _controllers.end(), [](std::unique_ptr<Controller> &ctr){ctr->control_running = false;});
  _ai_snake->alive = false;
}

// Function that places player controlled and AI controlled snakes in random positions on the game map. 
void Game::PlaceSnakes() {

  // For loop that initilizes player controlled snakes. 
  for(int i = 0; i < nPlayers; i++){
    int x = random_w(engine);
    int y = random_h(engine);
    _snakes.push_back(std::make_shared<Snake>(_grid_width, _grid_height, i, x, y));
  }
 
  // Code that initializes AI contolled snake.  
  int x, y;
  x = random_w(engine);
  y = random_h(engine);
  _ai_snake = std::make_shared<AI_Snake>(_grid_width, _grid_height, 0, x, y);
  _ai_snake->setGameHandle(this);

}

// Function that placed food on game map. 
void Game::PlaceFood() {
  // Checks if _food vector is empty, which is the case at the beginning of the game, 
  if(_food.empty()){
    // For loop that randomly sets postion of food items on game map.
    for(int i = 0; i < nFood; i++){
      int x, y;
      SDL_Point fd;
      while(true){
        x = random_w(engine);
        y = random_h(engine);
        // Checks if randomly generated numbers conflict with the position of any player or AI controlled snakes. 
        if(std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr){return !itr->SnakeCell(x,y);}) && !_ai_snake->SnakeCell(x,y)){
          fd.x = x;
          fd.y = y;
          _food.push_back(fd);
          break;
        }
      }
    }
  }
  // Block of code that will run through out rest of game. 
  else{
    int x, y;
    SDL_Point fd;
    while(true){
      x = random_w(engine);
      y = random_h(engine);
      // Checks if randomly generated numbers conflict with position of obstacle objects, or postion of any player or AI controlled snakes. 
      if(std::any_of(_obstacles.begin(), _obstacles.end(), [x, y](std::shared_ptr<Obstacle> &itr){return !(itr->GetObstacleXCoord() == x && itr->GetObstacleYCoord() == y);}) && std::any_of(_snakes.begin(), _snakes.end(), [x,y](std::shared_ptr<Snake> &itr) {return !itr->SnakeCell(x,y);}) && !_ai_snake->SnakeCell(x,y)){
        fd.x = x;
        fd.y = y;
        _food.push_back(fd);
        break;
      }
    }
  }
}


// Function that randomly places obstacle objects on game map. 
void Game::PlaceObstacle() {
  for(int i = 0; i < nObstacles; i++){
    Obstacle obstacle;
    int x, y;
    while(true){
      x = random_w(engine);
      y = random_h(engine);
      // Checks if randomly generated numbers conflict with position of food items, or player, and AI controlled snakes. 
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

// Functions that is called by AI_Snake class instance to refresh state grid. 
std::vector<std::vector<AI_Snake::State>> Game::getGrid(){

  // Nested for loops that loop through each row then each colounm of game map and checks if each map coordinate cell corresponds to a coordinate position 
  // of a snake, AI snake, food, or obstacle object and the state of that coordinate cell is set accordingly. 
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

// Getter function that is called by AI_Snake class instance to get coordinates of all food items. 
std::vector<std::vector<int>> Game::getFoodCoords(){

  std::vector<std::vector<int>> food_vec;

  for(auto fdr : _food){
    food_vec.push_back(std::vector<int>{fdr.x, fdr.y});
  }

  return food_vec;

}

// Function that is called by AI_Snake class instance to wait for a State::kNewCycle. 
AI_Snake::State Game::WaitforNewCycle(){

  while(true){

      AI_Snake::State cycle = _cycleMsg.recieve();
      if(cycle == AI_Snake::State::kNewCycle){
        return cycle;
      }

  }

}

// Function that checks for collision bewtween player or AI controlled snakes. 
void Game::CheckForCollision(int i, int x, int y, int ai_x, int ai_y){

  // if-else if block that checks if a player controlled snake has collided with a AI controlled snake. 
  if(std::any_of(_ai_snake->body.begin(), _ai_snake->body.end(), [x, y](SDL_Point itr){return(itr.x == x && itr.y == y);})){
      _snakes.at(i)->alive = false;
      _ai_snake->alive = false;
  }
  else if(std::any_of(_snakes.at(i)->body.begin(), _snakes.at(i)->body.end(), [ai_x, ai_y](SDL_Point itr){return(itr.x == ai_x && itr.y == ai_y);})){
    _snakes.at(i)->alive = false;
    _ai_snake->alive = false;
  }


  if(nPlayers > 1){
    // if-else block that checks if snakes controlled by player 1 and player 2 have collided with each other. 
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

  // Sets _newCycle to kOldCycle and calls _cycleMsg.send() function, which signals to AI_Snake class instance thread not to start a new A* search 
  // cycle. 
  _newCycle = AI_Snake::State::kOldCycle;
  _cycleMsg.send(std::move(_newCycle));

  
  // For loop that loops through each player, and AI controlled snake class instance and checks if if each snake has encountred a food item. 
  for(int i = 0; i < nPlayers; i++){
    // Get current position of snake head.
    int x = static_cast<int>(_snakes.at(i)->head_x);
    int y = static_cast<int>(_snakes.at(i)->head_y);
    
    // Get current position of AI snake head. 
    int ai_x = static_cast<int>(_ai_snake->head_x);
    int ai_y = static_cast<int>(_ai_snake->head_y);

    // Check if snake or ai snake has had a collision. 
    CheckForCollision(i, x, y, ai_x, ai_y);
    
    int t = 0;
    // For loop that loops through _food vector, and checks if food coordinates match coordinated of player and AI controlled snake heads. 
    for(auto fdr : _food){
      if(fdr.x == x && fdr.y == y){
        _food.erase(_food.begin() + t);
        score[i+1]++;
        _snakes.at(i)->GrowBody();
        _snakes.at(i)->speed += 0.02;
        snake_sizes.at(i+1) = _snakes.at(i)->size;
        PlaceFood();
        // Code that signals to AI_Snake thread the creation of new food item, so a new A* search alogrithm cycle can be started. 
        _newCycle = AI_Snake::State::kNewCycle;
        _cycleMsg.send(std::move(_newCycle));
        break;
      }
      else if(fdr.x == ai_x && fdr.y == ai_y){
        _food.erase(_food.begin() + t);
        score[0]++;
        _ai_snake->GrowBody();
        // Code that caps AI controlled snake speed to 0.3, 
        if(_ai_snake->speed < 0.3){
          _ai_snake->speed += 0.02;
        }
        snake_sizes.at(0) = _ai_snake->size;
        PlaceFood();
        // Code that signals to AI_Snake thread the creation of new food item, and snake speed and size is updated so a new A* search alogrithm cycle can be started. 
        _newCycle = AI_Snake::State::kNewCycle;
        _cycleMsg.send(std::move(_newCycle));
        break;
      }
      t++;
    }
  }
}


// Game class destructor. 
Game::~Game(){

  // Score for each player, and AI is printed to terminal if 2 player mode is used.
  if(snake_sizes.size() > 2){
    std::cout << "Score for Player 1: " << score.at(1) << ", " << "Score for Player 2: " << score.at(2) << ", " << "Score for Computer: " << score.at(0) << std::endl;
    std::cout << "Size of Snake 1: " << snake_sizes.at(1) << ", " << "Size of Snake 2: " << snake_sizes.at(2) << ", " << "Size of Comp Snake: " << snake_sizes.at(0) << std::endl;
  }
  else{
    // Score for player, and AI is printed to terminal if 1 player mode is used. 
    std::cout << "Score for Player 1: " << score.at(1) << ", " << "Score for Computer: " << score.at(0) << std::endl;
    std::cout << "Size of Snake 1: " << snake_sizes.at(1) << ", " <<  "Size of Comp Snake: " << snake_sizes.at(0) << std::endl;
  }

  // Final message that printed to terminal. 
  std::cout << "Thank you for playing!" << std::endl;


}

