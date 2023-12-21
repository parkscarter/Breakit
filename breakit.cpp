#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <ncurses.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

int gameWidth = 81;
int gameHeight = 40;

class Block{
public: 
    int hp;
    int x;
    int y;
    int width;
    int height;

    Block(int ihp, int iy, int ix, int iwidth, int iheight)
        :   hp(ihp), y(iy), x(ix), width(iwidth), height(iheight){
    }

    int draw(){
        int i;
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);   //Color pair 1; Red on Black

        attron(COLOR_PAIR(1));      // Turn on color pair
        for (int i = x; i <= x + width - 1; ++i) {
            mvaddch(y, i, ACS_HLINE);   // Top horizontal line
            mvaddch(y + height - 1, i, ACS_HLINE);  // Bottom horizontal line
        }

        for (int i = y; i <= height + y - 1; ++i) {
            mvaddch(i, x, ACS_VLINE);   // Left vertical line
            mvaddch(i, width + x - 1, ACS_VLINE);  // Right vertical line
        }

        mvaddch(y, x, ACS_ULCORNER);    // Upper-left corner
        mvaddch(y, x + width - 1, ACS_URCORNER);   // Upper-right corner
        mvaddch(y + height - 1, x, ACS_LLCORNER);   // Lower-left corner
        mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);   //Lower-right corner

        attroff(COLOR_PAIR(1));     // Turn off color pair

        mvprintw(y + height / 2, x + (width - 1) / 2, "%d", hp);
        
        return 0;
    }

    int clear() const {
        int i, j;
        for (i = y; i < y + height; ++i) {
            for (j = x; j < x + width; ++j) {
                mvaddch(i, j, ' ');
            }
        }
        return 0;
    }
};

class Ball{
public:
    double x;
    double y;
    double dir;
    int vel;

    Ball(int velocity)
        :  vel(velocity){
        int randomAngle = rand() % 360;
        x = gameWidth / 2;
        y = gameHeight - 2;
        randomAngle = rand() % 61 + 60;     //Random number between 60 and 120
        dir = randomAngle * M_PI / 180.0;
    }
};

class Paddle{
public:
    int length;
    int loc;
    int y;

    Paddle(int initialLength, int initialLoc, int initialY) 
        : length(initialLength), loc(initialLoc), y(initialY) {
    }

    int moveRight(){
        if (loc + 1 < gameWidth - length){
            loc += 2;
            mvaddch(y, loc - 1, ' ');
            mvaddch(y, loc - 2, ' ');
        }
        return 0;
    }

    int moveLeft(){
        if (loc - 1 > 1){
            loc -= 2;
            mvaddch(y, loc + length, ' ');
            mvaddch(y, loc + length + 1, ' ');
        }
        return 0;
    }
    
    int draw(){
        int i;
        for (i = 0; i < length; i ++){
            mvaddch(y, loc + i, '_');
        }
    return 0;
    }
};

int checkCollision(Ball* ball, std::vector<Block*>& blocks){

    auto it = blocks.begin();

    while (it != blocks.end()) {
        Block* block = *it;

        if (ball->x >= block->x && ball->x <= (block->x + block->width) &&
            ball->y >= block->y && ball->y <= (block->y + block->height)) {
            block->hp--;

            if (abs(ball->x - block->x) > 1 && abs(ball->x - (block->x + block->width)) > 1){
                ball->dir = -ball->dir;
            }
            else{
                ball->dir = M_PI - ball->dir;
            }


            //if block hp = 0, delete it from blocks array
            if (block->hp <= 0) {
                block->clear();
                it = blocks.erase(it);
                delete block;
            } 
            else {
                ++it; // Move to the next element in the vector
            }

            return 0; // Assuming only one collision should be handled per frame
        } else {
            ++it; // Move to the next element in the vector
        }
    }

    return 0;
}

/*
This function uses the ball's location and direction to move the ball, then checks some cases;
    (If the ball hits a paddle, goal, or edge)
*/
int moveBall(Ball* ball, Paddle* p, std::vector<Block*>& blocks){
    int randomAngle;
    double radians, bFactor;
    mvaddch(ball->y,ball->x, ' ');
    ball->x += (cos(ball->dir));
    ball->y -= (sin(ball->dir));

    if (ball->y > gameHeight - 2){
        //If the ball hits the paddle
        if (ball->x - p->loc < p->length && ball->x - p->loc >= 0){
            bFactor = (0.8 - (0.6 * (ball->x - p->loc)/p->length));       //close to .8 means left side of the paddle, close to .2 means right side
            ball->y = gameHeight - 2;
            ball->dir = bFactor * M_PI;             //Multiply bFactor by pi to get exit angle in radians
        }
    }
    if (ball->x < 1){   
        ball->x = 1;
        ball->dir = M_PI - ball->dir;
    }
    else if (ball->x > (gameWidth - 1)){
        ball->x = gameWidth - 1;
        ball->dir = M_PI - ball->dir;
    }
    else if (ball->y < 1){
        ball->y = 1;
        ball->dir = -(ball->dir);
    }

    if (ball->y > gameHeight - 1){
        return 1;
    }

    checkCollision(ball, blocks);
    return 0;
}

/*
This function uses ncurses to draw a red box for the game field
*/
int drawBox(int height, int width){
    int i;
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);   //Color pair 1; Red on Black

    attron(COLOR_PAIR(1));      // Turn on color pair
    for (int i = 0; i <= width; ++i) {
        mvaddch(0, i, ACS_HLINE);   // Top horizontal line
        mvaddch(height, i, ACS_HLINE);  // Bottom horizontal line
    }

    for (int i = 0; i <= height; ++i) {
        mvaddch(i, 0, ACS_VLINE);   // Left vertical line
        mvaddch(i, width, ACS_VLINE);  // Right vertical line
    }

    mvaddch(0, 0, ACS_ULCORNER);    // Upper-left corner
    mvaddch(0, width, ACS_URCORNER);   // Upper-right corner
    mvaddch(height, 0, ACS_LLCORNER);   // Lower-left corner
    mvaddch(height, width, ACS_LRCORNER);  // Lower-right corner

    attroff(COLOR_PAIR(1));     // Turn off color pair
    return 0;
}

/*
This function initializes things for the game, gives the user an interface to begin the game
*/
int begin(){
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    srand(time(NULL));
    drawBox(gameHeight, gameWidth);
    mvprintw(1,5, "Welcome to Breakit!");
    mvprintw(2,5, "This totally original game was created as extra c++ practice");
    mvprintw(3,5, "by me, Carter Parks!");
    mvprintw(5,5, "Press any button to continue");
    refresh();
    return 0;
}

int readCSV(const char* filePath, std::vector<Block*>& blocks){
    int height, width;
    std::ifstream inputFile(filePath);
    std::string line;
    std::vector<std::string> lines;
    std::vector<std::vector<int>> matrix;

    while (std::getline(inputFile, line)) {
        std::istringstream ss(line);
        std::string field;
        std::vector<int> row; 
        while(std::getline(ss, field, ',')){
            row.push_back(std::stoi(field));
        }
        matrix.push_back(row);
    }
    height = 3;
    if (matrix.size() > 6){
        height = 2;
    }
    

    for (size_t i = 0; i < matrix.size() && i < 10; ++i){
        width = gameWidth / matrix[i].size();
        for (size_t j = 0; j < matrix[i].size(); ++j){
            if (matrix[i][j] != 0){
                Block* block = new Block(matrix[i][j], (i * height) + 5, j * width + 1, width, height);
                blocks.push_back(block);
            }
        }
    }
    return 0;
}

int printBlockInfo(std::vector<Block*>& blocks){
    for (size_t i = 0; i < blocks.size(); ++i){
        blocks[i]->draw();
    }
    refresh();
    return 0;
}

int switchLevel(Ball* b, Paddle* p){
    int i, randomAngle;
    for (i = p->loc; i < p->loc + p->length; i++){
        mvaddch(p->y, i, ' ');
    }
    p->loc = (gameWidth / 2) - 3;
    p->draw();
    timeout(-1);
    getch();
    b->x = gameWidth / 2;
    b->y = gameHeight - 2;        
    randomAngle = rand() % 61 + 60;     //Random number between 60 and 120
    b->dir = randomAngle * M_PI / 180.0;
    return 0;
}

/*
This function draws the box, paddles, ball, score, and 'Bounce'
*/
int printBoard(Paddle* p, Ball* b, std::vector<Block*>& blocks, int level){
    drawBox(gameHeight,gameWidth);
    printBlockInfo(blocks);
    mvaddch(b->y, b->x, 'o');
    p->draw();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK); // Red on Black)
    attron(COLOR_PAIR(1));
    mvprintw(gameHeight + 1, (gameWidth / 2) - 5, "Breakit");
    mvprintw(gameHeight + 1, 1, "level: %d", level);
    attroff(COLOR_PAIR(1));
    move(gameHeight + 2,80);
    refresh();
    return 0;
}

int main(int argc, char *argv[]){
    int ui, randomAngle, i;
    begin();
    getch();
    clear();

    Paddle paddle = Paddle(16, (gameWidth / 2) - 3, gameHeight - 1);
    Ball ball = Ball(10);

    std::vector<Block*> blocks;
    std::string filename;
    std::vector<std::string> levels;
    for (i = 1; i <= 6; ++i) {
        std::string filename = "./data/lvl" + std::to_string(i) + ".csv";
        levels.push_back(filename);
    }
    i = 1;
    for (const std::string& level : levels){
        readCSV(level.c_str(), blocks);
        drawBox(gameHeight, gameWidth);
        printBlockInfo(blocks);
        timeout(0);
        while(ui != 'q' && blocks.size() > 0){
            //This line effectively speeds up the game as the ball's velocity becomes faster
            usleep(500000 / (&ball)->vel);
            printBoard(&paddle, &ball, blocks, i);
            //Take user input (unbuffered)
            ui = getch();
            switch (ui) {
                case KEY_LEFT:
                    paddle.moveLeft();
                    break;
                case KEY_RIGHT:
                    paddle.moveRight();
                    break;
                default:
                    refresh();
            }
            if (moveBall(&ball, &paddle, blocks) == 1){
                ui = 'q';
            }
        }
        if (ui == 'q'){
            break;
        }
        switchLevel(&ball, &paddle);
        i ++;
    }

    if (ui == 'q'){
        mvprintw(gameHeight / 2,(gameWidth / 2) - 5, "You Lost!");
    }
    else{
        mvprintw(gameHeight / 2,(gameWidth / 2) - 5, "You Won!");
    }

    move(gameHeight,gameWidth);
    refresh();
    usleep(1000000);
    endwin();

    for (Block* block : blocks) {
        delete block;
    }
    blocks.clear();

    return 0;
}