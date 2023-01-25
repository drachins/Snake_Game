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

  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  std::vector<Controller> controllers;
  for(int i = 0; i < nPlayers; i++){
    controllers.push_back(Controller());
  }

  Game game(kGridWidth, kGridHeight, nPlayers);
  game.Run(controllers, renderer, kMsPerFrame);
  //std::vector<int> score = game.GetScore();
  //std::vector<int> size = game.GetSize();
  std::cout << "Game has terminated successfully!\n";
  //std::cout << "Score for Player 1: " << score[0] << "Score for Player 2: " << score[1] <<"\n";
  //std::cout << "Size Snake 1: " << size[0] << "Snize Snake 2: " << size[1] << "\n";
  return 0;
}