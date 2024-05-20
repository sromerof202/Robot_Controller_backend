/*#include <iostream>
#include <windows.h>

#include "JAKAZuRobot.h"
#include "Framework.h"

constexpr double PI = 3.1415926;
constexpr char ipaddr[] = "192.168.0.121";


int main()
{
    //check status
    get_robot_status(ipaddr);

    //joint move
    JointValue joint_pos = {
        toRadians(45),
        toRadians(50),
        toRadians(50),
        toRadians(0),
        toRadians(45),
        toRadians(0) };
    joint_move(ipaddr, &joint_pos, ABS, TRUE, 0.6);


    //linear move
    CartesianPose cart;
    cart.tran.x = 345;
    cart.tran.y = -77;
    cart.tran.z = 450;
    cart.rpy.rx = toRadians(180);
    cart.rpy.ry = toRadians(0);
    cart.rpy.rz = toRadians(0);
    linear_move(cart, ipaddr, ABS, FALSE, 200, 10, 1, NULL);


    return 0;
}
*/