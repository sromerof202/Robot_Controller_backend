#include "crow_all.h" // C++ webserver
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "JAKAZuRobot.h" // JAKA header file
#include "Framework.h" // Our unique header file (Declaration/implementation separate)

constexpr double PI = 3.1415926;
constexpr char ipaddr[] = "10.5.5.100";

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/status")
        ([]() {
        // Check status
        get_robot_status(ipaddr);
        return crow::response(200);
            });

    CROW_ROUTE(app, "/joint_move")
        ([]() {
        // Joint move
        JointValue joint_pos = {
            toRadians(45),
            toRadians(50),
            toRadians(50),
            toRadians(0),
            toRadians(45),
            toRadians(0)
        };
        joint_move(ipaddr, &joint_pos, ABS, TRUE, 0.9);
        return crow::response(200);
            });

    CROW_ROUTE(app, "/linear_move")
        ([]() {
        // Linear move
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

    // New route for printing digital output status
    CROW_ROUTE(app, "/digital_output_status")
        ([]() {
        printDigitalOutputStatus(ipaddr, 0, 1);
        return crow::response(200, "Digital Output Status Printed");
            });

    // New route for using digital output 1
    CROW_ROUTE(app, "/use_digital_output1")
        ([]() {
        useDigitalOutput(ipaddr, 0);
        return crow::response(200, "Digital Output 1 Used");
            });

    // New route for using digital output 2
    CROW_ROUTE(app, "/use_digital_output2")
        ([]() {
        useDigitalOutput(ipaddr, 1);
        return crow::response(200, "Digital Output 2 Used");
            });

    app.port(8080).multithreaded().run();
}
