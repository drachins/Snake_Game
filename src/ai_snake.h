#ifndef AI_SNAKE
#define AI_SNAKE

#include "snake.h"

// Forward declaration of Game class
class Game;

// AI_Snake class defintion
class AI_Snake : public Snake {

    // Inhertance from Snake class. 
    using Snake::Snake;

    public:
        // Delclaration of State enums. 
        enum class State {kEmpty, kObstacle, kFood, kBody, kGoal, kClosed, kRunning, kNewCycle, kOldCycle, kKilled};

        // Public method declarations.
        void launch_ai_snake();
        
        // Setter method declarations. 
        void setGameHandle(Game *game);
        void setRunning(bool _running){running = _running;}

        // Class destructor. 
        ~AI_Snake();

        // Public member variable declaratiion 
        bool running{true};
      

    private:

        // Private member variable declarations. 
        std::vector<std::vector<AI_Snake::State>> grid;
        std::vector<std::vector<int>> open_list;
        std::vector<int> init;
        std::vector<std::thread> _ai_threads;
        int goal[2];
        State algo_state{State::kRunning};
        Game *_game;
        const int delta[4][2]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

        // Private member function declarations. 
        void run_ai_snake();
        void CellSort(std::vector<std::vector<int>> &vec);
        int  Hueristic(int x1, int x2, int y1, int y2);
        bool CheckValidCell(int x, int y, std::vector<std::vector<AI_Snake::State>> &grid);
        void AddToOpen(int x, int y, int g, int h, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid);
        void ExpandToNeighbors(int current[], int goal[], std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid);
        std::vector<int> FindNearestFood(int x, int y);
        void UpdateStateGrid();
        void SetDirection(int current_cell[], int previous_cell[]);
        AI_Snake::State AStarSearch();


};


#endif