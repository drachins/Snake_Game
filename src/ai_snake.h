#ifndef AI_SNAKE
#define AI_SNAKE

#include "snake.h"

class Game;


class AI_Snake : public Snake {

    using Snake::Snake;

    public:
        enum class State {kEmpty, kObstacle, kFood, kGoal, kClosed, kRunning};

        void launch_ai_snake();
        void setGameHandle(Game *game);
        ~AI_Snake();
      

    private:

        size_t grid_width;
        size_t grid_height;

        std::vector<std::vector<AI_Snake::State>> grid;
        std::vector<std::vector<int>> open_list;
        std::vector<int> current_cell;
        std::vector<int> init;
        std::vector<int> goal;
        std::vector<std::thread> _threads;

        Game *_game;

        const int delta[4][2]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

        void run_ai_snake();
        void CellSort(std::vector<std::vector<int>> &vec);
        int  Hueristic(int x1, int x2, int y1, int y2);
        bool CheckValidCell(int x, int y, std::vector<std::vector<AI_Snake::State>> &grid);
        void AddToOpen(int x, int y, int g, int h, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid);
        void ExpandToNeighbors(const std::vector<int> &current, std::vector<int> &goal, std::vector<std::vector<int>> &open_list, std::vector<std::vector<AI_Snake::State>> &grid);
        std::vector<int> FindNearestFood(int x, int y);
        void UpdateStateGrid();
        void SetDirectionAndCurrentCell();
        AI_Snake::State AStarSearch();
    



};


#endif