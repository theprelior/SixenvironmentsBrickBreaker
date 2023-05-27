#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string>
#include <sstream>
#include <conio.h>
#include <iostream>
using namespace std;
#define ENTER           10
#define ESC             27
#define RACKETLEFT1           'A' //TOP MOVE
#define RACKETLEFT2            'a'
#define RACKETRIGHT1          'D' //BOTTOM MOVE
#define RACKETRIGHT2             'd'
#define RACKETUP1           'W' //TOP MOVE
#define RACKETUP2           'w'
#define RACKETDOWN1          'S' //BOTTOM MOVE
#define RACKETDOWN2           's'
#define MOVE_UP       -1
#define MOVE_DOWN      +1




#define MAX_X           680
#define MAX_Y           480
#define BCKCOLOR        BLUE

#define RACKET_W        60
#define RACKET_H        10
#define RACKET_L        260
#define RACKET_T        400
#define RACKET_R        (RACKET_L+RACKET_W)
#define RACKET_B        (RACKET_T+RACKET_H)
#define RACKET_CLR      GREEN
#define RACKET_XMOVE    10
#define RACKET_YMOVE    10


#define BALL_X          240
#define BALL_Y          240
#define BALL_R          10
#define BALL_CLR        RED
#define BALL_XMOVE      3
#define BALL_YMOVE      3

#define BRICKCOUNT      10
#define HIGLEVELBRICKCOUNT      BRICKCOUNT-1
#define LWBRICKX        30
#define LWBRICKY        150
#define HWBRICKY		100
#define HWBRICKX        50
// typedef enum { false, true } bool;


char score;
char life;
int gameOver=false;
typedef struct {
    int max_x;
    int max_y;
    int bckcolor;
} gamewindow_t;

typedef struct {
    int left;
    int top;
    int right;
    int bottom;
    int color;
    int xmove;
    int ymove;
} racket_t;


typedef struct {
    int left;
    int top;
    int right;
    int bottom;
    int color;
  	bool isDestroyed;
} brick_t;
typedef struct {
    int left;
    int top;
    int right;
    int bottom;
    int color;
    int hitcount;
  	bool isDestroyed;
} brick_t1;




typedef struct {
    int centerx;
    int centery;
    int radius;
    int color;
    int xmove;
    int ymove;
} ball_t;

void initGraphics(gamewindow_t* gw)
{
    int GraphDriver = 0, GraphMode = 0, errorcode;

    initgraph(&GraphDriver, &GraphMode, "");
    errorcode = graphresult();
    if (errorcode != grOk) {  /* an error occurred */
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to halt:");
        getch();
        exit(1);               /* terminate with an error code */
    }
}
void endGame()
{
    closegraph();
}

void initGameWindow(gamewindow_t* gw, int max_x, int max_y, int bckcolor)
{
    gw->max_x = max_x;
    gw->max_y = max_y;
    gw->bckcolor = bckcolor;
}

void drawFilledRectangle(int left, int top, int right, int bottom, int color)
{
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    bar(left, top, right, bottom);
}

void drawFilledCircle(int centerx, int centery, int radius, int color)
{
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    fillellipse(centerx, centery, radius, radius);
}

void setBackGroundColor(gamewindow_t* gw)
{
    drawFilledRectangle(0, 0, gw->max_x, gw->max_y, gw->bckcolor);
}

void initRacket(racket_t* r, int left, int top, int right, int bottom, int color, int xmove, int ymove)
{
    r->left = left;
    r->right = right;
    r->top = top;
    r->bottom = bottom;
    r->color = color;
    r->xmove = xmove;
    r->ymove = ymove;

    drawFilledRectangle(left, top, right, bottom, color);
}

void initBrick(brick_t *brick, int left, int top, int right, int bottom, int color, int isDestroyed) {
    brick->left = left;
    brick->top = top;
    brick->right = right;
    brick->bottom = bottom;
    brick->color = color;
    brick->isDestroyed = isDestroyed;
}
void initBrick2(brick_t1 *brick, int left, int top, int right, int bottom, int color,int hitCount, int isDestroyed) {
    brick->left = left;
    brick->top = top;
    brick->right = right;
    brick->bottom = bottom;
    brick->color = color;
    brick->hitcount=hitCount;
    brick->isDestroyed = isDestroyed;
}




void initBall(ball_t* b, int centerx, int centery, int radius, int color, int xmove, int ymove)
{
    b->centerx = centerx;
    b->centery = centery;
    b->radius = radius;
    b->color = color;
    b->xmove = xmove;
    b->ymove = ymove;

    drawFilledCircle(centerx, centery, radius, color);
}

