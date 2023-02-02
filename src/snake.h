#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <memory>
#include <thread>
#include <algorithm>
#include <functional>
#include "SDL.h"
#include "obstacle.h"




class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height, int _playerN, int _head_x, int _head_y)
      : grid_width(grid_width),
        grid_height(grid_height),
        playerN(_playerN),
        head_x(_head_x),
        head_y(_head_y) {}

  ~Snake();
  
        

  void launch();
  void run();
  void Update();

  void GrowBody();
  bool SnakeCell(int x, int y);
  void GetObstacles(std::vector<std::shared_ptr<Obstacle>> obstacles);
  void CheckObstacle(SDL_Point curr_cell);


  Direction direction = Direction::kUp;
  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
  int playerN;

 protected:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);
  bool growing{false};
  std::vector<std::thread> threads;
  std::vector<std::shared_ptr<Obstacle>> _obstacles;
  int grid_width;
  int grid_height;


};

#endif