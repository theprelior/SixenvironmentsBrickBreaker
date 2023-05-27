import turtle as t
import os
import time

# initialize game variables
live = 3
score = 0

# create game window
win = t.Screen()    
win.title("TheBrickBreaker")
win.bgcolor('black')    
win.setup(width=800,height=600) 
win.tracer(0)  


bricks_level1 = []
brick_count = 9
bricks_level2 = []
brick_count = 9

# create level 1 bricks
def create_level1_bricks():
    for i in range(brick_count):
        brick = t.Turtle()
        brick.speed(0)
        brick.shape('square')
        brick.color('orange')
        brick.shapesize(stretch_wid=1, stretch_len=4)
        brick.penup()
        brick.goto(-360+i*100, 200)
        bricks_level1.append(brick)

# create level 2 bricks
def create_level2_bricks():
    for i in range(brick_count):
        brick = t.Turtle()
        brick.speed(0)
        brick.shape('square')
        brick.color('yellow')
        brick.shapesize(stretch_wid=1, stretch_len=4)
        brick.penup()
        brick.goto(-360+i*100, 240)
        brick.health = 2
        bricks_level2.append(brick)

# create player paddle
paddle1 = t.Turtle()

def init_player():
    paddle1.speed(0)
    paddle1.shape('square')
    paddle1.color('white')
    paddle1.shapesize(stretch_wid=1,stretch_len=5)
    paddle1.penup()
    paddle1.goto(0,-240)

# create game ball
ball = t.Turtle()
ball_dx = 0.5  
ball_dy = 0.5
def init_game_ball():
    ball.speed(0)
    ball.shape('circle')
    ball.color('red')
    ball.penup()
    ball.goto(0,0)
    

# create score and lives display
pen = t.Turtle()
def create_score():
    pen.speed(0)
    pen.color('skyblue')
    pen.penup()
    pen.hideturtle()
    pen.goto(0,260)
    pen.write("Live: {}   TheBrickBreaker   Score: {}".format(live, score), align="center", font=('Monaco',24,"normal"))

# define paddle movement functions
def pad1_right():
    if paddle1.ycor() <= 240:
        x = paddle1.xcor()
        x += 10
        paddle1.setx(x)

def pad1_left():
    if paddle1.ycor() >= -240:
        x = paddle1.xcor()
        x -= 10
        paddle1.setx(x)

def pad1_up():
    if paddle1.ycor() <= -140:
        y = paddle1.ycor()
        y += 10
        paddle1.sety(y)

def pad1_down():
    if paddle1.ycor() >= -230:
        y = paddle1.ycor()
        y -= 10
        paddle1.sety(y)

# register paddle movement functions with keyboard
win.listen()
win.onkeypress(pad1_right,"d")
win.onkeypress(pad1_left,"a")
win.onkeypress(pad1_up,"w")
win.onkeypress(pad1_down,"s")


# define collision detection
def collision(ball, brick):
    ball_x = ball.xcor()
    ball_y = ball.ycor()
    brick_x = brick.xcor()
    brick_y = brick.ycor()
    if (ball_x + 10 >= brick_x - 40) and (ball_x - 10 <= brick_x + 40) and (ball_y + 10 >= brick_y - 10) and (ball_y - 10 <= brick_y + 10):
        return True
    else:
        return False

# define main game loop

def main_game():
    global ball_dx
    global ball_dy
    global live
    global score
    global ball
    global bricks_level1
    global bricks_level2
    while True:
        win.update()

        # move ball
        ball.setx(ball.xcor() + ball_dx)
        ball.sety(ball.ycor() + ball_dy)

        # check for wall collisions
        if ball.xcor() > 390:
            ball_dx *= -1
        if ball.xcor() < -390:
            ball_dx *= -1
        if ball.ycor() > 290:
            ball_dy *= -1
        if ball.ycor() < -290:
            live -= 1
            pen.clear()
            pen.write("Live: {}   TheBrickBreaker   Score: {}".format(live, score), align="center", font=('Monaco',24,"normal"))
            ball_dy *= -1
            time.sleep(1)

        # check for paddle collision
        if (ball.ycor() < paddle1.ycor()+20) and (ball.ycor() > paddle1.ycor()) and (ball.xcor() < paddle1.xcor() + 50) and (ball.xcor() > paddle1.xcor() - 50):
            ball_dy *= -1

        # check for brick collision
        for brick in bricks_level1 + bricks_level2:
            if collision(ball, brick):
                if brick in bricks_level2:
                    brick.health -= 1
                    if brick.health == 1:
                        brick.color('green')
                    elif brick.health == 0:
                        brick.color('black')
                        bricks_level2.remove(brick)
                        score += 20
                else:
                    bricks_level1.remove(brick)
                    brick.hideturtle()
                    score += 10
                ball_dy *= -1
                pen.clear()
                pen.write("Live: {}   TheBrickBreaker   Score: {}".format(live, score), align="center", font=('Monaco',24,"normal"))

        # check for game over
        if live == 0 or (len(bricks_level1) == 0 and len(bricks_level2) == 0):
            pen.clear()
            pen.goto(0, 0)
            pen.write("Game Over", align="center", font=('Monaco',36,"normal"))
            time.sleep(3)
            win.bye()

create_level1_bricks()
create_level2_bricks()
init_player()
init_game_ball()
create_score()
main_game()