void initText()
{
	 /*std::string tmp = std::to_string(number);
    char *num_char = tmp.c_str()
	char* str2 = (char*) score2.c_str();
	char* str3= (char*)"Score"+str1+"-"+str2;*/
    setcolor(CYAN);
    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 2);
    outtextxy(300, 30, "Brick Breaker");

    setcolor(WHITE);
    settextstyle(GOTHIC_FONT, HORIZ_DIR, 1);
	
	char arr[50] ;
	sprintf(arr , "Score: %d - Health:%d",score,life);
	setcolor(LIGHTBLUE);
	outtextxy(365, 0, arr);
    
  	
   

}
void gameMenu(){
	setcolor(CYAN);
    settextstyle(TRIPLEX_FONT, HORIZ_DIR, 2);
	char arr[50] ;
	sprintf(arr , "Game Over!");
	setcolor(LIGHTBLUE);
	outtextxy(200, 200, arr);
	
	char arr2[50] ;
	sprintf(arr2 , "EXIT (ESC)");
	setcolor(LIGHTBLUE);
	outtextxy(200, 240, arr2);
	int keyPressed;
	while(1){
		if (kbhit()) {
	        keyPressed = getch();
	        if (keyPressed == ESC && gameOver) {
	            endGame();
	        }
		}
	}

}
void startGame(gamewindow_t* gw, racket_t* r, ball_t* b)
{
    initGameWindow(gw, MAX_X, MAX_Y, BCKCOLOR);
    initGraphics(gw);
    setBackGroundColor(gw);
    
    //gameMenu();
    
    initText();
    
   
	initRacket(r, RACKET_L, RACKET_T, RACKET_R, RACKET_B, RACKET_CLR, RACKET_XMOVE, RACKET_YMOVE);
    initBall(b, BALL_X, BALL_Y, BALL_R, BALL_CLR, BALL_XMOVE, BALL_YMOVE);
    rectangle(10,80,630,410);
    
}



void moveRacket(gamewindow_t* gw, racket_t* r, int dir)
{
    drawFilledRectangle(r->left, r->top, r->right, r->bottom, gw->bckcolor);
    r->right = r->right + (dir * r->xmove);
    r->left = r->left + (dir * r->xmove);
    drawFilledRectangle(r->left, r->top, r->right, r->bottom, r->color);
}

void moveRacket2(gamewindow_t* gw, racket_t* r, int dir)
{
    drawFilledRectangle(r->left, r->top, r->right, r->bottom, gw->bckcolor);
    r->top = r->top + (dir * r->ymove);
    r->bottom = r->bottom + (dir * r->ymove);
    drawFilledRectangle(r->left, r->top, r->right, r->bottom, r->color);
}

void resetGame(gamewindow_t* gw, ball_t* b ,racket_t* r,brick_t* bricks,brick_t1* bricks1){
	

	
	drawFilledRectangle(r->left, r->top, r->right, r->bottom, gw->bckcolor);
	
	drawFilledCircle(b->centerx, b->centery, b->radius, gw->bckcolor);
	
	char arr[50] ;
	sprintf(arr , "Score: %d - Health:%d",score,life);
	setcolor(LIGHTBLUE);
	outtextxy(365, 0, arr);
	
	initRacket(r, RACKET_L, RACKET_T, RACKET_R, RACKET_B, RACKET_CLR, RACKET_XMOVE, RACKET_YMOVE);
    initBall(b, BALL_X, BALL_Y, BALL_R, BALL_CLR, BALL_XMOVE, BALL_YMOVE);
    
    
}

