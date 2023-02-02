#ifndef GAME_H
#define GAME_H

#include <random>
#include <memory>
#include <vector>
#include <mutex>
#include "SDL.h"
#include "renderer.h"
#include "snake.h"
#include "obstacle.h"
#include "ai_snake.h"

class Controller;


class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, int nPlayers);
  ~Game();
  void Run(Renderer &renderer,std::size_t target_frame_duration);
  SDL_Keycode GetKeypress(int ind);
  std::vector<std::vector<AI_Snake::State>> getGrid();
  std::vector<std::vector<int>> getFoodCoords();

  std::vector<std::shared_ptr<Obstacle>> _obstacles;
  std::vector<std::shared_ptr<Snake>> _snakes;
  std::vector<std::unique_ptr<Controller>> _controllers;
  std::vector<std::vector<AI_Snake::State>> _states;
  std::shared_ptr<AI_Snake> _ai_snake;

  std::size_t _grid_width;
  std::size_t _grid_height;

 private:

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  
  std::vector<SDL_Point> _food;
  std::vector<int> score;
  std::vector<int> snake_sizes;
  std::vector<SDL_Keycode> key_codes_controller1 {SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT};
  std::vector<SDL_Keycode> key_codes_controller2 {SDLK_w, SDLK_s, SDLK_a, SDLK_d};
  std::vector<SDL_Keycode> result {SDLK_UP, SDLK_w};
  int nFood{2};
  int nObstacles{3};
  int nPlayers;
  SDL_Keycode key_code;
  
  void PlaceFood();
  void Update();
  void PlaceObstacle();
};

#endif