#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <memory>
#include <thread>
#include "SDL.h"
#include "obstacle.h"

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height, int _playerN)
      : grid_width(grid_width),
        grid_height(grid_height),
        playerN(_playerN),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void launch();
  void Update();

  void GrowBody();
  bool SnakeCell(int x, int y);
  void GetObstacles(std::vector<std::shared_ptr<Obstacle>> obstacles);
  void CheckObstacle(std::vector<std::shared_ptr<Obstacle>> obstacles, SDL_Point curr_cell);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
  int playerN;

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  bool growing{false};
  int grid_width;
  int grid_height;
  std::vector<std::thread> threads;
  std::vector<std::shared_ptr<Obstacle>> _obstacles;
};

#endif