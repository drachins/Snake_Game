#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Controller {
 public:
  void HandleInput1Player(bool &running, std::vector<std::shared_ptr<Snake>> snake) const;
  void HandleInput2Players(bool &running, std::vector<std::shared_ptr<Snake>> snake) const;

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif