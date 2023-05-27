/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "keyboard_map.h"


/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

extern unsigned char keyboard_map[128];
extern void keyboard_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* current cursor location */
unsigned int current_loc = 0;
/* video memory begins at address 0xb8000 */
char *vidptr = (char*)0xb8000;

/**********  MY DEFINITIONS ***********/
unsigned int r1_xpos = 24;
unsigned int r1_ypos = 24;
unsigned int r1_move = 5;
unsigned int r1_size=10;


unsigned int ball_xpos=12;
unsigned int ball_ypos=12;
unsigned int ball_xmove=1;
unsigned int ball_ymove=1;

unsigned int score=0;
unsigned int live=3;
unsigned int gameover=0;

#define BRICKCOUNT 7
struct Brick {
    unsigned int b_xpos;
	unsigned int b_ypos;
	unsigned int b_size;
	unsigned int b_destroyed;
};

struct Brick BRICKS[BRICKCOUNT];

void gotoxy(unsigned int x, unsigned int y);
void draw_strxy(const char *str,unsigned int x, unsigned int y);
void draw_rkt(void);
void draw_ball(void);
void init_bricks(void);
void draw_bricks(void);
void draw_score(void);
void draw_live(void);
void clear_ball(void);
void clear_rkt(void);
void move_ball(void);
void move_rkt_right(void);
void move_rkt_left(void);
void sleep(void) ;

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


void idt_init(void)
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);

	/* ICW4 - environment info */
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}

void kb_init(void)
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	write_port(0x21 , 0xFD);
}

void kprint(const char *str)
{
	unsigned int i = 0;
	while (str[i] != '\0') {
		vidptr[current_loc++] = str[i++];
		vidptr[current_loc++] = 0x07;
	}
}

void kprint_newline(void)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = current_loc + (line_size - current_loc % (line_size));
}

void clear_screen(void)
{
	unsigned int i = 0;
	while (i < SCREENSIZE) {
		vidptr[i++] = ' ';
		vidptr[i++] = 0x07;
	}
}

void gotoxy(unsigned int x, unsigned int y)
{
	unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
	current_loc = BYTES_FOR_EACH_ELEMENT * (x * COLUMNS_IN_LINE + y);
}

void draw_strxy(const char *str,unsigned int x, unsigned int y)
{
	gotoxy(x,y);
	kprint(str);
}