void drawlwBricks(brick_t* bricks){
    for (int i = 0; i < BRICKCOUNT; i++) {
        int left = LWBRICKX + i * 60;
        int top = LWBRICKY;
        int right = left + 50;
        int bottom = top + 20;
        int color = RED;
        initBrick(&bricks[i], left, top, right, bottom, color, false);
        setfillstyle(SOLID_FILL, bricks[i].color);
        bar(bricks[i].left, bricks[i].top, bricks[i].right, bricks[i].bottom);
    }
    
}
void drawlwBricksRefresher(brick_t* bricks){
    setfillstyle(SOLID_FILL, BCKCOLOR); // arka plan rengiyle doldurmak için fillstyle'i ayarlayin
    for (int i = 0; i < BRICKCOUNT; i++) {
        if (bricks[i].isDestroyed) { // yok edilmis bir tugla varsa
            bar(bricks[i].left, bricks[i].top, bricks[i].right, bricks[i].bottom); // tuglanin yerini arka plan rengiyle doldurun
        }
    }
}
void drawhwBricksRefresher(brick_t1* bricks1){
    setfillstyle(SOLID_FILL, BCKCOLOR); // arka plan rengiyle doldurmak için fillstyle'i ayarlayin
    for (int i = 0; i < HIGLEVELBRICKCOUNT; i++) {
        if (bricks1[i].isDestroyed) { // yok edilmis bir tugla varsa
           //  bar(bricks1[i].left, bricks1[i].top, bricks1[i].right, bricks1[i].bottom); // tuglanin yerini arka plan rengiyle doldurun
        	drawFilledRectangle(bricks1[i].left, bricks1[i].top, bricks1[i].right, bricks1[i].bottom, BCKCOLOR);
		}
        else if(!bricks1[i].isDestroyed&&bricks1[i].hitcount==1){
        	//setfillstyle(SOLID_FILL, DARKGRAY);
        	//bar(bricks1[i].left, bricks1[i].top, bricks1[i].right, bricks1[i].bottom);
        	drawFilledRectangle(bricks1[i].left, bricks1[i].top, bricks1[i].right, bricks1[i].bottom, DARKGRAY);
		}
    }
}

void drawhwBricks(brick_t1* bricks1){
	for (int i = 0; i < HIGLEVELBRICKCOUNT; i++) {
        int left = HWBRICKX + i * 60;
        int top = HWBRICKY;
        int right = left + 50;
        int bottom = top + 20;
        int color = YELLOW;
        initBrick2(&bricks1[i], left, top, right, bottom, color,0, false);
        setfillstyle(SOLID_FILL, bricks1[i].color);
        bar(bricks1[i].left, bricks1[i].top, bricks1[i].right, bricks1[i].bottom);
    }
}
void refreshTheScoreTable(){
	char arr[50] ;
	sprintf(arr , "Score: %d - Health:%d",score,life);
	setcolor(LIGHTBLUE);
	outtextxy(365, 0, arr);
}

void moveBall(gamewindow_t* gw, ball_t* b ,racket_t* r,brick_t* bricks,brick_t1* bricks1)
{
    drawFilledCircle(b->centerx, b->centery, b->radius, gw->bckcolor);

    // Collision checking
    if (b->centerx > 600 || b->centerx < 30 ) {
	 	/*if(b->centerx >=10 && b->centerx<=30){ //Left wall
	 		
	 		score2+=1;
	 		resetGame(gw, b, r, r1);
	 		outtextxy(240,240, "Right got point");
	 		delay(1000);
	 		drawFilledRectangle(240,200, 500, 360, gw->bckcolor);
	 		return;
	 		b->xmove *= -1.0;
		 }
		if(b->centerx >=590){        //right wall
			
	 		score1+=1;
	 		resetGame(gw, b, r, r1);
	 		outtextxy(240,240, "Left got point");
	 		delay(1000);
	 		drawFilledRectangle(240,200, 500, 360, gw->bckcolor);
	 		return;
	 		 b->xmove *= -1.0;
		}*/
        b->xmove *= -1.0;
    }

    // Bounch off top/bottom walls
    if (b->centery - b->radius > 392 || b->centery < 100 ) {
    	if(b->centery - b->radius> 392){
    		 b->ymove *= -1.0;
    		 life--;
    		 refreshTheScoreTable();
		}
		if( b->centery < 100){
			b->ymove *= -1.0;
		}
		
       
    }
    if(life==0){
			gameOver = true;
			gameMenu();
		}
	if ((b->centery+ b->radius >= r->top) && (b->centery - b->radius <= r->bottom)) {
    if ((b->centerx + b->radius >= r->left) && (b->centerx - b->radius <= r->right)) {
        /*if (b->centery > r->top && b->centery < r->bottom) {
            // Collision with the side of the paddle
            if(b->centerx + b->radius >= r->left||b->centerx - b->radius <= r->right){
            	b->xmove *= -1.0;
			}
        }
        else {*/
        if((b->centery+ b->radius >= r->top) && (b->centery - b->radius <= r->bottom)){
        	b->ymove *= -1.0;
		}
		
            

      // }
    }
	}
    // check collision with bricks
	/*for (int i = 0; i < BRICKCOUNT; i++) {
	    if (!bricks[0].isDestroyed) {
	    /*if ((b->centery+5 + b->radius >= bricks[i].top) && (b->centery- 5 - b->radius <= bricks[i]->bottom)) {
    		if ((b->centerx + b->radius >= bricks[i]->left) && (b->centerx - b->radius <= bricks[i]->right)) {
       			 if (b->centery > bricks[i]->top && b->centery < bricks[i]->bottom) {
            
	            if(b->centerx + b->radius >= bricks[i]->left||b->centerx - b->radius <= bricks[i]->right){
	            	b->xmove *= -1.0;
				}
	            
	            if (b->centerx > bricks[i]->left && b->centerx < bricks[i]->right) {
	               
	                b->ymove *= -1.0;
	            }
        	}
	        else {
	            
	        
	            b->ymove *= -1.0;
	            
	        }
    	   }
	    }*/
	    for (int i = 0; i < BRICKCOUNT; i++) {
		    if(!bricks[i].isDestroyed){
		    	if ((b->centery+5 + b->radius >= bricks[i].top) && (b->centery- 5 - b->radius <= bricks[i].bottom)){
			    	if ((b->centerx + b->radius >= bricks[i].left) && (b->centerx - b->radius <= bricks[i].right)) {
			    		b->ymove *= -1.0;
			    		bricks[i].isDestroyed = true;
			    		score++;
			    		refreshTheScoreTable();
			    		drawlwBricksRefresher(bricks);
				    }
				}
			}
		}
	
	
	cout<<bricks[0].top << " " <<bricks[0].bottom <<" "<<b->centery << " " <<b->centery- 5 <<"\n";
	
 	for (int i = 0; i < HIGLEVELBRICKCOUNT; i++) {
	    if (!bricks1[i].isDestroyed) {
	        	if ((b->centery+5 + b->radius >= bricks1[i].top) && (b->centery- 5 - b->radius <= bricks1[i].bottom)){ 
	        		if ((b->centerx + b->radius >= bricks1[i].left) && (b->centerx - b->radius <= bricks1[i].right)) {
			            b->ymove *= -1.0;
			            if(++bricks1[i].hitcount==2){
			            	bricks1[i].isDestroyed = true;
			            	refreshTheScoreTable();
			            	
			            }
			            score+=10;
			            drawhwBricksRefresher(bricks1);	
	        		}
	            }
	    }
	}
	
	



    b->centerx += b->xmove;
    b->centery += b->ymove;

    drawFilledCircle(b->centerx, b->centery, b->radius, b->color);
   	drawFilledRectangle(r->left, r->top, r->right, r->bottom, r->color);
   
}

