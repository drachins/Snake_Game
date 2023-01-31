#ifndef GAME_H
#define GAME_H

#include <random>
#include <memory>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "SDL.h"
#include "renderer.h"
#include "snake.h"
#include "obstacle.h"

class Controller;


template<class T>
class EventQueue{

  public:

    void send(T &&msg);
    T recieve();

  private:
    std::deque<SDL_Keycode> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;

};


class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, int nPlayers);
  ~Game();
  void Run(Renderer &renderer,std::size_t target_frame_duration);
    SDL_Keycode waitForEventMsg();

  std::vector<std::shared_ptr<Obstacle>> _obstacles;
  std::vector<std::shared_ptr<Snake>> _snakes;
  std::vector<Controller> _controllers;
  

 private:

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  
  std::vector<SDL_Point> _food;
  std::vector<int> score;
  std::vector<int> snake_sizes;
  int nFood{2};
  int nObstacles{3};
  int nPlayers;
  SDL_Keycode key;

  EventQueue<SDL_Keycode> _eventMsgs;

  void PlaceFood();
  void Update();
  void PlaceObstacle();
};

#endif