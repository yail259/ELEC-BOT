#include "stdbool.h"

#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define OVERALL_WINDOW_WIDTH 640
#define OVERALL_WINDOW_HEIGHT 480

#define ROBOT_WIDTH 20
#define ROBOT_HEIGHT 20

#define WALL_WIDTH 10

#define DEFAULT_ANGLE_CHANGE 15
#define DEFAULT_SPEED_CHANGE 1
#define MAX_ROBOT_SPEED 25
#define PI 3.14159265
#define SENSOR_VISION 420

#define FUTURE_NODE_NO 5

#define SCAN_RESOLUTION 45
#define DISTANCE_FROM_WALL 30
#define DISTANCE_THRESHOLD 100

typedef struct NodeNetwork *node;

struct Wall {
    int x,y;
    int width, height;
};

struct Wall_collection {
    int key;
    struct Wall wall;
    struct Wall_collection *next;
};

struct Robot {
    int x, y;
    double true_x, true_y;
    int direction;
    int angle;
    int currentSpeed;
    int width, height;
    int crashed;
    int auto_mode;
    int vision[360];
    int degreeMaxChange;
    int robotMap[360][2];
    int maxDistance;
    int turnAngle;
    int distanceTotal;
    int distanceLeft;
    int collisionWait;
    bool foundGoal;
    bool moving;
    node head;
    node currentNode;
    node movingFrom;
};

struct NodeNetwork
{
    int x, y;
    struct Node *nextNode[FUTURE_NODE_NO];
    struct Node *prevNode;
    bool deadEnd;
    bool scanned;
};



#endif // STRUCTURES_H_INCLUDED
