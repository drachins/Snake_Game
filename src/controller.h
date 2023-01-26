#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Controller {
 public:
  void RightController(bool &running, std::shared_ptr<Snake> snake) const;
  void LeftController(bool &running, std::shared_ptr<Snake> snake) const;

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif