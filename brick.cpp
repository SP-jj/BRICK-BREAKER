// https://www.youtube.com/watch?v=oZ92bAOOrLI&list=PLVotA8ycjnCtVD07Ps7cMr2NUwdnVC0SZ&index=10&ab_channel=AveryMakesGamesAveryMakesGames

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

using namespace std;

const int WIDTH = 620;
const int HEIGHT = 720;
const int FONT_SIZE = 32;
const int SPEED = 9;
const int BALL_SPEED = 12;
const int BALL_SIZE = 16;
const int ROW = 8;			// horizontal length of rows
const int COL = 13;
const int SPACING = 16;		//space between each brick
const float PI = 3.14;


SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font* font;
bool running;
int frameCount, timerFPS,lastFrame,fps;
SDL_Color color;

// SDL_Rect : https://wiki.libsdl.org/SDL_Rect#:~:text=An%20SDL_Rect%20defines%20a%20rectangular,by%20several%20other%20video%20functions.
SDL_Rect paddle , ball, lives, brick;


float velY, velX;	//velocity
int lifeCount;

// for brick placements, we're ginna have an array of booleans(if a brick is there or not) and recheck it every game loop
// this isnt gonna work for if you want rows with different amounts of bricks
//		to do ^ that, use a vector array instead
bool bricks[ROW * COL];

void write(string text, int x, int y)
{

	const char* t = text.c_str();
	SDL_Surface *surface = TTF_RenderText_Solid(font, t, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	//surface = TTF_RenderText_Solid(font, t, color);
	// if(!(surface=TTF_RenderText_Solid(font,t,color))) 
	
	// {
    // //handle error here, perhaps print TTF_GetError at least
	// 	cout << "ttf error here\n";
	// 	cout << TTF_GetError() << '\n';
	// }

	//texture = SDL_CreateTextureFromSurface(renderer, surface);

	// -> is for getting the property of an object, but you only have the pointer to that obj
	lives.w = surface->w;
	lives.h = surface->h;
	lives.x = x - lives.w;
	lives.y = y - lives.h;

	SDL_FreeSurface(surface);

	SDL_RenderCopy(renderer , texture, NULL, &lives);

	SDL_DestroyTexture( texture );

}
void resetBricks()
{
	// set all bricks to true to display them all	
	for (int i = 0; i < COL*ROW; i++)
	{
		bricks[i] = true;		
	}

	// reset lives
	lifeCount = 3;

	// put paddle in centre screen
	paddle.x = (WIDTH/2) - (paddle.w/2);

	//put ball's y_location to a little above the paddle
	ball.y = paddle.y - paddle.h*4;

	velY = BALL_SPEED/2;
	velX = 0;

	// put ball's x_location to centre screen(right above the paddle)
	ball.x = WIDTH/2 - BALL_SIZE/2;
}

// get and assign coordinates of the brick rect to brick at index i
// because were not using a 2d array, you need some math to calculate coordinates
void setBricks(int i)
{
	brick.x = ( ((i%COL) + 1) * SPACING) + ( (i%COL) * brick.w ) - (SPACING/2);
	brick.y =  ( ((i%ROW) + 1) * SPACING) + ( (i%ROW) * brick.w ) - (SPACING/2) + brick.h*3; //y is shifted down more because scoreboard is at top
}

void update()
{

	// check if player is out of lives
	if(lifeCount <= 0)
		resetBricks();

	// check if the ball is hitting the paddle
	if(SDL_HasIntersection( &ball , &paddle))
	{
		double rel = (paddle.x + (paddle.w/2)) - ( ball.x + (BALL_SIZE/2));
		double norm = rel/(paddle.w/2);
		double bounce = norm * (5*PI/12);
		velY = -BALL_SPEED * cos(bounce);
		velY = BALL_SPEED * -sin(bounce);
	}

	// check if the ball is hitting the walls
	//top border
	if(ball.y <= 0)
		velY = -velY;
	// bottom border
	if (ball.y + BALL_SIZE >= HEIGHT)
	{
		lifeCount--;
		velY = -velY;
	}
	// left and right borders
	if( ball.x <= 0 || ball.x+BALL_SIZE >= WIDTH)
		velX = -velX;


	//move ball
	ball.x += velX;
	ball.y += velY;

	//make sure the paddle isnt going out of the screen
	if(paddle.x < 0)
		paddle.x = 0;
	if(paddle.x+paddle.w > WIDTH)	
		paddle.x = WIDTH - paddle.w;


	// check if ball is hitting the bricks
	// iterate through each brick
	bool reset = true;
	for (int i = 0; i < ROW*COL; i++)
	{
		setBricks(i);
		//if ball is in the area of the brick, and the brick exists: set brick to not exist anymore
		if(SDL_HasIntersection(&ball, &brick) && bricks[i])
		{
			bricks[i] = false;

			// set new diretion of ball
			if(ball.x >= brick.x)
			{
				velX = -velX;
				ball.x -= 20;
			}
			if(ball.x <= brick.x)
			{
				velX = -velX;
				ball.x += 20;
			}
			if(ball.y >= brick.y)
			{
				velY = -velY;
				ball.y -= 20;
			}
			if(ball.y <= brick.y)
			{
				velY = -velY;
				ball.y += 20;
			}

			reset = false;
		}
	}
	if(reset)
		resetBricks();
}
void input()
{
	// check if esc was pressed and for paddle controls (L & R)
	SDL_Event e;
	const Uint8 *keystates = SDL_GetKeyboardState(NULL);

	while(SDL_PollEvent(&e))
	{
		if( e.type == SDL_QUIT ) // if user hits the x button of the window
			running = false;
		
		if(keystates[SDL_SCANCODE_ESCAPE])	//check if user presses esc
			running = false;

		if(keystates[SDL_SCANCODE_LEFT])
			paddle.x -= SPEED;				// move x coordinate val decreases when moving left


		if(keystates[SDL_SCANCODE_RIGHT])
			paddle.x += SPEED;				// move x coordinate val increases when moving right
	}
}
void render()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255); //black background
	SDL_RenderClear(renderer);

	frameCount++;

	// num of frames that have happened within the current second
	timerFPS = SDL_GetTicks() - lastFrame;

	// delay screen every 60 frames(every 1 sec)
	if(timerFPS < (1000/60))
	{
		SDL_Delay((1000/60) - timerFPS);
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	// make everything white

	SDL_RenderFillRect( renderer, &paddle);	//color paddle
	
	SDL_RenderFillRect( renderer, &ball);	//color ball

	write(to_string(lifeCount), WIDTH/2 + FONT_SIZE/2, FONT_SIZE*1.5);	// display current lives status

	// bricks will have different colors
	for( int i=0; i < ROW*COL; i++)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red

		// bricks will alternate between 2 colors
		if( i % 2 == 0 )
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green

		// iterate through bricks that exist
		if( bricks[i] )
		{
			setBricks(i);
			SDL_RenderFillRect(renderer, &brick);
		}
	}


	SDL_RenderPresent(renderer);
}

