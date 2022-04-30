#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <math.h>
using namespace std;

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 720
#define FONT_SIZE 32
#define BALL_SIZE 16
#define BALL_SPEED 16
#define PLAYER_SPEED 9
#define PI 3.14159265358979323846

int Screen_Width = SCREEN_WIDTH, Screen_Height = SCREEN_HEIGHT;

bool gameRunning;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color color, bg;
TTF_Font* font;
int lastFrame, lastTime = 0, fps, frameCount, timerFPS;
SDL_Rect l_paddle, r_paddle, ball, score_board;
float xvel, yvel;
string score;
int lscore, rscore;
bool turn;

void write(string text, int x, int y)
{
    SDL_Surface* surface;
    SDL_Texture* texture;
    if(font == NULL)
        cout<<"Font not found\n";
    const char* t = text.c_str();
    surface = TTF_RenderText(font, t, color, bg);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    score_board.w = surface->w;
    score_board.h = surface->h;
    score_board.x = x - surface->w;
    score_board.y = y - surface->h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &score_board);
    SDL_DestroyTexture(texture);       
}

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000/60)){
        SDL_Delay((1000/60)-timerFPS);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    write(score, (Screen_Width/2) + FONT_SIZE, FONT_SIZE*2);
    SDL_RenderFillRect(renderer, &l_paddle);
    SDL_RenderFillRect(renderer, &r_paddle);
    SDL_RenderFillRect(renderer, &ball);

    SDL_RenderPresent(renderer);
    // SDL_RenderClear(renderer);

}

void serve(){
    l_paddle.y = r_paddle.y = (Screen_Height/2) - (l_paddle.h/2);
    if (turn){
        ball.x = l_paddle.x + (l_paddle.w*4);
        xvel = BALL_SPEED/2;
    } else {
        ball.x = r_paddle.x - (r_paddle.w*4);
        xvel = -BALL_SPEED/2;
    }
    yvel = 0;
    ball.y = (Screen_Height/2) - (BALL_SIZE/2);
    turn = !turn;
    
}

void input(){
    SDL_Event event;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            gameRunning = false;
        
        if (keystates[SDL_SCANCODE_ESCAPE])
            gameRunning = false;

        if (keystates[SDL_SCANCODE_UP])
            r_paddle.y -= PLAYER_SPEED;
        if (keystates[SDL_SCANCODE_DOWN])
            r_paddle.y += PLAYER_SPEED;
        
        if (keystates[SDL_SCANCODE_S])
            l_paddle.y += PLAYER_SPEED;
        if(keystates[SDL_SCANCODE_W])
            l_paddle.y -= PLAYER_SPEED;
            
        
    }
}
void update(){
    if (SDL_HasIntersection(&ball, &l_paddle)){
        double rel = (l_paddle.y + (l_paddle.h/2)) - (ball.y + (BALL_SIZE/2));
        double norm = rel/(l_paddle.h)/2;
        double bounce = norm*(5*PI/12);
        xvel = BALL_SPEED * cos(bounce);
        yvel = (BALL_SPEED * -sin(bounce));
        if (yvel == 0) yvel++;
    }

    if (SDL_HasIntersection(&ball, &r_paddle)){
        double rel = (r_paddle.y + (r_paddle.h/2)) - (ball.y + (BALL_SIZE/2));
        double norm = rel/(r_paddle.h)/2;
        double bounce = norm*(5*PI/12);
        xvel = -BALL_SPEED * cos(bounce);
        yvel = (BALL_SPEED * -sin(bounce));
        if (yvel == 0) yvel++;
    }

    if (ball.y <= 0){
        yvel = -yvel;
    }
    if (ball.y + BALL_SIZE >= Screen_Height)
        yvel = -yvel;

    ball.x += xvel;
    ball.y += yvel;

    if(ball.x <= 0){
        rscore++;
        serve();
    }
    if (ball.x + BALL_SIZE >= Screen_Width){
        lscore++;
        serve();
    }
    score = to_string(lscore)+" "+to_string(rscore);

    if (l_paddle.y <= 0)
        l_paddle.y = 0;
    if (l_paddle.y + l_paddle.h >= Screen_Height)
        l_paddle.y = Screen_Height - l_paddle.h;

    if(r_paddle.y <= 0)
        r_paddle.y = 0;
    if (r_paddle.y + r_paddle.h >= Screen_Height)
        r_paddle.y = Screen_Height - r_paddle.h;
         
}

int main(int argv, char* argc[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        cout<<"Error while initializing sdl\n";
    if (TTF_Init() < 0)
        cout<<"Error while initializing ttf\n";
    if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer) < 0)
        cout<<"Error while creating window and renderer\n";
    font = TTF_OpenFont("Peepo.ttf", FONT_SIZE);

    SDL_GetRendererOutputSize(renderer, &Screen_Width, &Screen_Height);

    gameRunning = true;
    color.r=color.b=color.g=255;
    bg.r=bg.b=bg.g = 0;

    lscore = rscore = 0;
    l_paddle.x = 32;
    l_paddle.h = Screen_Height/5;
    l_paddle.w = 12;
    l_paddle.y = (Screen_Height/2) - (l_paddle.h/2);
    r_paddle = l_paddle;
    r_paddle.x = Screen_Width-r_paddle.w-32;

    ball.w = ball.h = BALL_SIZE;

    serve();
    
    while(gameRunning){
        lastFrame = SDL_GetTicks();
        if(lastFrame >= lastTime+1000){
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
    return 0;
}