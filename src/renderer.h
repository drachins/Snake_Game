#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <memory>
#include "SDL.h"
#include "snake.h"
#include "obstacle.h"
#include "ai_snake.h"

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Render(std::vector<std::shared_ptr<Snake>> const snakes, std::shared_ptr<AI_Snake> const ai_snake, std::vector<SDL_Point> const food, std::vector<std::shared_ptr<Obstacle>> const obstacles);
  void UpdateWindowTitle(std::vector<int> score, int fps);


 private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
};

#endif