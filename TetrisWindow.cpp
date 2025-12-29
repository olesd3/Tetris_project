#include "TetrisWindow.h"
#include <iostream>

TetrisWindow::TetrisWindow() :
AnimationWindow(560,50, tile_width * blockSize, tile_height * blockSize, "Tetris") {
    for (int i{0}; i < tile_height; i++) {
        std::vector<TetrominoType> row;
        for (int j{0}; j < tile_width; j++) {
            row.push_back(TetrominoType::NONE);
        }
        gridMatrix.push_back(row);
    }
    generateRandomTetromino();
}; 

void TetrisWindow::run() {
    unsigned int framesSinceLastTetronimoMove = 0;
    const unsigned int framesPerTetronimoMove = 40;

    while(!should_close()) { 
        if (!hasLostBool) {
            framesSinceLastTetronimoMove++;
            if(framesSinceLastTetronimoMove >= framesPerTetronimoMove) {
                framesSinceLastTetronimoMove = 0;
                if (crashedDownInBlocks()) fastenTetromino();
                else moveTetroinoDown();
            }
            handleInput();
            drawCurrentTetromino();
            drawGridMatrix();
        }
        else {
            drawCurrentTetromino();
            drawGridMatrix();
            draw_rectangle({tile_width*blockSize / 2  - 60, tile_height*blockSize / 2  - 20}, 75, 50, TDT4102::Color::white);
            draw_text({tile_width*blockSize / 2  - 60, tile_height*blockSize / 2  - 20}, "Lost", TDT4102::Color::black, 40U);
        }

        next_frame();
    }
}

void TetrisWindow::adjustCurrent() {
    TDT4102::Point current_pos = currentTetromino.getPosition();
    int matrixSize = currentTetromino.getMatrixSize();
    
    while (current_pos.x < 0) {
            currentTetromino.moveRight();
            current_pos = currentTetromino.getPosition();
    }
    while (current_pos.x > blockSize * (tile_width - matrixSize)) {
        currentTetromino.moveLeft();
        current_pos = currentTetromino.getPosition();
    }
    while (current_pos.y > blockSize * (tile_height - matrixSize)) {
        currentTetromino.moveUp();
        current_pos = currentTetromino.getPosition();
    }
}


void TetrisWindow::handleInput() {
    TDT4102::Point current_pos = currentTetromino.getPosition();
    int matrixSize = currentTetromino.getMatrixSize();
    int cut_left = currentTetromino.cutLeft();
    int cut_right = currentTetromino.cutRight();
    int cut_down = currentTetromino.cutDown();

    static bool lastLEFTKeyState = false;
    static bool lastRIGHTKeyState = false;
    static bool lastSKeyState = false;
    static bool lastAKeyState = false;
    static bool lastDKeyState = false;
    static bool lastXKeyState = false;
    //static bool lastFKeyState = false;

    bool currentLEFTKeyState = is_key_down(KeyboardKey::LEFT);
    bool currentRIGHTKeyState = is_key_down(KeyboardKey::RIGHT);
    bool currentSKeyState = is_key_down(KeyboardKey::S);
    bool currentAKeyState = is_key_down(KeyboardKey::A);
    bool currentDKeyState = is_key_down(KeyboardKey::D);
    bool currentXKeyState = is_key_down(KeyboardKey::X);
    //bool currentFKeyState = is_key_down(KeyboardKey::F);

    
    if(currentLEFTKeyState && !lastLEFTKeyState) {
        //If left key is pressed: rotate clockwise
        currentTetromino.rotateClockwise();

        //If, after rotating, the resulting currentTetromino happens to overlap with the existing grid: rotate back.
        if (overLaps()) currentTetromino.rotateCounterClockwise();
        
        //If not, adjust if necessary, (wallkick).
        else adjustCurrent();
    }

    if(currentRIGHTKeyState && !lastRIGHTKeyState) {
        //If right key is pressed: rotate counter clockwise
        currentTetromino.rotateCounterClockwise();
        if (overLaps()) currentTetromino.rotateClockwise();
        else adjustCurrent();
    }


    bool InBoundsdown = (current_pos.y - cut_down * blockSize < blockSize * (tile_height - matrixSize));
    bool InBoundsLeft = (current_pos.x + cut_left * blockSize > 0);
    bool InBoundsRight = (current_pos.x - cut_right * blockSize < blockSize * (tile_width - matrixSize));
    if (!lastSKeyState && currentSKeyState && InBoundsdown && !crashedDownInBlocks()) {
        currentTetromino.moveDown();
    }
    if (!lastAKeyState && currentAKeyState && InBoundsLeft && !blockWallsToTheLeft()) {
        currentTetromino.moveLeft();
    }
    if (!lastDKeyState && currentDKeyState && InBoundsRight && !blockWallsToTheRight()) {
        currentTetromino.moveRight();
    }
    if (!lastXKeyState && currentXKeyState) {
        while (!blockOrNothingUnderTetromino(currentTetromino)) {
            currentTetromino.moveDown();
        }
        fastenTetromino();
    }
    /*
    if (!lastFKeyState && currentFKeyState) {
        fastenTetromino();
    }
    */



    lastLEFTKeyState = currentLEFTKeyState;
    lastRIGHTKeyState = currentRIGHTKeyState;

    lastSKeyState = currentSKeyState;
    lastAKeyState = currentAKeyState;
    lastDKeyState = currentDKeyState;
    lastXKeyState = currentXKeyState;
    //lastFKeyState = currentFKeyState;

}

