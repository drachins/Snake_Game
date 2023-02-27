#include "ai_snake.h"
#include "game.h"
#include <iostream>
#include <cmath>


// Function that launches AI snake thread by calling run_ai_snake() function
void AI_Snake::launch_ai_snake(){

    // Block of code that sets values for the iniitial positoin, goal, and a state grid from game class instance. 
    init = {static_cast<int>(head_x), static_cast<int>(head_y)};
    std::vector<int> new_goal = FindNearestFood(init.at(0), init.at(1)); 
    goal[0] = new_goal.at(0);
    goal[1] = new_goal.at(1);
    grid = _game->getGrid();

    _ai_threads.emplace_back(&AI_Snake::run_ai_snake, this);
}

// Function that continuosly calls AStarSearch() function until Ai snake is either killed or game is closed.
void AI_Snake::run_ai_snake(){

    // Main while loop that runs and renitilializes A* search alogorithm until snake is either dead or game is closed.

    while(true){

        // Checks if algorithm state is "kRunning" and calls the AStarSearch() class method wich encapsulates the A* search alorithm, and returns a alorithm state value. 
        if(algo_state == State::kRunning)
            algo_state = AStarSearch();

        // Block of code that checks if the alogorithm state is that of goal being reached, and after messgae is recieved from game thread that it can start a new algorithm cycle
        // it reintializes A* search by resetting state to "kRunning" and finds nearest food for new goal, new intial positition is set to current position 
        // of snake head, and a new state grid is extracted from game class instance.
        if(algo_state == State::kGoal){

            State new_state;
            if(_game->_newCycle == State::kOldCycle){
                new_state = _game->WaitforNewCycle();
            }         

            algo_state = State::kRunning;
            init = {static_cast<int>(head_x), static_cast<int>(head_y)};
            std::vector<int> new_goal = FindNearestFood(init.at(0), init.at(1)); 
            goal[0] = new_goal.at(0);
            goal[1] = new_goal.at(1);
            grid = _game->getGrid();
        }


        // Checks if algorithm state is "kKilled" which is set inside and can be returned by the AStarSearch() method which breaks the main loop. 
        if(algo_state == State::kKilled)
            break;

    }
    
}

// Helper function that is used in the std::sort() algorithm funciton to compare the f values of two different A* search cells.
bool Compare(const std::vector<int> vec_a, const std::vector<int> vec_b){
    int f1 = vec_a.at(2) + vec_a.at(3);
    int f2 = vec_b.at(2) + vec_b.at(3);
    return(f1 > f2);
} 

// Helper function that uses std::sort algorithm which in turn uses the Compare function to sort a vector of A* search grid cells from the highest to lowest f values. 
void AI_Snake::CellSort(std::vector<std::vector<int>> &vec){
    std::sort(vec.begin(), vec.end(), Compare);
}

// Helper function that returns the hueristic of cell in the A* search grid, 
int AI_Snake::Hueristic(int x1, int y1, int x2, int y2){
    return(abs(x2 - x1) + abs(y2 - y1));
}

// Helper Function that checks is a A* search grid cell is on the grid, and is an empty cell or a cell with food.  
bool AI_Snake::CheckValidCell(int x, int y, std::vector<std::vector<AI_Snake::State>> &grid){
    bool on_grid_x  = (x >= 0 && x < grid.size());
    bool on_grid_y = (y >= 0 && y < grid.at(0).size());
    if(on_grid_x && on_grid_y){
        if(grid[x][y] == State::kEmpty || grid[x][y] == State::kFood)
            return true;
    }
    return false;
}

// Helper funciton that adds candidate cells to the open_list vector, and sets the cell state to closed.
void AI_Snake::AddToOpen(int x, int y, int g, int h, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){
    open_list.push_back(std::vector<int>{x, y, g, h});
    grid.at(x).at(y) = State::kClosed;
}

// Function thar finds the nearest food to snake head. 
std::vector<int> AI_Snake::FindNearestFood(int x, int y){

    // Get coodinates of food items.
    std::vector<std::vector<int>> food = _game->getFoodCoords();
    // Sort food item coords from lowest to highest hueristic 
    std::sort(food.begin(), food.end(), [x,y](std::vector<int> a, std::vector<int> b){return(abs(a.at(0) - x) + abs(a.at(1) - y)) < (abs(b.at(0) - x) + abs(b.at(1) - y));});

    // Return food item with lowest hueristic.
    return food.front();

}