void draw_rkt(void)
{
	char rkt_s[r1_size+1]; // create a character array to hold the racket string
    for(int i = 0 ; i<r1_size;i++){
        rkt_s[i] = '='; // add "=" to the character array
    }
    rkt_s[r1_size] = '\0'; // null-terminate the character array

	draw_strxy(rkt_s,r1_xpos,r1_ypos);
}
void int_to_str(int num, char* str) {
    int i = 0, j = 0, is_negative = 0;
    char temp;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (j = 0; j < i / 2; j++) {
        temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}
char *strcat(char *dest, const char *src) {
    char *p = dest;
    while (*p != '\0') {
        p++;
    }
    while (*src != '\0') {
        *p++ = *src++;
    }
    *p = '\0';
    return dest;
}

void draw_score(void){
    char score_str[20] = "Score:  ";
    char score_num_str[10];

    int_to_str(score, score_num_str);

    strcat(score_str, score_num_str);

    draw_strxy(score_str, 0, 0);
}
void draw_congrats(void){
    char score_str[50] = "Congrats please hit the button B your score is:  ";
    char score_num_str[10];

    int_to_str(score, score_num_str);

    strcat(score_str, score_num_str);

    draw_strxy(score_str, 0, 0);
}

void draw_live(void){
    char live_str[20] = "Live:  ";
    char live_num_str[10];

    int_to_str(live, live_num_str);

    strcat(live_str, live_num_str);

    draw_strxy(live_str, 0, 70);
}



void draw_ball(void){
	const char *ball_s="O";
	draw_strxy(ball_s,ball_xpos,ball_ypos);

}
void init_bricks(void){
	for(int i=0;i<BRICKCOUNT;i++){
		BRICKS[i].b_size=5;
		BRICKS[i].b_xpos=5;
		BRICKS[i].b_ypos=5+i*10;
		BRICKS[i].b_destroyed=0;
	}
}

void draw_bricks(void){
    for(int i=0;i<BRICKCOUNT;i++){
        char b_s[BRICKS[i].b_size+1]; // create a character array to hold the racket string
        for(int j = 0 ; j<BRICKS[i].b_size;j++){
            if(BRICKS[i].b_destroyed!=1){
                b_s[j] = '+'; // add "+" to the character array
            }
            else{
                b_s[j] = ' '; // add " " to the character array
            }
        }
        b_s[BRICKS[i].b_size] = '\0'; // null-terminate the character array

        draw_strxy(b_s,BRICKS[i].b_xpos,BRICKS[i].b_ypos);
    }
}

void clear_ball(void)
{
	const char *ball_s="  ";

	draw_strxy(ball_s,ball_xpos,ball_ypos);
}

void clear_rkt(void)
{
	char rkt_s[r1_size+1]; // create a character array to hold the racket string
    for(int i = 0 ; i<r1_size;i++){
        rkt_s[i] = '  '; // add "=" to the character array
    }
    rkt_s[r1_size] = '\0'; // null-terminate the character array

	draw_strxy(rkt_s,r1_xpos,r1_ypos);
}

void move_ball(void) {
    clear_ball(); // clear the previous ball position

   

    // check if the ball has hit the edges of the screen
    if (ball_xpos >= 24 || ball_xpos <= 0) {
        ball_xmove = -ball_xmove;
		if(ball_xpos >= 24){
			live--;
			if(live==0){
				gameover=1;
			}
		}
    }
    if (ball_ypos >= 80 || ball_ypos <= 0) {
        ball_ymove = -ball_ymove;
    }

      // Check if the ball hits the paddle
    if (ball_ypos >=r1_ypos && ball_ypos <=r1_ypos+r1_size && ball_xpos+2>r1_xpos) {
        // Invert the y-direction of the ball
        ball_xmove = -ball_xmove;
    }

	// Check if the ball hits the bricks

	for(int i = 0;i<BRICKCOUNT;i++){
		if(BRICKS[i].b_destroyed!=1){
			if (ball_ypos >=BRICKS[i].b_ypos && ball_ypos <=BRICKS[i].b_ypos+BRICKS[i].b_size && ball_xpos-2<BRICKS[i].b_xpos) {
        	// Invert the y-direction of the ball
			BRICKS[i].b_destroyed=1;
        	ball_xmove = -ball_xmove;
			score++;
    		}
		}
		
		
	}



	 // update the ball position
    ball_xpos += ball_xmove; //x yukari assagi
	ball_ypos += ball_ymove; //y saga sola
    draw_bricks();
    draw_ball(); // draw the ball at the new position
}

void sleep(void) {
	for(int i=0;i<10000000*5;i++){
		
	}
}




void move_rkt_right(void)
{
	clear_rkt();
	r1_ypos = r1_ypos + r1_move;
	draw_rkt();
}
void move_rkt_left(void)
{
	clear_rkt();
	r1_ypos = r1_ypos - r1_move;
	draw_rkt();
}
void keyboard_handler_main(void)
{
	unsigned char status;
	char keycode;

	/* write EOI */
	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;

		if(keycode == ENTER_KEY_CODE) {
			return;
		}
	}

	keycode = keyboard_map[(unsigned char) keycode];
 	switch(keycode)
    	{
			case 'a':
		case 'A':
				if(r1_ypos <= 80 && r1_ypos >= 5){
					move_rkt_left();
				}
        		break;
    		case 'd':
		case 'D':
				if(r1_ypos <= 65 && r1_ypos >= 0){
					move_rkt_right();
				}
        		break;
		case 'b':
		case 'B':
			kprint("Stopping the CPU. Bye!");
    			__asm__ __volatile__("hlt");
    	}

}

unsigned char wait_for_key(void)
{
    unsigned char status;
    char keycode;

    /* Wait for a key to be pressed */
    do {
        status = read_port(KEYBOARD_STATUS_PORT);
    } while (!(status & 0x01));

    /* Read the key code */
    keycode = read_port(KEYBOARD_DATA_PORT);
    
    return keycode;
}

void kmain(void)
{
	clear_screen();
     
    kprint("Press Enter to start the game...");
    wait_for_key();
	clear_screen();
	idt_init();
	kb_init();
    init_bricks();
	draw_rkt();
	draw_ball();
	draw_bricks();
	draw_score();
	while(gameover!=1){
		
		if(score==7){
			clear_screen();
			draw_congrats();
			wait_for_key();

		}else{
			move_ball();
			draw_score();
			draw_live();
			sleep();
		}
	}
	clear_screen();
	kprint("     Game Over... \n");
	kprint("     Press Enter to restart the game...");
    wait_for_key();
}