const std::map<TetrominoType, TDT4102::Color> TetrisWindow::tetToCol {
    {TetrominoType::J, TDT4102::Color::blue},
    {TetrominoType::L, TDT4102::Color::orange},
    {TetrominoType::T, TDT4102::Color::purple},
    {TetrominoType::S, TDT4102::Color::lime},
    {TetrominoType::Z, TDT4102::Color::red},
    {TetrominoType::O, TDT4102::Color::yellow},
    {TetrominoType::I, TDT4102::Color::light_blue},
};

void TetrisWindow::generateRandomTetromino() {
    static int round = 0;
    switch (round) {
        /*
        case 0:
            currentTetromino = Tetromino(startPoint, (TetrominoType)1);
            round++;
            break;
        case 1:
            currentTetromino = Tetromino(startPoint, (TetrominoType)5);
            round++;
            break;
        */
        default:
            std::random_device device;
            std::default_random_engine gen(device());
            std::uniform_int_distribution<int> dist(0,6);
            int random_num = dist(gen);
            currentTetromino = Tetromino(startPoint, (TetrominoType)random_num);

    }
    checkIfLost();
    //For testing a specific tetromino
    //currentTetromino = Tetromino(startPoint, (TetrominoType)2);
    
}



void TetrisWindow::drawCurrentTetromino() {
    drawDropDown();
    for (int i{0}; i < currentTetromino.getMatrixSize(); i++) {
        for (int j{0}; j < currentTetromino.getMatrixSize(); j++) {
            TetrominoType typ = currentTetromino.getBlock(i,j);
            if (typ != TetrominoType::NONE) {
                TDT4102::Point current_top_left = currentTetromino.getPosition();
                TDT4102::Point p = {current_top_left.x + blockSize * j, current_top_left.y + blockSize * i};
                draw_rectangle(p, blockSize, blockSize, 
                tetToCol.at(typ), TDT4102::Color::black);

            }
            
        }
    }
}

void TetrisWindow::moveTetroinoDown() {
    TDT4102::Point current_pos = currentTetromino.getPosition();
    int matrixSize = currentTetromino.getMatrixSize(), cut_down = currentTetromino.cutDown();
    if (current_pos.y - blockSize * cut_down < blockSize * (tile_height - matrixSize)) currentTetromino.moveDown();
    else fastenTetromino();
}

void TetrisWindow::fastenTetromino() {
    for (int i{0}; i < currentTetromino.getMatrixSize(); i++) {
        for (int j{0}; j < currentTetromino.getMatrixSize(); j++) {
            TetrominoType typ = currentTetromino.getBlock(i,j);
            TDT4102::Point pos = currentTetromino.getPosition();
            if (typ != TetrominoType::NONE) {
                gridMatrix.at(pos.y / blockSize + i).at(pos.x / blockSize + j) = typ;
            }
            
        }
    }
    removeFullRows();
    generateRandomTetromino();
}