// Function that extracts new state grid from game class instance, and refreshes state grid member vairable with current state data. 
void AI_Snake::UpdateStateGrid(){
    std::vector<std::vector<AI_Snake::State>> grid_frame = _game->getGrid();

    // Nested for loops that loop through each each row of state grid by coloumn. 
    for(size_t x  = 0; x < grid_frame.size(); x++){
        for(size_t y = 0; y < grid_frame.at(x).size(); y++){
            if(grid.at(x).at(y) != State::kClosed){
                // Switch block that overwrites old state data with new data if necessary, while preserving closed states. 
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

// Function that finds directionial relation of current cell to previous cell, then sets the direction member variable.
void AI_Snake::SetDirection(int current_cell[], int previous_cell[]){

    // Directional relation is found through an equality by adding value of previous cell to the corresponding delta value and grid height or grid width depending on
    // which direction is being checked, and the remainder is computing which allows for algorithm to wrap around the edge of the grid. 
    if(current_cell[0] == static_cast<int>(fmod(previous_cell[0] + delta[0][0] + grid_width, grid_width))){
        direction = Direction::kRight;
    }
    else if(current_cell[0] == static_cast<int>(fmod(previous_cell[0] + delta[2][0] + grid_width, grid_width))){
        direction = Direction::kLeft;
    }
    else if(current_cell[1] == static_cast<int>(fmod(previous_cell[1] + delta[3][1] + grid_height, grid_height))){
        direction = Direction::kUp;
    }
    else if(current_cell[1] == static_cast<int>(fmod(previous_cell[1] + delta[1][1] + grid_height, grid_height))){
        direction = Direction::kDown;
    }
    
}

// Function that expands the A* search alorithm to new unexplored cells. 
void AI_Snake::ExpandToNeighbors(int current[], int goal[], std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid){

    // Get current node's date.
    int x = current[0];
    int y = current[1];
    int g = current[2];  

    // Loop through current node's potential neighbors.
    for(int i = 0; i < 4; i++){
        int x2 = static_cast<int>(fmod(x + delta[i][0] + grid_width, grid_width));
        int y2 = static_cast<int>(fmod(y + delta[i][1] + grid_height, grid_height));

        //Check if potential neighbor is a valid cell.
        if(CheckValidCell(x2, y2, grid)){
            // Calculate g and h values of grid cells, then adds cell to open_list vector. 
            int g2 = g + 1;
            int h2 = Hueristic(x2, y2, goal[0], goal[1]);
            AddToOpen(x2, y2, g2, h2, open_list, grid);
            
        }

    }

}


// Function that encapsultes the A* search algorithm. 
AI_Snake::State AI_Snake::AStarSearch(){
    
    // Initialization of A* search algorithm
    AddToOpen(init[0], init[1], 0, Hueristic(init[0], init[1], goal[0], goal[1]), open_list, grid);
    int current_cell[4] = {0};
    for(size_t n = 0; n < init.size(); n++){
        current_cell[n] = init.at(n);
    }
    int previous_cell[4] = {0};
    int cycle = 0;
    State ASearch_State = State::kRunning;

    // Main Search loop.
    while(true){

        // Polls state of the game whether it is running, and whether the snake is still alive. 
        if(!running || !alive){
            ASearch_State = State::kKilled;
            break;
        }

        // Checks if if the head coordinates of snake head matches that of the A* sarch alorithm's current cell. 
        if(current_cell[0] == static_cast<int>(head_x) && current_cell[1] == static_cast<int>(head_y)){

            // Refreshes grid member variable with new data. 
            UpdateStateGrid();

            // Sorts current cell from highest f value to lowest, then sets current_cell array to cell with lowest f value, SetDirection() fucntion is called wich sets 
            // member variable "direction" then open_list is cleared. 
            CellSort(open_list);
            for(size_t i = 0; i < open_list.back().size(); i++){
                current_cell[i] = open_list.back().at(i);
            }
            if(cycle > 0){
                SetDirection(current_cell, previous_cell);
            }
            open_list.clear();

            // Checks if A* search alogorithm has reached the goal or if the goal still has a food state.
            if((current_cell[0] == goal[0] && current_cell[0] == goal[1]) || grid.at(goal[0]).at(goal[1]) != State::kFood){
                // While loop that calls Update() snake member funciton until coordinates of snake heads matches that of current cell. 
                while(!(current_cell[0] == static_cast<int>(head_x) && current_cell[1] == static_cast<int>(head_y))){
                    Update();
                }
                // Sets ASearch_State variable to kGoal and breaks while loop. 
                ASearch_State = State::kGoal;
                break;
            }

            // Function call of ExpandToNeighbors().
            ExpandToNeighbors(current_cell, goal, open_list, grid);

            std::this_thread::sleep_for(std::chrono::milliseconds(190));

            // Sets previous_cell to current cell. 
            for(size_t n = 0; n < sizeof(current_cell)/sizeof(int); n++){
                previous_cell[n] = current_cell[n];
            }
            cycle++;           
        }
        else{
            // Function call of Update() method until snake head coordinates match value of current cell. 
            Update();
        }       


    }
    return ASearch_State;
}
   
// Function that sets handle of _game member variable to game class instance that is created when game is launched. 
void AI_Snake::setGameHandle(Game *game){
    _game = game;
}

// Destructor that joins AI Snake thread. 
AI_Snake::~AI_Snake(){
    std::for_each(_ai_threads.begin(), _ai_threads.end(), [](std::thread &thr){thr.join();});
}



