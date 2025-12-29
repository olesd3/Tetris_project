#pragma once
#include <map>
#include "AnimationWindow.h"
#include "Tetromino.h"
#include "widgets/TextInput.h"
#include "widgets/Button.h"
#include <random>

class TetrisWindow : public TDT4102::AnimationWindow {

public:
    TetrisWindow();
    void run();
   
private:
    void handleInput();
    void generateRandomTetromino();
    void drawCurrentTetromino();
    void moveTetroinoDown();
    void fastenTetromino();
    void drawGridMatrix();
    bool crashedDownInBlocks();
    bool blockWallsToTheLeft();
    bool blockWallsToTheRight();
    bool overLaps();
    void adjustCurrent();
    void removeFullRows();
    void checkIfLost();
    bool blockOrNothingUnderTetromino(Tetromino & interest_tetromino);
    void drawDropDownTetromino(Tetromino & interest_tetromino);
    void drawDropDown();


    std::vector<std::vector<TetrominoType>> gridMatrix;
    Tetromino currentTetromino;
    static const std::map<TetrominoType, TDT4102::Color> tetToCol; 
    static constexpr int blockSize {Tetromino::blockSize};
    static constexpr int tile_width = 15;
    static constexpr int tile_height = 20;
    static constexpr TDT4102::Point p = {5,0};
    static constexpr TDT4102::Point startPoint = {p.x * blockSize, p.y * blockSize};
    bool hasLostBool{false};
};