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
    robot->distanceTotal = 0;
    robot->distanceLeft = 0;

    robot->moving = false;

    robot->currentNode = createNode();
    node firstNode = addNode(robot->currentNode, robot->x, robot->y, 0);

    robot->currentNode = firstNode;

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
                         (-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle + j + 180)*PI/180));
            yDir = round(robotCentreY+
                         (-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle + j + 180)*PI/180));
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
        // printf("(%d, %d)", j, robot->vision[j]);
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

void internalMap(struct SDL_Renderer * renderer, struct Robot * robot)
{
    for (int j=0; j<360; j++)
    {

        if (robot->vision[j] != 0) {
            int x_cor = (sin((robot->angle + j+180) * PI/180) * robot->vision[j]) + robot->x;
            int y_cor = -(cos((robot->angle + j+180) * PI/180) * robot->vision[j]) + robot->y;

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

    //exit(0);
}

node createNode()
{
    node tempNode;

    tempNode = (node)malloc(sizeof(struct NodeNetwork));

    for (int i=0; i<FUTURE_NODE_NO; i++) {
        tempNode->nextNode[i] = NULL;
        tempNode->anglesToRobot[i] = NULL;
    }

    tempNode->x = NULL;
    tempNode->y = NULL;
    tempNode->prevNode = NULL;
    tempNode->deadEnd = false;
    tempNode->scanned = false;

    return tempNode;
}

node addNode(node currentNode, int newX, int newY, int angle)
{
    node tempNode;

    tempNode = createNode();

    tempNode->x = newX;
    tempNode->y = newY;
    tempNode->prevNode = currentNode;

    // printf("(%d, %d)", newX, newY);

    for (int i=0; i<FUTURE_NODE_NO; i++) {
        if (currentNode->nextNode[i] == NULL) {
            currentNode->nextNode[i] = tempNode;
            currentNode->anglesToRobot[i] = angle;
            node possiblePath = currentNode->nextNode[i];
            printf("ADDNODE (%d, %d, %d)", i, possiblePath->x, possiblePath->y);
            break;
        }
    }
    return tempNode;
}

void drawNode(struct SDL_Renderer * renderer, node thisNode)
{
    // printf("DRAWNODE (%d, %d)", thisNode->x, thisNode->y);
    SDL_Rect rect = {thisNode->x, thisNode->y, 10, 10};
    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 69);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
}

/*
void printList(node currentNode)
{
    //struct node ptr = head;

    printf("I'M HERE");

    for (int i=0; i<FUTURE_NODE_NO; i++) {
        node possiblePath = currentNode->nextNode[i];
        if (currentNode->nextNode[i] != NULL) {
            printf("(%d, %d)", possiblePath->x, possiblePath->y);
        }
    }

    // exit(0);
}*/

void detectFutureNode(struct Robot * robot)
{
    //printf("DETECT WORKS %d", robot->x);
    int SCAN_RESOLUTION = 45;
    int DISTANCE_FROM_WALL = 30;


    node currentNode = robot->currentNode;
    currentNode->scanned = true;

    for(int j = 45; j < 359 - SCAN_RESOLUTION; j+=SCAN_RESOLUTION)
    {
        if(robot->vision[j] > 100)
        {
            printf("%d", j);
            int xNewNode = robot->x + (robot->vision[j] - DISTANCE_FROM_WALL) * sin((robot->angle + (j) + 180) * PI/180);
            int yNewNode = robot->y - (robot->vision[j] - DISTANCE_FROM_WALL) * cos((robot->angle + (j) + 180) * PI/180);

            node newNode = addNode(robot->currentNode, xNewNode, yNewNode, j);
        }
    }

    for (int i=0; i<FUTURE_NODE_NO; i++) {
        node possiblePath = currentNode->nextNode[i];
        if (possiblePath != NULL) {
            printf("DETECTNODE PATHS");
            printf("(%d, %d)", possiblePath->x, possiblePath->y);
            node prevNode = possiblePath->prevNode;
            printf("PREVIOUS (%d, %d)", prevNode->x, prevNode->y);
        }
    }
}

//Check deadEnd
bool checkDeadEnd(node currentNode) {
    bool deadEnd = true;

    for (int i=0; i<FUTURE_NODE_NO; i++) {
        node possiblePath = currentNode->nextNode[i];
        if (possiblePath != NULL && !possiblePath->deadEnd) {
            deadEnd = false;
        }
    }

    if (deadEnd) {
        currentNode->deadEnd = true;
    }

    return deadEnd;
}


void moveToNode(struct Robot * robot, node possiblePath, int angle) {
    node currentNode = robot->currentNode;

    robot->turnAngle = angle;
    printf("TURNANGLE %d \n", robot->turnAngle);

    int deltaX = possiblePath->x - robot->x;
    int deltaY = possiblePath->y - robot->y;

    robot->distanceTotal = sqrt(deltaX*deltaX + deltaY*deltaY);
    printf("\n DELTA %d %d DISTANCETOTAL %d", deltaX, deltaY, robot->distanceTotal);

    robot->moving = true;
    robot->currentNode = possiblePath;
}



void robotAutoMotorMove(struct Robot * robot, struct SDL_Renderer * renderer) {

    /*
    node firstNode = addNode(robot->head, robot->x, robot->y);
    node secondNode = addNode(robot->head, 45, 23);
    node thirdNode = addNode(robot->head, 18, 69);
    */

    /*
    for (int j=0; j<360; j++) {
        if (robot->vision[j] == 0) {
            if (j < 180) {
                //robot->direction = LEFT;
            } else if (j > 180) {
                robot->direction = RIGHT;
            } else {
                robot->direction = UP;
            }
        }
    }
    */


    if (robot->moving) {
        if (robot->turnAngle > 8)
        {
            robot->direction = RIGHT;
            robot->turnAngle-=15;
        }
        else if (robot->turnAngle < -8) {
            robot->direction = LEFT;
            robot->turnAngle+=15;
        }
        else {
            node destination = robot->currentNode;
            robot->distanceLeft = sqrt((destination->x - robot->x)*(destination->x - robot->x) +
                                       (destination->y - robot->y)*(destination->y - robot->y));
            //printf("DISTANCE LEFT %d", robot->distanceLeft);
            if (robot->distanceLeft < 50) {
                if (robot->currentSpeed>0) {
                    robot->direction = DOWN;
                } else {
                    robot->moving = false;
                }
            }
            else if (robot->distanceLeft > robot->distanceTotal/2)
                robot->direction = UP;
            else
                robot->direction = DOWN;
        }
    }
    else
    {
        if (!robot->currentNode->scanned)
            detectFutureNode(robot);

        node currentNode = robot->currentNode;

        checkDeadEnd(currentNode);

        if (currentNode->deadEnd)
        {
            printf("\n DEADEND \n");

            node prevNode = currentNode->prevNode;
            moveToNode(robot, prevNode, -180);
        }
        else
        {
            for (int i=0; i<FUTURE_NODE_NO; i++)
            {
                node possiblePath = currentNode->nextNode[i];
                if (possiblePath != NULL && !possiblePath->deadEnd)
                {
                    drawNode(renderer, possiblePath);

                    printf("\n NEWPATH \n");
                    moveToNode(robot, possiblePath, (currentNode->anglesToRobot[i] - 180));
                }
            }
        }

    }
}
