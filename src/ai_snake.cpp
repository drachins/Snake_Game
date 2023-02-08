#include "ai_snake.h"
#include "game.h"
#include <iostream>
#include <mutex>

void AI_Snake::launch_ai_snake(){
    _threads.emplace_back(&AI_Snake::run_ai_snake, this);
}

void AI_Snake::run_ai_snake(){
    AI_Snake::State algo_state = State::kRunning;
    init = {static_cast<int>(head_x), static_cast<int>(head_y)};
    goal = FindNearestFood(init.at(0), init.at(1)); 
    grid = _game->getGrid();
    while(alive){
        while(algo_state == State::kRunning){
            algo_state = AStarSearch();
        }
        algo_state = State::kRunning;
        init.at(0) = static_cast<int>(head_x);
        init.at(1) = static_cast<int>(head_y);   
        goal = FindNearestFood(init.at(0), init.at(1));     
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
    bool on_grid_x  = (x > 0 && x < grid.size());
    bool on_grid_y = (y > 0 && y < grid.at(0).size());
    if(on_grid_x && on_grid_y){
        return (grid[x][y] == State::kEmpty);
    }
    return false;
}

void AI_Snake::AddToOpen(int x, int y, int g, int h, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){
    open_list.push_back(std::vector<int>{x, y, g, h});
    grid.at(y).at(x) = State::kClosed;
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
            if(grid.at(x).at(y) != State::kClosed && grid.at(x).at(y) != State::kPath ){
                switch(grid_frame.at(x).at(y)){
                    case State::kFood:
                        grid.at(x).at(y) = State::kFood;
                    break;
                    case State::kObstacle:
                        grid.at(x).at(y) = State::kObstacle;
                    break;
                    case State::kEmpty:
                        grid.at(x).at(y) = State::kEmpty;
                    break;
                }
            }            
        }
    } 
}

void AI_Snake::SetDirection(std::vector<int> &current_cell){



    if(current_cell.at(0) == open_list.back().at(0) + delta[2][0]){
        direction = Direction::kRight;
        std::cout << "right" << std::endl;
    }
    else if(current_cell.at(0) == open_list.back().at(0) + delta[0][0]){
        direction = Direction::kLeft;
        std::cout << "left" << std::endl;
    }
    else if(current_cell.at(1) == open_list.back().at(1) + delta[3][1]){
        direction = Direction::kUp;
        std::cout << "up" << std::endl;
    }
    else if(current_cell.at(1) == open_list.back().at(1) + delta[1][1]){
        direction = Direction::kDown;
        std::cout << "down" << std::endl;
    }

    std::cout << "Current cell " << current_cell.at(0) << " " << current_cell.at(1) << std::endl;
    
    open_list.pop_back();
    Update();
    
}


void AI_Snake::ExpandToNeighbors(const std::vector<int> &current, std::vector<int> &goal, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){

    std::mutex mtx;
    std::lock_guard<std::mutex>  lck(mtx);
    // Get current node's date.
    int x = current.at(0);
    int y = current.at(1);
    int g = current.at(2);  

    std::cout << "ExpandToNeighbors" << std::endl;

    // Loop through current node's potential neighbors.
    for(int i = 0; i < 4; i++){
        int x2 = x + delta[i][0];
        int y2 = x + delta[i][1];

        //Check if potential neighbor is a valid cell.
        if(CheckValidCell(x2, y2, grid)){
            std::cout << "goal " << goal[0] << " " << goal[1] << std::endl;
            int g2 = g++;
            int h2 = Hueristic(x2, y2, goal[0], goal[1]);
            std::cout << "{ " << x2 << " " << y2 << " " << g2 << " " << h2 << " }, ";
            AddToOpen(x2, y2, g2, h2, open_list, grid);
        }
    //std::cout << "{ " << open_list.at(i).at(0) << " " << open_list.at(i).at(1) << " " << open_list.at(i).at(2) << " " << open_list.at(i).at(3) << " } ";
    }
    std::cout << std::endl;
    std::cout << open_list.size() << std::endl;

    /*for(int i = 0; i < 4; i++){
        std::cout << "{ " << open_list[i][0] << " " << open_list[i][1] << " " << open_list[i][2] << " " << open_list[i][3] << " } ";
    }*/

}



AI_Snake::State AI_Snake::AStarSearch(){
    
    // Initialize search algorithm
    int x = init[0];
    int y = init[1];
    int g = 0;
    int h = Hueristic(x, y, goal.at(0), goal.at(1));
    AddToOpen(x, y, g, h, open_list, grid);

    // Search loop.
    while(open_list.size()){
        
        UpdateStateGrid();
        CellSort(open_list);
        std::vector<int> current_cell = open_list.back();
        grid[current_cell.at(0)][current_cell.at(1)] = State::kPath;
        SetDirection(current_cell);
        if((current_cell.at(0) == goal.at(0) && current_cell.at(1) == goal.at(1)) || grid.at(goal.at(0)).at(goal.at(1)) != State::kFood){
            return State::kGoal;
            std::cout  << "goal" << std::endl;
        }

        for(size_t x = 0; x < grid.size(); x++){
            for(size_t y = 0; y < grid.size(); y++){
                switch(grid.at(x).at(y)){
                    case State::kEmpty:
                        std::cout << 0 << " ";
                    break;
                    case State::kObstacle:
                        std::cout << 1 << " ";
                    break;
                    case State::kFood:
                        std::cout << 2 << " ";
                    case State::kClosed:
                        std::cout << 3 << " ";
                    break;
                    case State::kPath:
                        std::cout << 4 << " ";

                }
            }
            std::cout << std::endl;
        }
        ExpandToNeighbors(current_cell, goal, open_list, grid);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if(!running){
            break;
        }

    }
    return State::kGoal;
}

void AI_Snake::setGameHandle(Game *game){
    _game = game;
}

AI_Snake::~AI_Snake(){
    std::for_each(_threads.begin(), _threads.end(), [](std::thread &thr){thr.join();});
}



