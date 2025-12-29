#pragma once
#include "Point.h"
#include <vector>
#include "AnimationWindow.h"
#include <iostream>


enum class TetrominoType{J, L, T, S, Z, O, I, NONE};


class Tetromino {
public:
    static constexpr int blockSize {30};

    Tetromino(TDT4102::Point startingPoint, TetrominoType tetType);
    Tetromino();
    
    void rotateClockwise();
    void rotateCounterClockwise();
    void moveDown();
    void moveLeft();
    void moveRight();
    void moveUp();
    bool blockExists(int row, int column);
    TetrominoType getBlock(int row, int column);
    int getMatrixSize() {return matrixSize;}
    TDT4102::Point getPosition() {return topLeftCorner;}
    TDT4102::Point & getPositionByRef() {return topLeftCorner;}
    int cutLeft();
    int cutRight();
    int cutDown();
private: 
    int matrixSize;
    TDT4102::Point topLeftCorner;
    std::vector<std::vector<TetrominoType>> blockMatrix;
};  