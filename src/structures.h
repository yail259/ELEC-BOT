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
#define SENSOR_VISION 400

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
};

/*struct Coordinate
{
	float x;
	float y;
};

struct SplineCurve
{
    //vector storing coordinates to construct spline curve.
	vector<Coordinate> splinePoints;

	Coordinate getPoint(float t)
	{
		int p0, p1, p2, p3;

        p1 = (int)t;
        p2 = (p1 + 1) % points.size();
        p3 = (p2 + 1) % points.size();
        p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;

		t = t - (int)t;

		float tt = t * t;
		float ttt = tt * t;

		float q1 = -ttt + 2.0f*tt - t;
		float q2 = 3.0f*ttt - 5.0f*tt + 2.0f;
		float q3 = -3.0f*ttt + 4.0f*tt + t;
		float q4 = ttt - tt;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
	}

    Coordinate getGradient(float t)
	{
		int p0, p1, p2, p3;


        p1 = (int)t;
        p2 = (p1 + 1) % points.size();
        p3 = (p2 + 1) % points.size();
        p0 = p1 >= 1 ? p1 - 1 : points.size() - 1;

		t = t - (int)t;

		float tt = t * t;
		float ttt = tt * t;

		float q1 = -3.0f * tt + 4.0f*t - 1;
		float q2 = 9.0f*tt - 10.0f*t;
		float q3 = -9.0f*tt + 8.0f*t + 1.0f;
		float q4 = 3.0f*tt - 2.0f*t;

		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);

		return{ tx, ty };
	}
};*/

#endif // STRUCTURES_H_INCLUDED
