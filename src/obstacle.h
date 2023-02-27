#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "SDL.h"

// Class definition of Obstacle class
class Obstacle{

    public:
        // Setters/Getters
        void SetObstacleCoords(int x, int y);
        int GetObstacleXCoord() const {return obs.x;}
        int GetObstacleYCoord() const {return obs.y;}


    private:
        SDL_Point obs;
    
    
        

};


#endif 
