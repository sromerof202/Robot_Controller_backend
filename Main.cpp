#include "crow_all.h"//c++ webserver 

#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "JAKAZuRobot.h" //jaka header file
#include "Framework.h"// our unique header file (Declaration/implementation separate)


constexpr double PI = 3.1415926;
constexpr char ipaddr[] = "10.5.5.100";


int main() {

    int digitalOutputIndex1 = 1;

    int digitalOutputIndex2 = 0;

    useDigitalOutputs(ipaddr, digitalOutputIndex1, digitalOutputIndex2);

    printDigitalOutputStatus(ipaddr, digitalOutputIndex1, digitalOutputIndex2);

  

return 0;
}








/*

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/status")
        ([]() {
        //check status
        get_robot_status(ipaddr);
        return crow::response(200);
            });

    CROW_ROUTE(app, "/joint_move")
        ([]() {
        //joint move
        JointValue joint_pos = {
            toRadians(45),
            toRadians(50),
            toRadians(50),
            toRadians(0),
            toRadians(45),
            toRadians(0) };
        joint_move(ipaddr, &joint_pos, ABS, TRUE, 0.9);
        return crow::response(200);
            });

    CROW_ROUTE(app, "/linear_move")
        ([]() {
        //linear move
        CartesianPose cart;
        cart.tran.x = 345;
        cart.tran.y = -77;
        cart.tran.z = 450;
        cart.rpy.rx = toRadians(180);
        cart.rpy.ry = toRadians(0);
        cart.rpy.rz = toRadians(0);
        linear_move(cart, ipaddr, ABS, FALSE, 200, 100, 1, NULL);
        return crow::response(200);
            });

    app.port(8080).multithreaded().run();
}

*/