#ifndef GAME_H
#define GAME_H

#include <random>
#include <memory>
#include <vector>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "obstacle.h"


class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, int nPlayers);
  void Run(std::vector<Controller> const &controllers, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  //int GetSize() const;

  std::vector<std::shared_ptr<Obstacle>> _obstacles;
  std::vector<std::shared_ptr<Snake>> _snakes;
  

 private:

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  
  std::vector<SDL_Point> _food;

  int score{0};
  int nFood{2};
  int nObstacles{3};
  int nPlayers;

  void PlaceFood();
  void Update();
  void PlaceObstacle();
};

#endif