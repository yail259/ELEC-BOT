#include "stdio.h"
#include "stdlib.h"
#include "sdl.h"
#include "SDL2_gfxPrimitives.h"
//#include "time.h"
#include "sys/time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"

int done = 0;


int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }

    //Welcome Screen
    SDL_Window * welcome = SDL_CreateWindow("Welcome Screen",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    SDL_Renderer *rendererW = SDL_CreateRenderer(welcome, -1, 0);

    SDL_Surface *image = SDL_LoadBMP("startscreen.bmp");

    SDL_Texture *texture = SDL_CreateTextureFromSurface(rendererW, image);
    SDL_RenderCopy(rendererW, texture, NULL, NULL);
    SDL_RenderPresent(rendererW);

    SDL_bool start = SDL_FALSE;
    SDL_Event ev;

    while(start == SDL_FALSE){
        while(SDL_PollEvent(&ev)){
            const Uint8 *input = SDL_GetKeyboardState(NULL);
            if(ev.type == SDL_QUIT){
                done = 1;
            }
            if(input[SDL_SCANCODE_SPACE]){


                SDL_DestroyTexture(texture);
                SDL_FreeSurface(image);
                SDL_DestroyRenderer(rendererW);
                SDL_DestroyWindow(welcome);

                start = SDL_TRUE;
            }
        }
    }


    window = SDL_CreateWindow("Robot Maze", 0, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Window *window2 = SDL_CreateWindow("Robot Vision", 640, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *renderer2 = SDL_CreateRenderer(window2, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_left_sensor, front_right_sensor=0;
    //clock_t start_time, end_time;
    //int msec;
    struct timeval start_time, end_time;
    gettimeofday(&start_time, 0);
    unsigned long msec;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    int gap_width = 60;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    #include "math.h"

    insertAndSetFirstWall(&head, 1,  OVERALL_WINDOW_WIDTH/2, OVERALL_WINDOW_HEIGHT/2, 10, OVERALL_WINDOW_HEIGHT/2);
insertAndSetFirstWall(&head, 2,  OVERALL_WINDOW_WIDTH/2-100, OVERALL_WINDOW_HEIGHT/2+100, 10, OVERALL_WINDOW_HEIGHT/2-100);
insertAndSetFirstWall(&head, 3,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2+100, 150, 10);
insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2, 150, 10);
insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
insertAndSetFirstWall(&head, 6,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 10, 100);
insertAndSetFirstWall(&head, 7,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 450, 10);
insertAndSetFirstWall(&head, 8,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 250, 10);
insertAndSetFirstWall(&head, 9,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
insertAndSetFirstWall(&head, 10,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2-100, 10, 300);
insertAndSetFirstWall(&head, 11,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2+200, OVERALL_WINDOW_WIDTH/2-100, 10);
insertAndSetFirstWall(&head, 12,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2+100, OVERALL_WINDOW_WIDTH/2-100, 10);

    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;

    while(!done){
        SDL_SetRenderDrawColor(renderer, 97, 97, 97, 255);
        SDL_RenderClear(renderer);

//        SDL_SetRenderDrawColor(renderer2, 97, 97, 97, 255);
//        SDL_RenderClear(renderer2);

        //Move robot based on user input commands/auto commands
        if (robot.auto_mode == 1)
        {
            robotAutoMotorMove(&robot, renderer2);
            //drawNode(&robot, renderer2);
        }
        robotMotorMove(&robot);

        //Check if robot reaches endpoint. and check sensor values
        if (checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT/2+100, 10, 100)){ //Maze 5){
            //end_time = clock();
            //msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
            gettimeofday(&end_time, 0);
            msec = ((end_time.tv_sec - start_time.tv_sec)*1000)+(end_time.tv_usec - start_time.tv_usec)/1000;
            robotSuccess(&robot, msec);
        }
        else if(checkRobotHitWalls(&robot, head))
            robotCrash(&robot);
        //Otherwise compute sensor information
        else {
            front_right_sensor = checkRobotSensorFrontRightAllWalls(&robot, head);
            /*if (front_right_sensor>0)
                printf("Getting close on the right. Score = %d\n", front_right_sensor);*/
            internalMap(renderer2, &robot);

        }
        robotUpdate(renderer, &robot);
//        internalMap(renderer2, &robot);
        updateAllWalls(head, renderer);
//        updateAllWalls(head, renderer2);

        // Check for user input
        SDL_RenderPresent(renderer);
        SDL_RenderPresent(renderer2);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                robot.direction = UP;
            }
            if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                robot.direction = DOWN;
            }
            if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
                robot.direction = LEFT;
            }
            if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
                robot.direction = RIGHT;
            }
            if(state[SDL_SCANCODE_SPACE]){
                setup_robot(&robot);
            }
            if(state[SDL_SCANCODE_RETURN]){
                robot.auto_mode = 1;
                //start_time = clock();
                gettimeofday(&start_time, 0);
            }
        }

        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
//    SDL_DestroyRenderer(renderer2);
    SDL_DestroyWindow(window2);
    printf("DEAD\n");
}