void TetrisWindow::drawGridMatrix() {
    for (int i{0}; i < tile_height; i++) {
        for (int j{0}; j < tile_width; j++) {
            TetrominoType typ = gridMatrix.at(i).at(j);
            if (typ != TetrominoType::NONE) {
                TDT4102::Point p = {blockSize * j, blockSize * i};
                draw_rectangle(p, blockSize, blockSize, tetToCol.at(typ), TDT4102::Color::black);
            }
        }
    }
}
bool TetrisWindow::crashedDownInBlocks() {
    //Check if the current tetromino has crashed down into other blocks
    int matrixSize = currentTetromino.getMatrixSize();
    TDT4102::Point pos = currentTetromino.getPosition();

    //Iterating throuh all the TetrominoTypes in currentTetromino
    for (int i{0}; i < matrixSize; i++) {
        for (int j{0}; j < matrixSize; j++) {
            //For each TetrominoType: check if there is a TetrominoType beneath different from NONE.
            //If so: the current tetromino has crashed in a block beneath.
            TetrominoType typ = currentTetromino.getBlock(i,j);
            TDT4102::Point interest_pos = TDT4102::Point(j + pos.x / blockSize, i + pos.y / blockSize);
            if ((typ != TetrominoType::NONE) && 
                (interest_pos.y < tile_height - 1) &&
                (gridMatrix.at(interest_pos.y + 1).at(interest_pos.x) != TetrominoType::NONE)) {
                    return true;
            }
        }
    }
    return false;
}


bool TetrisWindow::blockWallsToTheLeft() {
    //Check if the current tetromino has blocks to the left
    int matrixSize = currentTetromino.getMatrixSize();
    TDT4102::Point pos = currentTetromino.getPosition();
    int cut_left = currentTetromino.cutLeft();

    //Iterating throuh all the TetrominoTypes in currentTetromino
    for (int i{0}; i < matrixSize; i++) {
        for (int j{0}; j < matrixSize; j++) {
            //For each TetrominoType: check if there is a TetrominoType to the left different from NONE.
            //If so: the current tetromino can't move anymore to the left.
            TetrominoType typ = currentTetromino.getBlock(i,j);
            TDT4102::Point interest_pos = TDT4102::Point(j + pos.x / blockSize, i + pos.y / blockSize);
            if ((typ != TetrominoType::NONE) && 
                (interest_pos.x > 0) &&
                (gridMatrix.at(interest_pos.y).at(interest_pos.x - 1) != TetrominoType::NONE)) {
                    return true;
            }
        }
    }
    return false;
}

bool TetrisWindow::blockWallsToTheRight() {
    //Check if the current tetromino has blocks to the right
    int matrixSize = currentTetromino.getMatrixSize();
    TDT4102::Point pos = currentTetromino.getPosition();
    int cut_right = currentTetromino.cutRight();

    //Iterating throuh all the TetrominoTypes in currentTetromino
    for (int i{0}; i < matrixSize; i++) {
        for (int j{0}; j < matrixSize; j++) {
            //For each TetrominoType: check if there is a TetrominoType to the right different from NONE.
            //If so: the current tetromino can't move anymore to the right.
            TetrominoType typ = currentTetromino.getBlock(i,j);
            TDT4102::Point interest_pos = TDT4102::Point(j + pos.x / blockSize, i + pos.y / blockSize);
            if ((typ != TetrominoType::NONE) && 
                (interest_pos.x < tile_width - 1) &&
                (gridMatrix.at(interest_pos.y).at(interest_pos.x + 1) != TetrominoType::NONE)) {
                    return true;
            }
        }
    }
    return false;
}

