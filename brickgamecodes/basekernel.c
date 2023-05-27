/*
Copyright (C) 2015-2019 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#include "console.h"
#include "page.h"
#include "process.h"
#include "keyboard.h"
#include "mouse.h"
#include "interrupt.h"
#include "clock.h"
#include "ata.h"
#include "device.h"
#include "cdromfs.h"
#include "string.h"
#include "graphics.h"
#include "kernel/ascii.h"
#include "kernel/syscall.h"
#include "rtc.h"
#include "kernelcore.h"
#include "kmalloc.h"
#include "memorylayout.h"
#include "kshell.h"
#include "cdromfs.h"
#include "diskfs.h"
#include "serial.h"

/*
This is the C initialization point of the kernel.
By the time we reach this point, we are in protected mode,
with interrupts disabled, a valid C stack, but no malloc heap.
Now we initialize each subsystem in the proper order:
*/
int width = 128, height = 95;
char scene[128][95];
char letter[128][95];
int boole=1;
int score = 0;
int lives=3;
int brick_count = 12;


typedef struct {
    int x, y;
    int vx, vy;
} Ball;

typedef struct {
    int x, y;
    int length;
} Paddle;

typedef struct {
    int x, y;
    int destroyed;
	int length;
} Brick;

Ball ball;
Paddle paddle;
Brick bricks[12];

void clear_screen() {
	int y,x;
	for (y = 3; y < height-1;y++) {

		for (x = 1; x < width-1; x++) {
			scene[x][y] = ' ';
				
		}
		
	}
}

void clear_score_table() {
	int x;
	

		for (x = 0; x < width-1; x++) {
			scene[x][0] = ' ';
			
				
		}
		
	
}

void init_ball() {
    ball.x = width / 2;
    ball.y = height / 2;
    ball.vx = 1;
    ball.vy = -1;
}

void init_paddle() {
    paddle.x = width / 2 - 5;
    paddle.y = height - 2;
    paddle.length = 10;
}
void init_bricks() {
	for(int i=0;i<brick_count;i++){
		bricks[i].length=8;
		bricks[i].y=5;
		bricks[i].x=5+i*10;
		bricks[i].destroyed=0;
	}
}

void draw_ball() {
    scene[ball.x][ball.y] = 'O';
}
void draw_paddle() {
    int x;
    for (x = paddle.x; x < paddle.x + paddle.length; x++) {
		
        scene[x][paddle.y] = '=';
    }
}
void draw_bricks() {
   
	  for(int i=0;i<brick_count;i++){
        
        for(int j = 0 ; j<bricks[i].length;j++){
            if(bricks[i].destroyed!=1){
				scene[bricks[i].x+j][bricks[i].y]=219;
            }
            else{
                scene[bricks[i].x+j][bricks[i].y]=' ';
            }
        }
       

    }
}

void draw_score(struct graphics *g){
	gotoxy(g,1,1,"Score:");
	char scorestr;
			uint_to_string(score,scorestr);
			gotoxy(g,7,1,scorestr);
	gotoxy(g,width-10,1,"Lives:");
	char livesstr;
			uint_to_string(lives,livesstr);
			gotoxy(g,width-4,1,livesstr);
}


void move_ball(struct graphics *g) {
    ball.x += ball.vx;
    ball.y += ball.vy;

    // Detect collisions with walls
    if (ball.x <= 0 || ball.x >= width - 1) {
        ball.vx = -ball.vx;
    }
    if (ball.y <= 3 || ball.y >= height-2) {
        ball.vy = -ball.vy;
		if(ball.y >= height-2){
			lives--;
			clear_score_table();
		    draw_score(g);
			if(lives==0){
				boole=0;
			}
		}
    }

	//Deteck collision with paddle

	if(paddle.x<=ball.x&&paddle.x+paddle.length>=ball.x&&ball.y+1==paddle.y){
		ball.vy = -ball.vy;
	}
	else if(paddle.x<=ball.x&&paddle.x+paddle.length>=ball.x&&ball.y-1==paddle.y){
		ball.vy = -ball.vy;
	}

	//Detect bricks collision

	for(int i=0;i<brick_count;i++){
		if(bricks[i].destroyed==0){
			if(bricks[i].x<=ball.x&&bricks[i].x+bricks[i].length>=ball.x&&ball.y+1==bricks[i].y){
				ball.vy = -ball.vy;
				bricks[i].destroyed=1;
				score=score+10;
				clear_score_table();
				draw_score(g);
				
			}
			else if(bricks[i].x<=ball.x&&bricks[i].x+bricks[i].length>=ball.x&&ball.y-1==bricks[i].y){
				ball.vy = -ball.vy;
				bricks[i].destroyed=1;
				score=score+10;
				clear_score_table();
				draw_score(g);
			}
		}
	}
	

}


