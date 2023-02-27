#include <iostream>
#include <vector>
#include "controller.h"
#include "game.h"
#include "renderer.h"

int main() {
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  // Code that prompts player to choose between 1 or 2 player modes, code prompt will keep looping until either "1" or "2" is entered 
  int nPlayers;
  std::cout << "Enter 1 for 1 player mode, or 2 for 2 player mode" << std::endl;
  std::cin >> nPlayers;
  while(true){
    if(nPlayers != 1 && nPlayers != 2){
      std::cout << "Enter a valid number" << std::endl;
      std::cin >> nPlayers;
    }
    else{break;}
  }

  // Intialization of Game and Renderer class instances 
  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  Game game(kGridWidth, kGridHeight, nPlayers);

  // Calling of main game run method. 
  game.Run(renderer, kMsPerFrame);
  std::cout << "Game has terminated successfully!\n";
  return 0;
}