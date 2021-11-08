#include "stdio.h"
#include "stdlib.h"
#include "sdl.h"
#include "SDL2_gfxPrimitives.h"
#include "time.h"

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


    window = SDL_CreateWindow("Robot Maze", 0, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Window *window2 = SDL_CreateWindow("Robot Vision", 640, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Renderer *renderer2 = SDL_CreateRenderer(window2, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_left_sensor, front_right_sensor=0;
    clock_t start_time, end_time;
    int msec;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    int gap_width = 50;

    //entry wall right
    insertAndSetFirstWall(&head, 1,
                          OVERALL_WINDOW_WIDTH/6 - 80 + gap_width, OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2,
                          WALL_WIDTH, OVERALL_WINDOW_HEIGHT/2 - gap_width/2);

    //entry wall left
    insertAndSetFirstWall(&head, 2,
                          OVERALL_WINDOW_WIDTH/6 - 95, OVERALL_WINDOW_HEIGHT/6 -50,
                          WALL_WIDTH, 5*OVERALL_WINDOW_HEIGHT/6 + WALL_WIDTH + 50);

    //further wall right
    insertAndSetFirstWall(&head, 3,
                          OVERALL_WINDOW_WIDTH/6 - 80 + gap_width, OVERALL_WINDOW_HEIGHT/6 + 9,
                          WALL_WIDTH, OVERALL_WINDOW_HEIGHT/3 - gap_width/2 + 40);

    //dead end
    insertAndSetFirstWall(&head, 4,
                          OVERALL_WINDOW_WIDTH/2 - 70, OVERALL_WINDOW_HEIGHT/6 - 40,
                          WALL_WIDTH, gap_width + 10);

    // upper lid
    insertAndSetFirstWall(&head, 5,
                          OVERALL_WINDOW_WIDTH/6 - 95 , OVERALL_WINDOW_HEIGHT/6 -50,
                          OVERALL_WINDOW_WIDTH/2 - OVERALL_WINDOW_WIDTH/6 + 35, WALL_WIDTH);

    insertAndSetFirstWall(&head, 6,
                          OVERALL_WINDOW_WIDTH/6 - 80 + gap_width, OVERALL_WINDOW_HEIGHT/6 + 10,
                          OVERALL_WINDOW_WIDTH/2 - OVERALL_WINDOW_WIDTH/6 - gap_width + WALL_WIDTH, WALL_WIDTH);

    insertAndSetFirstWall(&head, 7,
                          OVERALL_WINDOW_WIDTH/6 - 80 + gap_width , OVERALL_WINDOW_HEIGHT/2 + gap_width/2 - WALL_WIDTH,
                          2*OVERALL_WINDOW_WIDTH/3 -50, WALL_WIDTH);
    insertAndSetFirstWall(&head, 8,
                          OVERALL_WINDOW_WIDTH/6 - 80 + gap_width , OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2,
                          OVERALL_WINDOW_WIDTH/4, WALL_WIDTH);

    insertAndSetFirstWall(&head, 9,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4), OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2,
                          WALL_WIDTH, OVERALL_WINDOW_HEIGHT/5);

    insertAndSetFirstWall(&head, 10,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4), OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2 + OVERALL_WINDOW_HEIGHT/5,
                          OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH, WALL_WIDTH);

    insertAndSetFirstWall(&head, 11,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9, OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2,
                          WALL_WIDTH, OVERALL_WINDOW_HEIGHT/5);

    insertAndSetFirstWall(&head, 12,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9, OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2,
            (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH , WALL_WIDTH);

    insertAndSetFirstWall(&head, 13,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9 + (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH ,
                          OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2,
                          WALL_WIDTH, OVERALL_WINDOW_HEIGHT/5);

    insertAndSetFirstWall(&head, 14,
                           (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9 + (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH ,
                           OVERALL_WINDOW_HEIGHT/2 + 3*gap_width/2 + OVERALL_WINDOW_HEIGHT/5,
                          OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH, WALL_WIDTH);

    insertAndSetFirstWall(&head, 15,
                          OVERALL_WINDOW_WIDTH/6 + 420, OVERALL_WINDOW_HEIGHT/6 + 41,
                          WALL_WIDTH, 5*OVERALL_WINDOW_HEIGHT/8);

    insertAndSetFirstWall(&head, 16,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9 + (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH ,
                          OVERALL_WINDOW_HEIGHT/6 + 41,
                          WALL_WIDTH, OVERALL_WINDOW_HEIGHT/5 + 48);

    insertAndSetFirstWall(&head, 17,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9,
                          OVERALL_WINDOW_HEIGHT/6 + 41,
            (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH , WALL_WIDTH);

    insertAndSetFirstWall(&head, 18,
                          OVERALL_WINDOW_WIDTH/6 + 420,
                          OVERALL_WINDOW_HEIGHT/6 + 41,
            (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH , WALL_WIDTH);

    insertAndSetFirstWall(&head, 19,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9,
                          OVERALL_WINDOW_HEIGHT/6 -28,
                        WALL_WIDTH, 70);

    insertAndSetFirstWall(&head, 20,
                          (OVERALL_WINDOW_WIDTH/6 - 80 + gap_width) + (OVERALL_WINDOW_WIDTH/4) + OVERALL_WINDOW_WIDTH/9,
                          OVERALL_WINDOW_HEIGHT/6 -28,
            (2*OVERALL_WINDOW_WIDTH/3 -50) - OVERALL_WINDOW_WIDTH/4 - (OVERALL_WINDOW_WIDTH/9 + WALL_WIDTH) + WALL_WIDTH + 200, WALL_WIDTH);

    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;

    while(!done){
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
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
        if (checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT/2+100, 10, 100)){
            end_time = clock();
            msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
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
                start_time = clock();
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
