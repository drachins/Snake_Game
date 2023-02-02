#include "ai_snake.h"
#include "game.h"

void AI_Snake::launch_ai_snake(){
    _threads.emplace_back(&AI_Snake::run_ai_snake, this);
}

void AI_Snake::run_ai_snake(){
    AI_Snake::State algo_state = State::kRunning;
    init = {static_cast<int>(head_x), static_cast<int>(head_y)};
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
    return (grid.at(y).at(x) == State::kEmpty);
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
            if(grid.at(x).at(y) != State::kClosed){
                grid.at(x).at(y) = (grid_frame.at(x).at(y) == State::kObstacle) ? State::kObstacle : State::kEmpty;
                grid.at(x).at(y) = (grid_frame.at(x).at(y) == State::kFood) ? State::kFood : State::kEmpty;
            }            
        }
    } 
}

void AI_Snake::SetDirectionAndCurrentCell(){

    CellSort(open_list);
    if(current_cell.at(0) == open_list.back().at(0) + delta[2][0])
        direction = Direction::kRight;
    else if(current_cell.at(0) == open_list.back().at(0) + delta[0][0])
        direction = Direction::kLeft;
    else if(current_cell.at(1) == open_list.back().at(1) + delta[3][1])
        direction = Direction::kUp;
    else if(current_cell.at(1) == open_list.back().at(1) + delta[2][1])
        direction = Direction::kDown;

    current_cell = open_list.back();
    open_list.pop_back();
    
}


void AI_Snake::ExpandToNeighbors(const std::vector<int> &current, std::vector<int> &goal, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){

    // Get current node's date.
    int x = current.at(0);
    int y = current.at(1);
    int g = current.at(2);


    // Loop through current node's potential neighbors.
    for(int i = 0; i < 4; i++){
        int x2 = x + delta[i][0];
        int y2 = x + delta[i][1];


        //Check if potential neighbor is a valid cell.
        if(CheckValidCell(x2, y2, grid)){
            int g2 = g++;
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
    int h = Hueristic(x, y, goal.at(0), goal.at(1));
    AddToOpen(x, y, g, h, open_list, grid);
    grid = _game->getGrid();

    // Search loop.
    while(open_list.size()){
        
        // Update grid with new data
        SetDirectionAndCurrentCell();
        if((current_cell.at(0) == goal.at(0) && current_cell.at(1) == goal.at(1)) || grid.at(x).at(y) != State::kFood){
            return State::kGoal;
        }
        //Set direction of snake and set current cell
        UpdateStateGrid();
        ExpandToNeighbors(current_cell, goal, open_list, grid);
        Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));

    }
    return State::kGoal;
}

void AI_Snake::setGameHandle(Game *game){
    _game = game;
}



