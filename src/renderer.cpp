#include "renderer.h"
#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::Render(std::vector<std::shared_ptr<Snake>> const snakes, std::shared_ptr<AI_Snake> const ai_snake, std::vector<SDL_Point> const food, std::vector<std::shared_ptr<Obstacle>> const obstacles) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;
  

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  for(auto fdr : food){
    SDL_SetRenderDrawColor(sdl_renderer, 0x7A, 0xF9, 0X4F, 0xFF);
    block.x = fdr.x * block.w;
    block.y = fdr.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);    
  }

  // Render snake's body
  for(auto snk : snakes){
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (SDL_Point const &point : snk->body) {
      block.x = point.x * block.w;
      block.y = point.y * block.h;
      SDL_RenderFillRect(sdl_renderer, &block);
    }
  }

  // Render AI snakes body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : ai_snake->body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }
  
  // Render obstacles
  for(auto itr : obstacles){
    SDL_SetRenderDrawColor(sdl_renderer, 0xA0, 0xA0, 0XA0, 0xFF);
    block.x = itr->GetObstacleXCoord() * block.w;
    block.y = itr->GetObstacleYCoord() * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  for(auto snk : snakes){
    block.x = static_cast<int>(snk->head_x) * block.w;
    block.y = static_cast<int>(snk->head_y) * block.h;
    if (snk->alive) {
      if(snk->playerN == 0){
        SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
      }
      else{ 
        SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x91, 0x00, 0xFF);
      }
    } 
    else {
      SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
    }
    SDL_RenderFillRect(sdl_renderer, &block);    
  }

  // Render AI Snakes head
  block.x = static_cast<int>(ai_snake->head_x) * block.w;
  block.y = static_cast<int>(ai_snake->head_y) * block.h;
  if (ai_snake->alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x99, 0xCC, 0xFF, 0xFF);
  }
  else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);    
  

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
  
  
}

void Renderer::UpdateWindowTitle(std::vector<int> score, int fps) {
  std::string title;
  if(score.size() > 1){
    title = " Snake 1 " + std::to_string(score[0]) + " Snake 2 " +std::to_string(score[1]) + " FPS: " + std::to_string(fps);
  }
  else{
    title = " Snake 1 " + std::to_string(score[0]) + " FPS: " + std::to_string(fps);
  }
  
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
