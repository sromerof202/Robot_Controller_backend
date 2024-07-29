#include "crow_all.h" // C++ webserver
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "JAKAZuRobot.h" // JAKA header file
#include "Framework.h" // Our unique header file (Declaration/implementation separate)

constexpr char ipaddr[] = "192.168.0.126";

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/login_in")
        ([]() {
        errno_t result = login_in(ipaddr);
        if (result == ERR_SUCC) {
            return crow::response(200, "Login successful");
        }
        else {
            return crow::response(500, "Login failed");
        }
            });

    CROW_ROUTE(app, "/login_out")
        ([]() {
        errno_t result = login_out();
        if (result == ERR_SUCC) {
            return crow::response(200, "Logout successful");
        }
        else {
            return crow::response(500, "Logout failed");
        }
            });

    CROW_ROUTE(app, "/power_on")
        ([]() {
        errno_t result = power_on();
        if (result == ERR_SUCC) {
            return crow::response(200, "Power on successful");
        }
        else {
            return crow::response(500, "Power on failed");
        }
            });

    CROW_ROUTE(app, "/power_off")
        ([]() {
        errno_t result = power_off();
        if (result == ERR_SUCC) {
            return crow::response(200, "Power off successful");
        }
        else {
            return crow::response(500, "Power off failed");
        }
            });

    CROW_ROUTE(app, "/enable_robot")
        ([]() {
        errno_t result = enable_robot();
        if (result == ERR_SUCC) {
            return crow::response(200, "Robot enabled successfully");
        }
        else {
            return crow::response(500, "Robot enable failed");
        }
            });

    CROW_ROUTE(app, "/disable_robot").methods(crow::HTTPMethod::Post)
        ([]() {
        errno_t result = disable_robot(ipaddr); // Assuming disable_robot now accepts an IP address
        if (result == ERR_SUCC) {
            return crow::response(200, "Robot disabled successfully");
        }
        else {
            return crow::response(500, "Robot disable failed");
        }
            });

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

    // New route for saving robot status and digital output status
    CROW_ROUTE(app, "/save_robot_status")
        ([]() {
        errno_t result = save_robot_status_and_digital_output(ipaddr, 0, 1);
        if (result == ERR_SUCC) {
            return crow::response(200, "Robot status saved successfully");
        }
        else {
            return crow::response(500, "Failed to save robot status");
        }
            });

    // New route for running saved movements
    CROW_ROUTE(app, "/run_saved_movements/<int>")
        ([](int times) {
        errno_t result = run_saved_movements(ipaddr, times, ABS, TRUE, 0.9); // Example parameters
        if (result == ERR_SUCC) {
            return crow::response(200, "Saved movements executed successfully");
        }
        else {
            return crow::response(500, "Failed to execute saved movements");
        }
            });

             
    app.port(8080).multithreaded().run();
}