bool manageGame(gamewindow_t* gw, racket_t* r, ball_t* b,brick_t* bricks,brick_t1* bricks1)
{
    int keyPressed;

    delay(0.5);

    if (kbhit()) {
        keyPressed = getch();
        if (keyPressed == ESC && gameOver) {
            closegraph();
        }
        if(keyPressed == RACKETLEFT1 || keyPressed == RACKETLEFT2){
        	if(r->left>10 && r->right<=670){
        		moveRacket(gw, r, MOVE_UP);  //SOLA
			}
			
        	
		}
		if(keyPressed == RACKETRIGHT1 || keyPressed == RACKETRIGHT2){
			if(r->left>=10 && r->right<670)	{
        		moveRacket(gw, r, MOVE_DOWN); //SAGA
			}
			
		}
		if(keyPressed == RACKETUP1 || keyPressed == RACKETUP2){
			if(r->top>205 && r->bottom<=470){
        		moveRacket2(gw, r, MOVE_UP); //yukari
			}
		
			
		}
		
		if(keyPressed == RACKETDOWN1 || keyPressed == RACKETDOWN2){
			if(r->top>=205 && r->bottom<410){
        		moveRacket2(gw, r, MOVE_DOWN); //asagi
			}
			
		}
      
       

    }
    moveBall(gw, b, r ,bricks,bricks1);
    return false;
}

void playGame(gamewindow_t* gw, racket_t* r , ball_t* b,brick_t* bricks,brick_t1* bricks1 )
{
    drawlwBricks(bricks);
    drawhwBricks(bricks1);
    while (!gameOver) {
        gameOver = manageGame(gw, r, b,bricks,bricks1);
    }
}


int main()
{
    gamewindow_t    gamew;
    racket_t        racket1;
  	brick_t bricks[BRICKCOUNT];
	brick_t1 bricks1[HIGLEVELBRICKCOUNT];
    ball_t          ball1;
	score=0;
	life=3;
	LABEL:if(gameOver){
		resetGame(&gamew, &ball1 , &racket1,bricks,bricks1);
			
	}
	
	
    startGame(&gamew, &racket1, &ball1);
	
    playGame(&gamew, &racket1, &ball1,bricks,bricks1);

    endGame();
}