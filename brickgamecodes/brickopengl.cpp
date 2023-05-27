#include <GL/freeglut_std.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <sstream>

#define WINDOW_W 800
#define WINDOW_H 500
#define BRICKCOUNT 7
static GLfloat screenLeft = -49.5;
static const int FPS = 60;
static GLfloat stick1Position = 10.0;
static GLfloat stickThick = 10.0;
static GLfloat stickSize = 1;
static GLfloat moveXFactor = 0.5;
static GLfloat moveYFactor = 0.5;
int gameOver = 0;
int gameRunning = 1;
int destroyedCount = 0;
static GLint player1_score = 0, player1_lives = 3;

struct _ball
{
	GLfloat radius = 1.5;
	GLfloat X = 0.0;
	GLfloat Y = 0.0;
	int directionX = -1;
	int directionY = 1;
}ball;

struct {
	GLfloat x1 = -20.0; //left
	GLfloat x2 = 0.0;  //right
	GLfloat y1 = -28.0; //top
	GLfloat y2 = -30.0; //bottom
	GLfloat speed = 1.0; //speed

}paddle;

struct bricklevel1 {
	GLfloat x1, x2, y1, y2;//left
	bool isDestroyed = false;
}
;
struct bricklevel2 {
	GLfloat x1, x2, y1, y2;//left
	int hitCount = 0;
	bool isDestroyed = false;
};

bricklevel1 Bricks[BRICKCOUNT];
bricklevel2 Bricks1[BRICKCOUNT];

void initBricklevel1() {
	
	GLfloat left = 13;
	
	for (int i = 0; i < BRICKCOUNT; i++) {
		Bricks[i].x1 = -45.0+ i*left ;
		Bricks[i].x2 = -34.0+ i*left ;
		Bricks[i].y1 = 18.0;
		Bricks[i].y2 = 16.0;
		if (Bricks[i].isDestroyed) {
			Bricks[i].isDestroyed = false;
		}

		std::cout << Bricks[i].x1 << " " << Bricks[i].x2 << " " << "\n";
	}
}

void initBricklevel2() {
	GLfloat left = 13;
	for (int i = 0; i < BRICKCOUNT - 1; i++) {
		Bricks1[i].x1 = -38.0 + i * left;
		Bricks1[i].x2 = -27.0 + i * left;
		Bricks1[i].y1 = 26.0;
		Bricks1[i].y2 = 24.0;
		Bricks1[i].hitCount = 0;
		if (Bricks1[i].isDestroyed) {
			Bricks1[i].isDestroyed = false;
		}
		
	}
}


void Drawbricks() {
	// Brickleri çiz
	for (int i = 0; i < BRICKCOUNT; i++) {
		if (!Bricks[i].isDestroyed) { // eğer brick yok edilmediyse
			glPushMatrix(); // brick'in koordinat sisteminin kaydedilmesi
			glTranslatef(Bricks[i].x1, Bricks[i].y1, 0.0f); // brick'in sol üst köşesine taşınması

			glColor3f(1.0f, 0.0f, 0.0f); //red
			glRectf(0.0f, 0.0f, Bricks[i].x2 - Bricks[i].x1, Bricks[i].y2 - Bricks[i].y1);

			glColor3f(1.0f, 1.0f, 1.0f); // orijinal renge geri dön
			glPopMatrix(); // brick'in koordinat sisteminin geri yüklenmesi
		}
	}

	// Bricklevel2 structları için
	for (int i = 0; i < BRICKCOUNT - 1; i++) {
		if (!Bricks1[i].isDestroyed) { // eğer brick yok edilmediyse
			if (Bricks1[i].hitCount == 0) // hit sayısı 0 ise beyaz renk
				glColor3f(1.0f, 1.0f, 1.0f);
			else if (Bricks1[i].hitCount == 1) // hit sayısı 1 ise sarı renk
				glColor3f(1.0f, 1.0f, 0.0f);
			else if (Bricks1[i].hitCount == 2) // hit sayısı 2 ise yeşil renk
				glColor3f(0.0f, 1.0f, 0.0f);
			else // hit sayısı 3 veya daha fazlaysa mavi renk
				glColor3f(0.0f, 0.0f, 1.0f);
			glPushMatrix(); // brick'in koordinat sisteminin kaydedilmesi
			glTranslatef(Bricks1[i].x1, Bricks1[i].y1, 0.0f); // brick'in sol üst köşesine taşınması
			glRectf(0.0f, 0.0f, Bricks1[i].x2 - Bricks1[i].x1, Bricks1[i].y2 - Bricks1[i].y1); // brick'i çiz
			glColor3f(1.0f, 1.0f, 1.0f); // orijinal renge geri dön
			glPopMatrix(); // brick'in koordinat sisteminin geri yüklenmesi
		}
	}

	// Paddle ve Ball objelerini çiz
	glColor3f(1.0f, 1.0f, 1.0f); // orijinal renge geri dön
}