int main()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
		cout << "Failed at SDL_Init()\n";

	if(SDL_CreateWindowAndRenderer( WIDTH, HEIGHT, 0, &window, &renderer) < 0)
		cout << "Failed at SDL_CreateWindowAndRenderer()\n";
	
	SDL_SetWindowTitle(window,"Brick Breaker");
	if(TTF_Init() < 0)
		cout << "Failed at TTF_Init()\n";
	
	font = TTF_OpenFont("font.ttf", FONT_SIZE);
	running = true;

	static int lastTime = 0;

	color.r = color.g = color.b = 255;

	paddle.h = 12;
	paddle.w = WIDTH/4;

	paddle.y = HEIGHT - paddle.h - 32;

	ball.w = ball.h = BALL_SIZE;

	// calculates brick size based on how many columns there will be(taking into accout spacing)
	// 		(WIDTH - (SPACING * COL): subtracts total spacing length from window width, so now you just have the length of screen the bricks take up
	//  						/ COL: divides a row's total length of bricks by columns, so now you have the length of each brick
	brick.w = (WIDTH - (SPACING*COL))/COL;

	brick.h = 22;

	resetBricks();

	// game loop
	while(running)
	{
		// Get the number of milliseconds since SDL library initialization.
		lastFrame = SDL_GetTicks();

		// new frame once every second
		if(lastFrame == lastTime +1000) // if 1 sec has passed since the frame, fps, frameCount reset
		{
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}

		update();
		input();
		render();
	}

	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	TTF_Quit();

	return 0;
	
}