# ELEC1601 Project


Maze Navigating Robot
Hardware Implementation

The maze navigating robot uses one spinning LiDAR (Light Detection And Ranging) sensor VL53L0X (ST microelectronics, see Appendix A). That uses a time of flight mechanism to calculate distances of objects the laser bounces off of for upto 2m with a resolution of 1mm (Mouser Electronics, n.d.). Since the program's resolution is 1 pixel = 5mm, the simulated robot could detect distances accurate to 1 pixel.

Initially three ultrasound sensors were used but we wanted more points of data from the environment. Since the ultrasound sensors have a radial spread in its measure of distance, it can’t be used on spinning DC motors as consecutive measurements will interfere with each other. Therefore we tried to use three lidar sensors however the Arduino only has one I2C port. Thus, we decided to mount the lidar sensor on a DC motor to achieve the functionality of multiple measurements at different angles.

The spinning lidar sensor is mounted onto a DC motor at the centre of the robot and spins horizontally for 360 degrees to scan the entire 2D map as far as it can see for 400 pixels. Lidar functions through Arduino’s 2 I2C ports (Arduino, n.d.), with the A4 and A5 pins controlling the SCL (Serial clock) and SDL(Serial Data) lines. DC motor speed should be set such that the maximum time taken to range the distance (22ms) is within 1/360th of a spin (calculations in Appendix H). The lidar sensor allows us to have a 360 degree range of vision and can replicate the section of the map it sees accurate to the specs of the sensor. A new window is created to the right of the original to visualise what the robot is able to see (Appendix F). 
Software Implementation
The robot stores a length 360 array inside it’s memory, where each entry is the distance sensed at a particular degree. This is flushed every time a robot moves to conserve memory.

One approach we tried was that for each distance scanned, the robot uses an algorithm which compares the distance before and the distance after that particular degree. Since at corners and openings distance increased and decreased, this was thought to be able to detect them.  

If robot sensing distance is increasing:
    Set a toggle to detect decreasing distance
If robot sensing distance is decreasing and toggle is activated:
    Place a node in front of the wall as it’s either a corner or an opening.


However this approach was abandoned as many nodes were placed unexpectedly with inconsistencies in the collision and more complex maps with unpredictable open spaces. 
This is also unrealistic in terms of the Arduino’s processing ability in collecting and storing a distance value for every angle within one cycle of the robot’s motion.

The subsequent approach we took was sampling an angle every 15 degrees. Then every 45 degrees the distance was compared to a threshold value. If the distance is larger than the threshold value, then a node is placed 30 pixels before the wall at that degree. This forms one of the possible paths that the robot can move to.
For every 15 degrees in 360 degrees
    If the distance sensed is larger than 100 pixels
        Place node in front of wall.

These nodes are stored as a tree with a struct representing each node. Each node contains its pixel coordinate and stores an array of nodes which it can move to (Hii, J.Y., et all, 2021). Rather than the robot reacting once it’s near collision, it stores a network of nodes and plans out the possible paths. Each node tracks if it’s a dead end based on whether any future nodes can be formed from it. The robot stores the node it’s currently on and this allows the robot to backtrack to the nodes with valid future paths and attempt to find the goal at another future node. This performs a recursive, depth-first search through the junctions of the map (Appendix F). By default the robot prioritises the paths to go first in clockwise direction. This is a variation of the right-hand rule of the maze-navigating algorithm, which guarantees a solution to every maze (Memim, n.d.). 


Create Tree data structure with nodes,

node:
stores x and y coordinate
stores all the future nodes seen from this node
Stores the node before this in case the robot need to back-track
Stores whether the node is a dead end
Stores whether future nodes had already been scanned at this node

The motion of the robot is that it accelerates halfway between two nodes before decelerating. The robot comes to a complete stop at the destination node. After it decides which node to go to next, the robot orients its angle to the next angle and travels to the new node in the same way.

When the robot does reach a dead end, it performs a 360 degree turn to scan for where the detected distance is 0 (the default undefined value) and recognises this distance as the goal. The robot then orients itself and completes the maze.
Limitations
Even though the algorithm which places nodes works for simple mazes, it’s unable to adapt to the more complex corners and openings with its use of a constant threshold distance value. This method of placing nodes is not complex enough for placing nodes in the complex mazes. If the prior approach of detecting corners could be improved and remove randomly placed nodes, navigation could be improved as corners can be any distance away and be detected.
Since the arduino has limited processing power, it can send this data over bluetooth or to another on board device to do more complex processing of the data of all 360 degrees. Future implementation could use Bluetooth to transmit data collected by the sensor on the robot to a more powerful computer, which is capable of processing data collected from the whole 360 degrees range, and allow for a more sophisticated control of the robot. 

If this was a real physical robot, the angle of the measurements and the timing before the sensor is able to read in the data will have to be calibrated as there will be timing inconsistencies as the sensor spins around. In the simulation we assume we know the position of the robot, however in a physical environment, the real position of the robot will drift from the calculated position of the robot as the servos can’t be perfectly simulated and kept track of. This can be compensated for with the internal map of the robot and is a key advantage over other robots.