void reshape(GLint w, GLint h)
{
	glViewport(0, 0, w, h);
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, 50.0, -50.0 / aspect, 50.0 / aspect, -1.0, 1.0);
}

void draw_circle(float x, float y, float radius) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	static const int circle_points = 100;
	static const float angle = 2.0f * 3.1416f / circle_points;

	// this code (mostly) copied from question:
	glBegin(GL_POLYGON);
	double angle1 = 0.0;
	glVertex2d(radius * cos(0.0), radius * sin(0.0));
	int i;
	for (i = 0; i < circle_points; i++)
	{
		glVertex2d(radius * cos(angle1), radius * sin(angle1));
		angle1 += angle;
	}
	glEnd();
	glPopMatrix();
}
void draw_paddle() {
	glRectf(paddle.x1, paddle.y1, paddle.x2, paddle.y2);
}
void drawStrokeText(char* string, float x, float y, float z)
{
	char* c;
	glRasterPos3f(x, y, z);

	for (c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}


void drawGameOverScreen() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-50, 50, -30, 30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(-20, 0);
	std::stringstream ss;
	ss << "GameOver ! Your score:" << player1_score;
	std::string gameover_str = ss.str();
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)gameover_str.c_str());

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(-20, -5);
	std::stringstream sc;
	sc << "Press 'R' to restart";
	std::string restart_str = sc.str();
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)restart_str.c_str());

	
	
}

void drawGameSucc() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-50, 50, -30, 30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(-20, 0);
	std::stringstream ss;
	ss << "Congrats Your score:" << player1_score;
	std::string gameover_str = ss.str();
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)gameover_str.c_str());

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(-20, -5);
	std::stringstream sc;
	sc << "Press 'R' to restart";
	std::string restart_str = sc.str();
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)restart_str.c_str());



}
void restartGame() {
	// Reset the game state variables here
	player1_score = 0;
	player1_lives = 3;
	initBricklevel1();
	initBricklevel2();
	Drawbricks();
	
	gameOver = 0;
	gameRunning = 1;
	destroyedCount = 0;
}

static char score_1[20], lives_1[20];

void display()
{
	

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (gameOver==1) {
		drawGameOverScreen();
		gameRunning = 0;
	}
	if (gameOver == 0 && gameRunning == 0) {
		drawGameSucc();
	}
	

	if (gameRunning==1) {
		


		glColor3f(1.0f, 1.0f, 1.0f);
		glRasterPos2i(35, -25);
		std::stringstream ss;
		ss << "Score:" << player1_score;
		std::string score_str = ss.str();
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)score_str.c_str());

		std::stringstream sc;
		sc << "Lives:" << player1_lives;
		std::string lives_str = sc.str();
		glRasterPos2i(-40, -25);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)lives_str.c_str());
		draw_paddle();
		draw_circle(ball.X, ball.Y, ball.radius);
		Drawbricks();
	}

	glutSwapBuffers();
}

