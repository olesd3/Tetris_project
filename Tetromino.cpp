#include "Tetromino.h"
#include <map>
    
const std::map<TetrominoType, std::vector<std::vector<int>>> initialMatrixMap {

    {
    TetrominoType::J,
        {
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 1}
        }
    },
    {
        TetrominoType::L, 
        {
        {0,0,0},
        {1,1,1},
        {1,0,0}
        }
    },
    {
        TetrominoType::T, 
        {
        {0,0,0},
        {1,1,1},
        {0,1,0}
        }
    },
    {
        TetrominoType::S, 
        {
        {0,0,0},
        {0,1,1},
        {1,1,0}
        }
    },
    {
        TetrominoType::Z, 
        {
        {0,0,0},
        {1,1,0},
        {0,1,1}
        }
    },
    {
        TetrominoType::O, 
        {
        {1,1},
        {1,1}
        }
    },
    {
        TetrominoType::I, 
        {
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0},
        {1,0,0,0}
        }
    },
};



Tetromino::Tetromino(TDT4102::Point startingPoint, TetrominoType tetType) : 
topLeftCorner{startingPoint}
{   
    std::vector<std::vector<int>> initMap = initialMatrixMap.at(tetType);
    matrixSize = initMap.at(0).size();

    for (int i{0}; i < matrixSize; i++) {
        std::vector<TetrominoType> row;
        for (int j{0}; j < matrixSize; j++) {
            if (!initMap.at(i).at(j)) row.emplace_back(TetrominoType::NONE);
            else row.emplace_back(tetType);
        }
        blockMatrix.emplace_back(row);
    }
}
Tetromino::Tetromino() {
    topLeftCorner =  TDT4102::Point(0,0);
    matrixSize = 0;
}

void Tetromino::rotateCounterClockwise() {
    //Første rotasjon, med hensyn på diagonalen
    for(int row = 0; row < matrixSize; row++){
        for(int column = 0; column < row; column++){
            std::swap(blockMatrix[row][column], blockMatrix[column][row]);
        }
    }

    //Andre rotasjon, med hensyn på den midtre raden
    for(int row = 0; row < matrixSize/2; row++){
        for(int column = 0; column < matrixSize; column++){
            std::swap(blockMatrix[row][column], blockMatrix[matrixSize-row-1][column]);
            
        }
    }
}

void Tetromino::rotateClockwise() {
    //Første rotasjon, med hensyn på diagonalen
    for(int row = 0; row < matrixSize; row++){
        for(int column = 0; column < row; column++){
            std::swap(blockMatrix[row][column], blockMatrix[column][row]);
        }
    }
     
    //Andre rotasjon, med hensyn på den midtre kolonnen
    for(int row = 0; row < matrixSize; row++){
        for(int column = 0; column < matrixSize/2; column++){
            std::swap(blockMatrix[row][column], blockMatrix[row][matrixSize-column-1]);
        }
    }
}

void Tetromino::moveDown() {
    topLeftCorner.y += blockSize;
}

void Tetromino::moveLeft() {
    topLeftCorner.x -= blockSize;
}
void Tetromino::moveRight() {
    topLeftCorner.x += blockSize;
}
void Tetromino::moveUp() {
    topLeftCorner.y -= blockSize;
}


bool Tetromino::blockExists(int row, int column) {
    try {
        return blockMatrix.at(row).at(column) != TetrominoType::NONE;
    }
    catch (...) {
        throw "Index out of range\n";
    }
}
TetrominoType Tetromino::getBlock(int row, int column) {
    return blockMatrix.at(row).at(column);
}

int Tetromino::cutLeft() {
    for (int j{0}; j < matrixSize; j++) {
        for (int i{0}; i < matrixSize; i++) {
            if (blockMatrix.at(i).at(j) != TetrominoType::NONE) return j;
        }
    }
    return matrixSize - 1;
}

int Tetromino::cutRight() {
    for (int j{matrixSize - 1}; j >= 0; j--) {
        for (int i{0}; i < matrixSize; i++) {
            if (blockMatrix.at(i).at(j) != TetrominoType::NONE) return matrixSize - 1 - j;
        }
    }
    return 0;
}
int Tetromino::cutDown() {
    for (int i{matrixSize - 1}; i >= 0; i--) {
        for (int j{0}; j < matrixSize; j++) {
            if (blockMatrix.at(i).at(j) != TetrominoType::NONE) return matrixSize - 1 - i;
        }
    }
    return 0;
}