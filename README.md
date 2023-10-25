# CPPND: Capstone Snake Game

This is my submission for the Capstone project for the Udacity C++ Nanodegree Program. I have taken the Snake Game starter code and I have extended it by adding features such as a 2 player mode, obstacles, and a computer controlled snake to compete with. With the added features I have encorporated C++ programming concepts such as OOP, memory management, and multi-threaded programming. 

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* SDL2 >= 2.0
  * All installation instructions can be found [here](https://wiki.libsdl.org/Installation)
  >Note that for Linux, an `apt` or `apt-get` installation is preferred to building from source. 
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./SnakeGame`.

## File Structure.
    .
    ├── CMakeLists.txt
    ├── CODEOWNERS
    ├── LICENSE.md
    ├── README.md
    ├── cmake
    │   └── FindSDL2_image.cmake
    ├── images
    │   ├── Snake_Game_1_player_mode.gif
    │   ├── Snake_Game_snake_collision.png
    │   ├── Snake_Game_snake_obstacle.png
    │   ├── Snake_Game_user_prompt.png
    │   ├── Snake_game_2_player_mode.gif
    │   ├── Snake_game_final_message.png
    │   └── snake_game.gif
    └── src
        ├── ai_snake.cpp
        ├── ai_snake.h
        ├── controller.cpp
        ├── controller.h
        ├── game.cpp
        ├── game.h
        ├── main.cpp
        ├── obstacle.cpp
        ├── obstacle.h
        ├── renderer.cpp
        ├── renderer.h
        ├── snake.cpp
        └── snake.h


## Class Structure

There are 7 distinct class objects.
- Game class object, which runs the main thread of execution, and main game loop, and spawns the other threads.
- Renderer class object, which renders the game graphics to a window.
- Snake class object, the object that the application user directly controls, each Snake class instance is run in it's own thread.
- AI_Snake class object, this class is a daughter of the Snake class, the AI_Snake class instance is a Snake object that is directly controlled by the computer
  and is run in its own thread
- Controller class object, each Controller instance is paired with a Snake class instance, this class facilitates keyboard input from the user to control the Snake.    
- Obstacle class object, a very simple class, who’s instances act as obstacles on the game map, they are initialized in the main Game instance thread.
- CycleNotify class object, a generic messenger class that primarily serves as a messenger between the Game instance thread and the AI_Snake instance thread. 

## Expected Output.

When executed the program will start by promping the user on terminal to enter a 1 or 2 to chosse between a 1 or 2 player mode.

![](images/Snake_Game_user_prompt.png)

Expected output and gameplay for 1 player mode.

![](images/Snake_Game_1_player_mode.gif)

Expected output and gameplay for 2 player mode.

![](images/Snake_game_2_player_mode.gif)

The object of the game is to avoid obstacles, and other snakes, and collect food, colliding with another snake or an obstacle is instant death, with the head of the snake turning to a red colour. The food items are green coloured blocks, while the obstacles are grey coloured blocks. Collecting a food item will make a snake grow longer and increment its speed. The controls for player 1 is the keyboard arrow keys, while the controls for player two follows the WASD control scheme. The player 1 snake has a blue coloured head, and the player 2 snake has a orange coloured head, while the computer controlled snake has a light blue coloured head. The running score for each player and the computer is displayed on top of the window. The game can be exited by pressing the close button on the top right corner of the window. 

Example of a snakes colliding with an obstacle.

![](images/Snake_Game_snake_obstacle.png)

Example of two snakes colliding.

![](images/Snake_Game_snake_collision.png)

When the game is exited a mesage with the final score and size of each snake is printed to the terminal.

![](images/Snake_game_final_message.png)

## Rubric Points

10 rubric points as stipulated in the CPPND Capstone Project Rubric are addressed by this project.

- The project demonstrates an understanding of C++ functions and control structures.

  All throughout the source code for this project control statements are used, and every class is organized into functions.

- The project accepts input from a user as part of the necessary operations of the program. 

  The program begins with a user prompt asking the user to choose between a 1 player option or a 2 player co-op option, and the modes are selected by entering a 1 or 2 respectively. The player controls a snake using the arrow keys or the WASD keys, and the program terminates with a final message printed to the terminal.

- The program uses Object Oriented Programming techniques.

  The program has 6 separate class objects, each class holding data, and containing methods to perform tasks. 

- Classes use appropriate access specifiers for class members.

  Each class has public member variables, and functions, with the exception of the Obstacle class, which only has a private member variable. The Game, AI_Snake, Controller, and Renderer class each have public, and private member functions and variables. The Snake class has public and protected member functions and variables, as the protected members are inherited by the AI_Snake class.

- Class constructors utilize member initialization lists.

  Every class with the exception of the Obstacle class uses a member initialization list. 

- Templates generalize functions in the project.

  The CycleNotify messenger class have a send and receive public functions that accept generic parameters. 

- The project uses smart pointers instead of raw pointers.

  shared pointers and a unique pointer are used in the Game class. Shared pointers are also used in the Renderer, and Controller classes.

- The project uses multithreading.

  Each Snake, Controller, and AI_Snake object is ran in its own thread of execution. When 1 player mode is selected 3 threads are launched from the Game class instance, when 2 player mode is selected 5 threads are launched. 

- A mutex or lock is used in the project.

  In the receive() public function in the CycleNotify class a unique lock is used, and in the send() public function in the CycleNotify class a lock_gaurd is used, each using a mutex as input.

- A condition variable is used in the project.

  In the recieve() public function in the CycleNotify class, a control variable is used with its inbuilt wait() function to wait for a data to be received in a queue vector. 

## CC Attribution-ShareAlike 4.0 International


Shield: [![CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

This work is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[![CC BY-SA 4.0][cc-by-sa-image]][cc-by-sa]

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: https://licensebuttons.net/l/by-sa/4.0/88x31.png
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg
