#include <windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include <cstdlib>
#include <vector>

using namespace std;

struct Pad {
    int x1 = 40, x2 = 60, y = 22;
};

struct Screen {
    int x = 100;
    int y = 25;
};

struct Ball {
    int x = 50, y = 10;
    int xMove = 1, yMove = 1;
};

struct Brick {
    int x1, x2, y;
    bool isDestroyed = false;
};

int score = 0;
int bricksDestroyed = 0;
int health=3;
vector<Brick> bricks;

void initBricks() {
    int brickWidth = 16;
    int brickHeight = 3;
    int rows = 3;
    int cols = (Screen().x - 2) / brickWidth;

    int startX = (Screen().x - cols * brickWidth) / 2;
    int startY = 3;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Brick brick;
            brick.x1 = startX + col * brickWidth;
            brick.x2 = brick.x1 + brickWidth - 1;
            brick.y = startY + row * brickHeight;
            bricks.push_back(brick); //copy the brick and add the end of the brick. With same length and height
        }
    }
}

void drawPad(int padx1, int padx2, int pady, int screeny, int screenx, int ballx, int bally) {
    for (int i = 0; i < screeny; i++) {
        for (int j = 0; j < screenx; j++) {
            if ((i == 0) || (i == screeny - 1) || (j == 0) || (j == screenx - 1)) {
                cout << "#";
            } else if (j >= padx1 && j <= padx2 && i == pady) {
                cout << "=";
            } else if (i == bally && j == ballx) {
                cout << "O";
            } else {
                bool isBrick = false;
                for (auto brick : bricks) {
                    if (!brick.isDestroyed && j >= brick.x1 && j <= brick.x2 && i == brick.y) {
                        cout << "*";
                        isBrick = true;
                        break;
                    }
                }
                if (!isBrick) {
                    cout << " ";
                }
            }
        }
        cout << "\n";
    }
    cout << "\nScore: " << score << "\t Bricks Destroyed: " << bricksDestroyed<< "\t Health: " << health;
}

void MoveBall(Ball* ball, Screen* screen, Pad* pad) {
    if (ball->x == screen->x - 1 || ball->x == (screen->x - screen->x + 1)) {
        ball->xMove *= -1;
    }
    if (ball->y == screen->y - 1) {
            if(health>0){
                health--;
                Ball newBall;
                ball->x = newBall.x;
                ball->y = newBall.y;
            }

    } else if (ball->y == (screen->y - screen->y + 1)) {
        ball->yMove *= -1;
    } else if (ball->y == pad->y - 1 && (ball->x >= pad->x1 && ball->x <= pad->x2)) {
        ball->yMove *= -1;
        int padCenter =(pad->x1 + pad->x2) / 2;
        int ballPositionOnPad = ball->x - pad->x1;
        int ballDirectionOnPad = ballPositionOnPad > padCenter ? 1 : -1;
        ball->xMove = ballDirectionOnPad;
    }
    ball->x += ball->xMove;
    ball->y += ball->yMove;
}
void movePad(Pad* pad) {
    if (_kbhit()) {
        int key = _getch();
        if (key == 'a' && pad->x1 > 1) {
            pad->x1--;
            pad->x2--;
        } else if (key == 'd' && pad->x2 < Screen().x - 2) {
            pad->x1++;
            pad->x2++;
        }
    }
}
bool isColliding(Ball* ball, Brick* brick) {
    int ballTop = ball->y - 1;
    int ballBottom = ball->y + 1;
    int ballLeft = ball->x - 1;
    int ballRight = ball->x + 1;
    int brickTop = brick->y;
    int brickBottom = brick->y + 2;
    int brickLeft = brick->x1;
    int brickRight = brick->x2;

    if (ballBottom < brickTop || ballTop > brickBottom || ballRight < brickLeft || ballLeft > brickRight) {
        return false;
    }
    return true;
}

void checkCollisions(Ball* ball, vector<Brick>& bricks) {
    for (auto& brick : bricks) {
        if (!brick.isDestroyed && isColliding(ball, &brick)) {
            brick.isDestroyed = true;
            ball->yMove *= -1;
            score += 10;
            bricksDestroyed++;
            return;
        }
    }
}


int main() {
Screen screen;
Pad pad;
Ball ball;
initBricks();
while (true) {
        if(health==0){
            system("cls");
            Sleep(100);
            for(int i=0;i<screen.y;i++){
                    cout<<" ";
                for(int j=0;j<screen.x;j++){
                    if(i==screen.y/2&&j==screen.x/2-10){
                        cout<<"Game Over";
                        Sleep(2000);
                        break;
                    }
                    cout<<" ";
                }
            }
            break;

        }
    system("cls");
    movePad(&pad);
    MoveBall(&ball, &screen, &pad);
    checkCollisions(&ball, bricks);
    drawPad(pad.x1, pad.x2, pad.y, screen.y, screen.x, ball.x, ball.y);
    Sleep(10);
}

return 0;

}
