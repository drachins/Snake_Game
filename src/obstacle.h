#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "SDL.h"

class Obstacle{

    private:
        SDL_Point obs;
    
    public:
        // Setters/Getters
        void SetObstacleCoords(int x, int y);
        int GetObstacleXCoord(){return obs.x;}
        int GetObstacleYCoord(){return obs.y;}
        

};


#endif 