bool TetrisWindow::overLaps() {
    int matrixSize = currentTetromino.getMatrixSize();
    TDT4102::Point pos = currentTetromino.getPosition();

    //Iterating throuh all the TetrominoTypes in currentTetromino
    for (int i{0}; i < matrixSize; i++) {
        for (int j{0}; j < matrixSize; j++) {
            //For each TetrominoType: check if there there already is a tetrominotype at (i,j).
            //If so: there is a overlap and we are to return true.
            TetrominoType typ = currentTetromino.getBlock(i,j);
            TDT4102::Point interest_pos = TDT4102::Point(j + pos.x / blockSize, i + pos.y / blockSize);
            if ((typ != TetrominoType::NONE)   && 
                (interest_pos.x < tile_width)  &&
                (interest_pos.x >= 0)          &&
                (interest_pos.y < tile_height) &&
                (interest_pos.y >= 0)          &&
                (gridMatrix.at(interest_pos.y).at(interest_pos.x) != TetrominoType::NONE)) {
                    return true;
            }
        }
    }
    return false;
}

void TetrisWindow::removeFullRows() {
    //Find all full rows
    std::vector<int> rows_to_be_removed;
    for (int i{0}; i < tile_height; i++) {
        bool row_is_full = true;
        for (int j{0}; j < tile_width; j++) {
            if (gridMatrix.at(i).at(j) == TetrominoType::NONE) {
                row_is_full = false;
                continue;
            }
        }
        if (row_is_full) rows_to_be_removed.push_back(i);
    }

    //Remove found full rows
    std::vector<TetrominoType> row(tile_width, TetrominoType::NONE);
    for (int indexRemove : rows_to_be_removed) {
        gridMatrix.erase(gridMatrix.begin() + indexRemove);
        gridMatrix.insert(gridMatrix.begin(), row);
    }

}

void TetrisWindow::checkIfLost() {
    TDT4102::Point current_point = currentTetromino.getPosition();
    if ((current_point.x == startPoint.x) && (current_point.y == startPoint.y) && overLaps()) {
        hasLostBool = true;
    }
}

// Functions for drawing the drop by x tetromino
// ----- Section1 Start -----

bool TetrisWindow::blockOrNothingUnderTetromino(Tetromino & interest_tetromino) {
    //Check if the current tetromino has crashed down into other blocks
    int matrixSize = interest_tetromino.getMatrixSize();
    TDT4102::Point pos = interest_tetromino.getPosition();

    //Iterating throuh all the TetrominoTypes in interest_tetromino
    for (int i{0}; i < matrixSize; i++) {
        for (int j{0}; j < matrixSize; j++) {
            //For each TetrominoType: check if there is a TetrominoType beneath different from NONE.
            //If so: the current tetromino has crashed in a block beneath.
            TetrominoType typ = interest_tetromino.getBlock(i,j);
            TDT4102::Point interest_pos = TDT4102::Point(j + pos.x / blockSize, i + pos.y / blockSize);
            if ((typ != TetrominoType::NONE) && 
                ((interest_pos.y >= tile_height - 1) ||
                (gridMatrix.at(interest_pos.y + 1).at(interest_pos.x) != TetrominoType::NONE))) {
                    return true;
            }
        }
    }
    return false;
}

void TetrisWindow::drawDropDownTetromino(Tetromino & interest_tetromino) {
    for (int i{0}; i < interest_tetromino.getMatrixSize(); i++) {
        for (int j{0}; j < interest_tetromino.getMatrixSize(); j++) {
            TetrominoType typ = interest_tetromino.getBlock(i,j);
            if (typ != TetrominoType::NONE) {
                TDT4102::Point current_top_left = interest_tetromino.getPosition();
                TDT4102::Point p = {current_top_left.x + blockSize * j, current_top_left.y + blockSize * i};
                draw_rectangle(p, blockSize, blockSize, 
                TDT4102::Color::white, TDT4102::Color::red);
            }
            
        }
    }
   
}

void TetrisWindow::drawDropDown() {
    Tetromino interest_tetromino = currentTetromino;
    bool have_moved = false;
    while (!blockOrNothingUnderTetromino(interest_tetromino)) {
        interest_tetromino.getPositionByRef().y += blockSize;
        have_moved = true;
    }
    if (have_moved) drawDropDownTetromino(interest_tetromino);
}

// ----- Section1 End -----