void timer(int v)
{
	glutPostRedisplay();
	if (BRICKCOUNT + BRICKCOUNT - 1==destroyedCount) {
		gameRunning = 0;
		
	}
	if (gameRunning == 1) {
		if (ball.X + ball.radius > 50 || ball.X - ball.radius < -50)
		{
			ball.directionX = ball.directionX * (-1);

		}


		//Paddle top COllision
		if (ball.X + ball.radius >= paddle.x1 && ball.X + ball.radius <= paddle.x2 && ball.Y - ball.radius <= paddle.y1) {
			ball.directionY = ball.directionY * (-1);


		}
		//Paddle bottom Collision
		if (ball.X + ball.radius >= paddle.x1 && ball.X + ball.radius <= paddle.x2 && ball.Y + ball.radius <= paddle.y2) {
			ball.directionY = ball.directionY * (-1);

		}
		//Paddle right collision
		if (ball.X + ball.radius >= paddle.x2 && ball.X + ball.radius <= paddle.x2 + 5 && ball.Y + ball.radius >= paddle.y1 && ball.Y - ball.radius <= paddle.y2) {
			ball.directionX = ball.directionX * (-1);
		}

		//Paddle left collision
		if (ball.X - ball.radius <= paddle.x1 && ball.X - ball.radius >= paddle.x1 - 5 && ball.Y + ball.radius >= paddle.y1 && ball.Y - ball.radius <= paddle.y2) {
			ball.directionX = ball.directionX * (-1);
		}


		//Low level Bricks collision check

		for (int i = 0; i < BRICKCOUNT; i++) {
			if (!Bricks[i].isDestroyed) {
				//brick bottom collision check
				if (ball.X + ball.radius >= Bricks[i].x1 && ball.X + ball.radius <= Bricks[i].x2 && ball.Y + ball.radius >= Bricks[i].y2) {
					ball.directionY = ball.directionY * (-1);
					Bricks[i].isDestroyed = true;
					Drawbricks();
					player1_score = player1_score + 10;
					destroyedCount++;

				}
				//brick top collision check
				if (ball.X + ball.radius >= Bricks[i].x1 && ball.X + ball.radius <= Bricks[i].x2 && ball.Y - ball.radius >= Bricks[i].y1) {
					ball.directionY = ball.directionY * (-1);
					Bricks[i].isDestroyed = true;
					Drawbricks();
					player1_score = player1_score + 10;
					destroyedCount++;
				}
				if (ball.X + ball.radius >= Bricks[i].x2 && ball.X + ball.radius <= Bricks[i].x2 + 5 && ball.Y + ball.radius >= Bricks[i].y1 && ball.Y - ball.radius <= Bricks[i].y2) {
					ball.directionX = ball.directionX * (-1);
					Bricks[i].isDestroyed = true;
					Drawbricks();
					player1_score = player1_score + 10;
					destroyedCount++;
				}

				//Paddle left collision
				if (ball.X - ball.radius <= Bricks[i].x1 && ball.X - ball.radius >= Bricks[i].x1 - 5 && ball.Y + ball.radius >= Bricks[i].y1 && ball.Y - ball.radius <= Bricks[i].y2) {
					ball.directionX = ball.directionX * (-1);
					Bricks[i].isDestroyed = true;
					Drawbricks();
					player1_score = player1_score + 10;
					destroyedCount++;
				}



			}
		}

		for (int i = 0; i < BRICKCOUNT - 1; i++) {
			if (!Bricks1[i].isDestroyed) {
				//brick bottom collision check
				if (ball.X + ball.radius >= Bricks1[i].x1 && ball.X + ball.radius <= Bricks1[i].x2 && ball.Y + ball.radius >= Bricks1[i].y2) {
					ball.directionY = ball.directionY * (-1);
					if (Bricks1[i].hitCount == 3) {
						Bricks1[i].isDestroyed = true;
						player1_score = player1_score + 30;
						destroyedCount++;

					}
					Bricks1[i].hitCount++;
					Drawbricks();

				}
				//brick top collision check
				if (ball.X + ball.radius >= Bricks1[i].x1 && ball.X + ball.radius <= Bricks1[i].x2 && ball.Y - ball.radius >= Bricks1[i].y1) {
					ball.directionY = ball.directionY * (-1);
					if (Bricks1[i].hitCount == 3) {
						Bricks1[i].isDestroyed = true;
						player1_score = player1_score + 30;
						destroyedCount++;

					}
					Bricks1[i].hitCount++;
					Drawbricks();

				}
				if (ball.X + ball.radius >= Bricks1[i].x2 && ball.X + ball.radius <= Bricks1[i].x2 + 5 && ball.Y + ball.radius >= Bricks1[i].y1 && ball.Y - ball.radius <= Bricks1[i].y2) {
					ball.directionX = ball.directionX * (-1);
					if (Bricks1[i].hitCount == 3) {
						Bricks1[i].isDestroyed = true;
						player1_score = player1_score + 30;
						destroyedCount++;

					}
					Bricks1[i].hitCount++;
					Drawbricks();
				}

				//Paddle left collision
				if (ball.X - ball.radius <= Bricks1[i].x1 && ball.X - ball.radius >= Bricks1[i].x1 - 5 && ball.Y + ball.radius >= Bricks1[i].y1 && ball.Y - ball.radius <= Bricks1[i].y2) {
					ball.directionX = ball.directionX * (-1);
					if (Bricks1[i].hitCount == 3) {
						Bricks1[i].isDestroyed = true;
						player1_score = player1_score + 30;
						destroyedCount++;

					}
					Bricks1[i].hitCount++;
					Drawbricks();
				}
			}
		}

		if (ball.Y + ball.radius > 30 || ball.Y - ball.radius < -30)
		{
			std::cout << ball.X + ball.radius << " " << ball.Y + ball.radius << "\n";
			ball.directionY = ball.directionY * -1;
			if (ball.Y - ball.radius < -30) {
				player1_lives--;
				if (player1_lives < 0) {
					gameOver = 1;
				}
			}

		}
		ball.X = ball.X + (moveXFactor * ball.directionX);
		ball.Y = ball.Y + (moveYFactor * ball.directionY);
	}
	
	
	glutTimerFunc(1000 / FPS, timer, v);
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{

	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{

	}
}

void kbSpecial(int key, int x, int y)
{
	
}
void NormalKeyHandler(unsigned char key, int x, int y)
{
	if (gameOver==1 || gameRunning==0 && key == 'r') {
		restartGame();
	}
	else if (key == 'w') {
		if (paddle.y1 < -10) {
			paddle.y1 = paddle.y1 + paddle.speed;
			paddle.y2 = paddle.y2 + paddle.speed;
			std::cout << paddle.x1 << " " << paddle.x2 << " " << paddle.y1 << " " << paddle.y2 << "\n";
		}
	}
	else if (key == 's') {
		if (paddle.y2 > -31) {
			paddle.y1 = paddle.y1 - paddle.speed;
			paddle.y2 = paddle.y2 - paddle.speed;
			std::cout << paddle.x1 << " " << paddle.x2 << " " << paddle.y1 << " " << paddle.y2 << "\n";
		}
	}
	else if (key == 'a'){
		//do something here
		
		if (paddle.x1 > -50) {
			paddle.x1 = paddle.x1 - paddle.speed;
			paddle.x2 = paddle.x2 - paddle.speed;
			std::cout << paddle.x1 << " " << paddle.x2 << " " << paddle.y1 << " " << paddle.y2 << "\n";
		}
	}
	else if (key == 'd'){
			if (paddle.x2 < 50) {
				paddle.x1 = paddle.x1 + paddle.speed;
				paddle.x2 = paddle.x2 + paddle.speed;
				std::cout << paddle.x1 << " " << paddle.x2 << " " << paddle.y1 << " " << paddle.y2 << "\n";
			}
		} // do other stuff
	// and so on...
}


int main(int argc, char** argv)
{
	initBricklevel1();
	initBricklevel2();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(260, 140);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutCreateWindow("Spinning Square");
	glutReshapeFunc(reshape);
	
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, 0);
	glutMouseFunc(mouse);
	glutSpecialFunc(kbSpecial);
	glutKeyboardFunc(NormalKeyHandler);
	glutMainLoop();
}