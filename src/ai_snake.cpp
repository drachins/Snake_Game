#include "ai_snake.h"
#include "game.h"
#include <iostream>
#include <cmath>



void AI_Snake::launch_ai_snake(){
    _ai_threads.emplace_back(&AI_Snake::run_ai_snake, this);
}

void AI_Snake::run_ai_snake(){

    AI_Snake::State algo_state = State::kRunning;
    init = {static_cast<int>(head_x), static_cast<int>(head_y)};
    std::vector<int> new_goal = FindNearestFood(init.at(0), init.at(1)); 
    goal[0] = new_goal.at(0);
    goal[1] = new_goal.at(1);
    grid = _game->getGrid();

    while(alive){

        while(algo_state == State::kRunning){
            algo_state = AStarSearch();
        }
        State new_state;
        if(_game->_newCycle == State::kOldCycle){
            new_state = _game->WaitforNewCycle();
        }
        if(new_state == State::kNewCycle)
            std::cout << "kNewCycle" << std::endl;
        else
            std::cout << "kOldCycle" << std::endl;
        algo_state = State::kRunning;
        init.at(0) = static_cast<int>(head_x);
        init.at(1) = static_cast<int>(head_y);   
        std::vector<int> new_goal = FindNearestFood(init.at(0), init.at(1)); 
        goal[0] = new_goal.at(0);
        goal[1] = new_goal.at(1);
        grid = _game->getGrid();   

    }
    
}

bool Compare(const std::vector<int> vec_a, const std::vector<int> vec_b){
    int f1 = vec_a.at(2) + vec_a.at(3);
    int f2 = vec_b.at(2) + vec_b.at(3);
    return(f1 > f2);
} 


void AI_Snake::CellSort(std::vector<std::vector<int>> &vec){
    std::sort(vec.begin(), vec.end(), Compare);
}

int AI_Snake::Hueristic(int x1, int y1, int x2, int y2){
    return(abs(x2 - x1) + abs(y2 - y1));
}

bool AI_Snake::CheckValidCell(int x, int y, std::vector<std::vector<AI_Snake::State>> &grid){
    bool on_grid_x  = (x >= 0 && x < grid.size());
    bool on_grid_y = (y >= 0 && y < grid.at(0).size());
    if(on_grid_x && on_grid_y){
        if(grid[x][y] == State::kEmpty || grid[x][y] == State::kFood)
            return true;
    }
    return false;
}

void AI_Snake::AddToOpen(int x, int y, int g, int h, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){
    open_list.push_back(std::vector<int>{x, y, g, h});
    grid.at(x).at(y) = State::kClosed;
}

std::vector<int> AI_Snake::FindNearestFood(int x, int y){

    // Get coodinates of food items.
    std::vector<std::vector<int>> food = _game->getFoodCoords();
    // Sort food item coords from lowest to highest hueristic 
    std::sort(food.begin(), food.end(), [x,y](std::vector<int> a, std::vector<int> b){return(abs(a.at(0) - x) + abs(a.at(1) - y)) < (abs(b.at(0) - x) + abs(b.at(1) - y));});

    // Return food item with lowest hueristic.
    return food.front();

}

void AI_Snake::UpdateStateGrid(){
    std::vector<std::vector<AI_Snake::State>> grid_frame = _game->getGrid();

    for(size_t x  = 0; x < grid_frame.size(); x++){
        for(size_t y = 0; y < grid_frame.at(x).size(); y++){
            if(grid.at(x).at(y) != State::kClosed){
                switch(grid_frame.at(x).at(y)){
                    case State::kFood:
                        grid.at(x).at(y) = State::kFood;
                    break;
                    case State::kObstacle:
                        grid.at(x).at(y) = State::kObstacle;
                    break;
                    case State::kBody:
                        grid.at(x).at(y) = State::kBody;
                    break;
                    case State::kEmpty:
                        grid.at(x).at(y) = State::kEmpty;
                    break;
                }
            }            
        }
    } 
}

void AI_Snake::SetDirection(int current_cell[], int previous_cell[]){

    if(current_cell[0] == previous_cell[0] + delta[0][0]){
        direction = Direction::kRight;
    }
    else if(current_cell[0] == previous_cell[0] + delta[2][0]){
        direction = Direction::kLeft;
    }
    else if(current_cell[1] == previous_cell[1] + delta[3][1]){
        direction = Direction::kUp;
    }
    else if(current_cell[1] == previous_cell[1] + delta[1][1]){
        direction = Direction::kDown;
    }
    
}


void AI_Snake::ExpandToNeighbors(int current[], int goal[], std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){

    // Get current node's date.
    int x = current[0];
    int y = current[1];
    int g = current[2];  

    // Loop through current node's potential neighbors.
    for(int i = 0; i < 4; i++){
        int x2 = static_cast<int>(fmod(x + delta[i][0] + grid_width, grid_width));
        int y2 = static_cast<int>(fmod(y + delta[i][1] + grid_height, grid_height));
        //std::cout << i << std::endl;
        //Check if potential neighbor is a valid cell.
        if(CheckValidCell(x2, y2, grid)){
            int g2 = g + 1;
            int h2 = Hueristic(x2, y2, goal[0], goal[1]);
            AddToOpen(x2, y2, g2, h2, open_list, grid);
            
        }

    }

}



AI_Snake::State AI_Snake::AStarSearch(){
    
    // Initialize search algorithm
    int x = init[0];
    int y = init[1];
    int g = 0;
    int h = Hueristic(x, y, goal[0], goal[1]);
    AddToOpen(x, y, g, h, open_list, grid);
    AI_Snake::State ASearch_State = State::kRunning;
    int current_cell[4] = {0};
    for(size_t n = 0; n < init.size(); n++){
        current_cell[n] = init.at(n);
    }
    int previous_cell[4] = {0};
    int cycle = 0;


    // Search loop.
    while(true){
        if(current_cell[0] == static_cast<int>(head_x) && current_cell[1] == static_cast<int>(head_y)){
            UpdateStateGrid();
            CellSort(open_list);
            for(size_t i = 0; i < open_list.back().size(); i++){
                current_cell[i] = open_list.back().at(i);
            }
            if(cycle > 0){
                SetDirection(current_cell, previous_cell);
            }
            open_list.clear();
            if((current_cell[0] == goal[0] && current_cell[0] == goal[1]) || grid.at(goal[0]).at(goal[1]) != State::kFood){
                while(!(current_cell[0] == static_cast<int>(head_x) && current_cell[1] == static_cast<int>(head_y))){
                    Update();
                }
                ASearch_State = State::kGoal;
                break;
            }

            ExpandToNeighbors(current_cell, goal, open_list, grid);
            std::this_thread::sleep_for(std::chrono::milliseconds(190));
            if(!running || !alive){
                break;
            }
            for(size_t n = 0; n < sizeof(current_cell)/sizeof(int); n++){
                previous_cell[n] = current_cell[n];
            }
            cycle++;           
        }
        else{
            Update();
        }       


    }
    return ASearch_State;
}
   

void AI_Snake::setGameHandle(Game *game){
    _game = game;
}

AI_Snake::~AI_Snake(){
    std::for_each(_ai_threads.begin(), _ai_threads.end(), [](std::thread &thr){thr.join();});
}