void walls() {
	
	int x,y;
	
	for (x = 0; x < width; x++) { 
		if(x < width-1){
			scene[x][2] = 219; //top
		}
		
		scene[x][height - 1] = 219; //bottom
		
	}
	for (y = 2; y < height; y++) {
		scene[0][y] = 219; //left
		scene[width - 1][y] = 219; //right
		
	}
	
}
void gotoxy(struct graphics *g,int x,int y,char *str){
	x = x * 8;
	y = y * 8;
	int value=0;
	int bit = 8;
	while(strlen(str)>value){
		graphics_char(g,x+(bit*value),y,str[value]);
		value++;
	}
}
void DrawScreen(struct graphics *g) {
	int y = 0,x =0;
	char alan[2];
	for (y = 0; y < height;y++) {

		for (x = 0; x < width; x++) {
			
		alan[0] = scene[x][y];
		alan[1] = 0;
		gotoxy(g,x,y,alan);
		
		}
		
	}
}
void delay(){
	for(int i = 0; i < 1000000;i++){ 
		
	}
}

void win_over(struct graphics *g){
	graphics_clear(g,0,0,1000,1000);
	clear_screen();
	DrawScreen(g);
	gotoxy(g,50,48,"Congrats");
	//kprint("Game Over " );
	gotoxy(g,50,50,"Your Score is Here : ");
	char scorestr;
	uint_to_string(score,scorestr);
	gotoxy(g,71,50,scorestr);
	gotoxy(g,50,52,"Restart(R)");
	char *key[2],numr[2];
		while(1){
				key[0] = keyboard_read(0);
				key[1] = 0;
				numr[0] = 114;
				numr[1] = 0;
				if(key[0] ==  numr[0]){
					graphics_clear(g,0,0,1200,1000);
					boole = 1;
					lives=3;
					score = 0;
					menu(g);
					continue;
	 		
	 			}
	 	}
}
void game_over(struct graphics *g){
	graphics_clear(g,0,0,1000,1000);
	clear_screen();
	DrawScreen(g);
	gotoxy(g,50,48,"Game Over");
	//kprint("Game Over " );
	gotoxy(g,50,50,"Your Score is Here : ");
	char scorestr;
	uint_to_string(score,scorestr);
	gotoxy(g,71,50,scorestr);
	gotoxy(g,50,52,"Restart(R)");
	char *key[2],numr[2];
		while(1){
				key[0] = keyboard_read(0);
				key[1] = 0;
				numr[0] = 114;
				numr[1] = 0;
				if(key[0] ==  numr[0]){
					graphics_clear(g,0,0,1200,1000);
					boole = 1;
					lives=3;
					score = 0;
					menu(g);
					continue;
	 		
	 			}
	 	}
}
void games(struct graphics *g){
	char *key[2],up[2],left[2],right[2],down[2];
	key[0]=0;
	key[1] =0;
	up[0] = 119;
	up[1] = 0;
	left[0] = 97;
	left[1] = 0;
	right[0] =100;
	right[1] =0;
	down[0] = 115;
	down[1] = 0;
	clear_screen();
	init_ball();
	init_bricks();
	init_paddle();
	DrawScreen(g);
	clear_score_table();
	draw_score(g);
	while(boole){
		key[0] = keyboard_read(1);
		key[1] = 0;
		if(key[0] == up[0]&&paddle.y>10){
			paddle.y--;
		}
		if(key[0] == down[0]&&paddle.y<height-4){
			paddle.y++;
		}
		if(key[0] == right[0]&&paddle.x+paddle.length<width-2){
			paddle.x=paddle.x+2;
		}
		if(key[0] == left[0]&&paddle.x>1){
			paddle.x=paddle.x-2;
		}
		walls();
		draw_ball();
		draw_bricks();
		draw_paddle();
		move_ball(g);
		DrawScreen(g);
		clear_screen();
		if(score/10==brick_count){
			win_over(g);
		}
		delay();	 
	}
	if(boole==0){
	//screen_clear(g);
		
			game_over(g);
				
	}
	
	
}
void menu(struct graphics *g){
	char *key[2],num1[2],num2[2],num3[2];
	num1[0] = '1';
	num1[1] = 0;
	num2[0] = '2';
	num2[1] = 0;
	num3[0] = '3';
	num3[1] = 0;
		gotoxy(g,40,50,"TO START PLAYING THE BRICK BREAKER GAME PRESS 1");
		
	
	 key[0] = keyboard_read(0);
	 key[1] = 0;
	 //printf(num1);
	 if(key[0] ==  num1[0]){
	 
	 	games(g);
	 
	 }
	
	}
int kernel_main()
{
	struct graphics *g = graphics_create_root();

	console_init(g);
	console_addref(&console_root);
	page_init();
	kmalloc_init((char *) KMALLOC_START, KMALLOC_LENGTH);
	interrupt_init();
	rtc_init();
	keyboard_init();
	process_init();
	
	menu(g);
	char alan[2];
		alan[0] = 219;
		alan[1] = 0;

	return 0;
}