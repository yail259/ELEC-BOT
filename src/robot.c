#include "robot.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

typedef struct NodeNetwork *node;

void setup_robot(struct Robot *robot){
    robot->x = OVERALL_WINDOW_WIDTH/2-50;
    robot->y = OVERALL_WINDOW_HEIGHT-50;
    robot->true_x = OVERALL_WINDOW_WIDTH/2-50;
    robot->true_y = OVERALL_WINDOW_HEIGHT-50;
    robot->width = ROBOT_WIDTH;
    robot->height = ROBOT_HEIGHT;
    robot->direction = 0;
    robot->angle = 0;
    robot->currentSpeed = 0;
    robot->crashed = 0;
    robot->auto_mode = 0;
    robot->degreeMaxChange = 1;
    robot->maxDistance = 0;
    robot->turnAngle = 0;

    robot->head = createNode();
    node firstNode = addNode(robot->head, robot->x, robot->y);

    printf("Press arrow keys to move manually, or enter to move automatically\n\n");
}
int robot_off_screen(struct Robot * robot){
    if(robot->x < 0 || robot-> y < 0){
        return 0;
    }
    if(robot->x > OVERALL_WINDOW_WIDTH || robot->y > OVERALL_WINDOW_HEIGHT){
        return 0;
    }
    return 1;
}

int checkRobotHitWall(struct Robot * robot, struct Wall * wall) {

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

int checkRobotHitWalls(struct Robot * robot, struct Wall_collection * head) {
   struct Wall_collection *ptr = head;
   int hit = 0;

   while(ptr != NULL) {
      hit = (hit || checkRobotHitWall(robot, &ptr->wall));
      ptr = ptr->next;
   }
   return hit;

}

int checkRobotReachedEnd(struct Robot * robot, int x, int y, int width, int height){

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height,
                 x,width,y,height);

    return overlap;
}

void robotCrash(struct Robot * robot) {
    robot->currentSpeed = 0;
    if (!robot->crashed)
        printf("Ouchies!!!!!\n\nPress space to start again\n");
    robot->crashed = 1;
}

void robotSuccess(struct Robot * robot, int msec) {
    robot->currentSpeed = 0;
    if (!robot->crashed){
        printf("Success!!!!!\n\n");
        printf("Time taken %d seconds %d milliseconds \n", msec/1000, msec%1000);
        printf("Press space to start again\n");
    }
    robot->crashed = 1;
}

int checkRobotSensor(int x, int y, int sensorSensitivityLength, struct Wall * wall)  {
    //viewing_region of sensor is a square of 2 pixels * chosen length of sensitivity
    int overlap = checkOverlap(x,2,y,sensorSensitivityLength,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

int checkRobotSensorFrontRightAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    int i;
    int j;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;

    int sensorSensitivityLength =  floor(SENSOR_VISION/SENSOR_VISION);

    head_store = head;
    robotCentreX = robot->x + ROBOT_WIDTH/2;
    robotCentreY = robot->y + ROBOT_HEIGHT/2;
    score = 0;

    for (j = 0; j < 360; j++)
    {
        robot->vision[j] = 0;

        for (i = 0; i < SENSOR_VISION - 10; i++)
        {
            ptr = head_store;
            //+(ROBOT_WIDTH/2-2)*cos((robot->angle + j)*PI/180)
            //+(ROBOT_WIDTH/2-2)*sin((robot->angle + j)*PI/180)
            xDir = round(robotCentreX-
                         (-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle + j)*PI/180));
            yDir = round(robotCentreY+
                         (-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle + j)*PI/180));
            /*xDir = round(robotCentreX + i * sin((robot->angle + j)*PI/180));
            yDir = round(robotCentreY + i * cos((robot->angle + j)*PI/180));*/
            xTL = (int) xDir;
            yTL = (int) yDir;
            hit = 0;

            while(ptr != NULL) {
                hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
                ptr = ptr->next;
            }
            if (hit)
            {
                score = SENSOR_VISION - i;
                robot->vision[j] = score;
            }
        }
        if (j > 2 && robot->vision[j - 1] != 0 && robot->vision[j] != 0)
        {
            int newChange = abs(score - robot->vision[j-1]);
            int maxChange = abs(robot->vision[robot->degreeMaxChange] - robot->vision[robot->degreeMaxChange-1]);
            //printf("(%d %d)", newChange,maxChange);
            if (newChange > maxChange)
            {
                robot->degreeMaxChange = j;
                // printf("(%d, %d)", robot->degreeMaxChange, j);
            }
        }

        if (robot->vision[j] >= robot->maxDistance)
        {
            robot->maxDistance = robot->vision[j];
            //printf("%d", robot->maxDistance);
        }
    }
    //exit(0);
    return score;
}


