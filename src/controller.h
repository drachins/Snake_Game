#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "SDL.h"
#include "snake.h"

class Controller {
 public:
    
    Snake::Direction RightController(Snake &snake) const;
    Snake::Direction LeftController(Snake &snake) const;



 private:
    Snake::Direction ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
    
};

#endif