void robotUpdate(struct SDL_Renderer * renderer, struct Robot * robot){
    double xDir, yDir;

    int robotCentreX, robotCentreY, xTR, yTR, xTL, yTL, xBR, yBR, xBL, yBL;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    /*
    //Other Display options:
    // The actual square which the robot is tested against (not so nice visually with turns, but easier
    // to test overlap
    SDL_Rect rect = {robot->x, robot->y, robot->height, robot->width};
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
    */
    /*
    //Center Line of Robot. Line shows the direction robot is facing
    xDir = -30 * sin(-robot->angle*PI/180);
    yDir = -30 * cos(-robot->angle*PI/180);
    xDirInt = robot->x+ROBOT_WIDTH/2+ (int) xDir;
    yDirInt = robot->y+ROBOT_HEIGHT/2+ (int) yDir;
    SDL_RenderDrawLine(renderer,robot->x+ROBOT_WIDTH/2, robot->y+ROBOT_HEIGHT/2, xDirInt, yDirInt);
    */

    //Rotating Square
    //Vector rotation to work out corners x2 = x1cos(angle)-y1sin(angle), y2 = x1sin(angle)+y1cos(angle)
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;

    xDir = round(robotCentreX+(ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xTR = (int) xDir;
    yTR = (int) yDir;

    xDir = round(robotCentreX+(ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xBR = (int) xDir;
    yBR = (int) yDir;

    xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xBL = (int) xDir;
    yBL = (int) yDir;

    xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    xTL = (int) xDir;
    yTL = (int) yDir;

    SDL_RenderDrawLine(renderer,xTR, yTR, xBR, yBR);
    SDL_RenderDrawLine(renderer,xBR, yBR, xBL, yBL);
    SDL_RenderDrawLine(renderer,xBL, yBL, xTL, yTL);
    SDL_RenderDrawLine(renderer,xTL, yTL, xTR, yTR);

    //Front Right Sensor
    int sensor_sensitivity =  floor(SENSOR_VISION/SENSOR_VISION);
    int i;
    int j;
    for (j = 0; j < 360; j++)
    {
        for (i = 0; i < SENSOR_VISION; i++)
        {
            xDir = round(robotCentreX+(ROBOT_WIDTH/2-2)*cos((robot->angle + j)*PI/180)-
                         (-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin((robot->angle + j)*PI/180));
            yDir = round(robotCentreY+(ROBOT_WIDTH/2-2)*sin((robot->angle + j)*PI/180)+
                         (-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos((robot->angle + j)*PI/180));
            xTL = (int) xDir;
            yTL = (int) yDir;

            SDL_Rect rect = {xTL, yTL, 2, sensor_sensitivity};
            SDL_SetRenderDrawColor(renderer, 80+(20*(SENSOR_VISION-i)), 80+(20*(SENSOR_VISION-i)), 80+(20*(SENSOR_VISION-i)), 255);
            SDL_RenderDrawRect(renderer, &rect);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void internalMap(struct SDL_Renderer * renderer, struct Robot * robot)
{
    for (int j=0; j<360; j++)
    {

        if (robot->vision[j] != 0) {
            int x_cor = (sin((robot->angle + j) * PI/180) * robot->vision[j]) + robot->x;
            int y_cor = -(cos((robot->angle + j) * PI/180) * robot->vision[j]) + robot->y;

            // main PRINT printf("(%d, %d)", j, robot->vision[j]);

            robot->robotMap[j][0] = x_cor;
            robot->robotMap[j][1] = y_cor;

            //printf("(%d, %d)", robot->robotMap[j][0], robot->robotMap[j][1]);

            SDL_Rect rect = {x_cor, y_cor, 10, 10};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 69);
            SDL_RenderDrawRect(renderer, &rect);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    for(int l=0; l < 360; l++)
    {
        if(robot->vision[(l+360-180)%180] != 0 && robot->vision[l] != 0)
        {
            int opposite_x = robot->robotMap[(-l+360)%360][0];
            int opposite_y = robot->robotMap[(-l+360)%360][1];

            int x_mod = robot->robotMap[l][0];
            int y_mod = robot->robotMap[l][1];

            if(opposite_x != x_mod && opposite_y != y_mod && l > 245 && l < 295)
            {
                int avg_x = (opposite_x + x_mod)/2;
                int avg_y = (opposite_y + y_mod)/2;

                /*SDL_Rect rect = {avg_x, avg_y, 10, 10};
                SDL_SetRenderDrawColor(renderer, 255, 165, 0, 69);
                SDL_RenderDrawRect(renderer, &rect);
                SDL_RenderFillRect(renderer, &rect);*/

                //printf("OPP(%d, %d)", opposite_x, opposite_y);
            }
        }
    }

    //exit(0);
}

node createNode()
{
    node tempNode;

    tempNode = (node)malloc(sizeof(struct NodeNetwork));

    tempNode->nextNode = NULL;

    return tempNode;
}

node addNode(node head, int newX, int newY)
{
    node tempNode, p;

    tempNode = createNode();

    tempNode->x = newX;
    tempNode->y = newY;

    if(head == NULL)
    {
        head = tempNode;
    }
    else
    {
        p = head;

        while(p->nextNode != NULL)
        {
            p = p->nextNode;
        }

        p->nextNode = tempNode;
    }

    return tempNode;
}

void drawNode(struct Robot * robot, struct SDL_Renderer * renderer, node head)
{
    while(head != NULL)
    {
        //printf("(%d, %d)", head->x, head->y);
        SDL_Rect rect = {head->x, head->y, 10, 10};
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 69);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
        head = head->nextNode;
    }
    moveToNode(robot, head);

    //exit(0);
    //struct Node valueThisNode = *robot->thisNode;
    //int valueThisNodeX = valueThisNode->x;

    //printf("YOU ROBOT (%d, %d)", valueThisNode->x, valueThisNode->y);
}

void robotMotorMove(struct Robot * robot) {
    double x_offset, y_offset;
    switch(robot->direction){
        case UP :
            robot->currentSpeed += DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed > MAX_ROBOT_SPEED)
                robot->currentSpeed = MAX_ROBOT_SPEED;
            break;
        case DOWN :
            robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                robot->currentSpeed = -MAX_ROBOT_SPEED;
            break;
        case LEFT :
            robot->angle = (robot->angle+360-DEFAULT_ANGLE_CHANGE)%360;
            break;
        case RIGHT :
            robot->angle = (robot->angle+DEFAULT_ANGLE_CHANGE)%360;
            break;
    }
    robot->direction = 0;
    x_offset = (-robot->currentSpeed * sin(-robot->angle*PI/180));
    y_offset = (-robot->currentSpeed * cos(-robot->angle*PI/180));

    robot->true_x += x_offset;
    robot->true_y += y_offset;

    x_offset = round(robot->true_x);
    y_offset = round(robot->true_y);

    robot->x = (int) x_offset;
    robot->y = (int) y_offset;
}

void printList(node head)
{
    //struct node ptr = head;

    printf("I'M HERE");

    while(head != NULL)
    {
        printf("(%d, %d)", head->x, head->y);
        head = head->nextNode;
    }

    // exit(0);
}

void detectFutureNode(struct Robot * robot)
{
    printf("DETECT WORKS %d", robot->x);

    int ADJUSTMENT_DEGREE = 5;
    for(int j = 1; j < 359; j++)
    {
        bool increasing;

        if(robot->vision[j+1] >= robot->vision[j])
        {
            increasing = true;
        }

        if(increasing && robot->vision[j+1] < robot->vision[j])
        {
            if(robot->vision[j - 1] < robot->vision[j])
            {
                if(robot->vision[j] > 100)
                {
                    int xNewNode = robot->x + (robot->vision[j] - 80) * sin((robot->angle +
                                                   (j - ADJUSTMENT_DEGREE)) * PI/180);
                    int yNewNode = robot->y - (robot->vision[j] - 80) * cos((robot->angle +
                                                   (j - ADJUSTMENT_DEGREE)) * PI/180);

                    node genericNode = addNode(robot->head, xNewNode, yNewNode);
                    increasing = false;
                }
            }
        }
    }
}

void moveToNode(struct Robot * robot, node targetNode)
{
    //printf("%d JUSTX", robot->x);
    int deltaX = targetNode->x - robot->x;
    int deltaY = targetNode->y - robot->y;
    //printf("%d DELTA", deltaX);
    double angle = atan((deltaY)/(deltaX)) * 180 / PI;
    int angleDegree = (int) angle;
    //printf("ANGLE %d", angleDegree);

    robot->turnAngle = 0;

    if(deltaX > 0)
    {
        robot->turnAngle = 90 + angleDegree;
    }
    else
    {
        robot->turnAngle = -90 + angleDegree;
    }

    printf("ANGLE %d", robot->turnAngle);
}

void robotAutoMotorMove(struct Robot * robot, struct SDL_Renderer * renderer) {

    /*
    node firstNode = addNode(robot->head, robot->x, robot->y);
    node secondNode = addNode(robot->head, 45, 23);
    node thirdNode = addNode(robot->head, 18, 69);
    */
    /*
    if (robot->turnAngle > 0)
    {
        robot->direction = RIGHT;
        robot->turnAngle--;
    }
    else if (robot->turnAngle < 0) {
        robot->direction = LEFT;
        robot->turnAngle++;
    }
    */

    detectFutureNode(robot);

    printList(robot->head);

    drawNode(robot, renderer, robot->head);
}
