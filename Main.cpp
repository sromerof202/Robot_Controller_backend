// Main.cpp
#include "crow_all.h" // C++ webserver
#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include "JAKAZuRobot.h" // JAKA header file
#include "Framework.h" // Our unique header file (Declaration/implementation separate)
#include "CORS.h"
#include <string>

std::string url_decode(const std::string& url);

int main() {
    crow::App<CORS> app;
 
    CROW_ROUTE(app, "/set_ip")
        .methods("POST"_method)
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessionID.empty()) {
            sessionID = generateSessionID();
        }

        auto ipaddr = req.body;
        std::cout << ipaddr;
        sessions[sessionID] = make_unique<APIPionoid>(ipaddr);

        crow::response res;
        setSessionCookie(res, sessionID);
        res.code = 200;
        res.body = "IP address set successfully";
        return res;
            });

    CROW_ROUTE(app, "/login_in")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        std::cout << "Session ID: " << sessionID << std::endl; // Debug print
        if (sessions.find(sessionID) == sessions.end()) {
            std::cout << "Session not found" << std::endl; // Debug print
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->login_in();
        if (result == ERR_SUCC) {
            return crow::response(200, "Login successful");
        }
        else {
            std::cout << "Login failed with error code: " << result << std::endl;
            return crow::response(500, "Login failed");
        }
            });

    CROW_ROUTE(app, "/login_out")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->login_out();
        if (result == ERR_SUCC) {
            return crow::response(200, "Logout successful");
        }
        else {
            return crow::response(500, "Logout failed");
        }
            });

    CROW_ROUTE(app, "/power_on")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->power_on();
        if (result == ERR_SUCC) {
            return crow::response(200, "Power on successful");
        }
        else {
            return crow::response(500, "Power on failed");
        }
            });

    CROW_ROUTE(app, "/power_off")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->power_off();
        if (result == ERR_SUCC) {
            return crow::response(200, "Power off successful");
        }
        else {
            return crow::response(500, "Power off failed");
        }
            });


    CROW_ROUTE(app, "/enable_robot")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->enable_robot();
        if (result == ERR_SUCC) {
            return crow::response(200, "Robot enabled successfully");
        }
        else {
            return crow::response(500, "Robot enable failed");
        }
            });

    CROW_ROUTE(app, "/disable_robot")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->disable_robot();
        if (result == ERR_SUCC) {
            return crow::response(200, "Robot disabled successfully");
        }
        else {
            return crow::response(500, "Robot disable failed");
        }
            });

    CROW_ROUTE(app, "/shut_down")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->shut_down();
        if (result == ERR_SUCC) {
            return crow::response(200, "Shutdown successful");
        }
        else {
            return crow::response(500, "Shutdown failed");
        }
            });

    CROW_ROUTE(app, "/digital_output_status")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        sessions[sessionID]->printDigitalOutputStatus(0, 1);
        return crow::response(200, "Digital Output Status Printed");
            });

    CROW_ROUTE(app, "/use_digital_output/<string>")
        ([](const crow::request& req, const std::string& doIndex) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        sessions[sessionID]->useDigitalOutput(stoi(doIndex) - 1);
        return crow::response(200, "Digital Output Used");
            });

    CROW_ROUTE(app, "/use_digital_input/<string>")
        ([](const crow::request& req, const std::string& diIndex) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        sessions[sessionID]->useDigitalInput(stoi(diIndex) - 1);
        return crow::response(200, "Digital input Used");
            });

    CROW_ROUTE(app, "/save_robot_status")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        crow::json::wvalue responseJson;
        errno_t result = sessions[sessionID]->save_robot_status_and_digital_output(0, 1, responseJson);


        if (result == ERR_SUCC) {
            /*return crow::response(200, "Robot status saved successfully");*/
            return crow::response(responseJson);
        }
        else {
            return crow::response(500, "Failed to save robot status");
        }
            });

    /*CROW_ROUTE(app, "/delete_robot_status")
        .methods("DELETE"_method)
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        
        std::string nameToDelete = req.body;
        std::cout << "Received name to delete: " << nameToDelete << std::endl;

       
        errno_t result = sessions[sessionID]->delete_robot_status(nameToDelete);

        if (result == ERR_SUCC) {
            return crow::response("Robot status deleted successfully");
        }
        else {
            return crow::response("Failed to delete robot status");
        }
            });*/
    CROW_ROUTE(app, "/delete_robot_status/<string>")
        ([](const crow::request& req, const std::string& encodedName) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        // URL decoding
        std::string decodedName = url_decode(encodedName);
        std::cout << "Received name to delete: " << decodedName << std::endl;

        errno_t result = sessions[sessionID]->delete_robot_status(decodedName);

        if (result == ERR_SUCC) {
            return crow::response("Robot status deleted successfully");
        }
        else {
            return crow::response("Failed to delete robot status");
        }
            });

    CROW_ROUTE(app, "/update_robot_status/<string>")
        .methods("PUT"_method)
        ([](const crow::request& req, const std::string& originalName) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        // URL decoding
        std::string decodedName = url_decode(originalName);

        // Parse request body as JSON using nlohmann::json
        json requestJson;
        try {
            requestJson = json::parse(req.body);
        }
        catch (const json::parse_error& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            return crow::response(400, "Invalid JSON");
        }

        errno_t result = sessions[sessionID]->update_robot_status(decodedName, requestJson);

        if (result == ERR_SUCC) {
            return crow::response(200, "Robot status updated successfully");
        }
        else {
            return crow::response(500, "Failed to update robot status");
        }
            });




    CROW_ROUTE(app, "/run_saved_movements/<int>")
        ([](const crow::request& req, int times) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        errno_t result = sessions[sessionID]->run_saved_movements(times, ABS, TRUE, 0.9); // Example parameters
        if (result == ERR_SUCC) {
            return crow::response(200, "Saved movements executed successfully");
        }
        else {
            return crow::response(500, "Failed to execute saved movements");
        }
            });

    app.port(8080).multithreaded().run();
}

std::string url_decode(const std::string& url) {
    std::string result;
    std::istringstream input(url);
    char c;

    while (input.get(c)) {
        if (c == '%') {
            int i;
            if (input.get(c) && std::isxdigit(c)) {
                i = std::isdigit(c) ? c - '0' : std::toupper(c) - 'A' + 10;
                i *= 16;

                if (input.get(c) && std::isxdigit(c)) {
                    i += std::isdigit(c) ? c - '0' : std::toupper(c) - 'A' + 10;
                    result += static_cast<char>(i);
                }
                else {
                    result += '%';
                    result += c;
                }
            }
            else {
                result += '%';
                result += c;
            }
        }
        else {
            result += c;
        }
    }

    return result;
}

/*    CROW_ROUTE(app, "/status")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        sessions[sessionID]->get_robot_status();
        return crow::response(200);
            });

    CROW_ROUTE(app, "/joint_move")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        JointValue joint_pos = {
            toRadians(45),
            toRadians(50),
            toRadians(50),
            toRadians(0),
            toRadians(45),
            toRadians(0)
        };
        sessions[sessionID]->joint_move(&joint_pos, ABS, TRUE, 0.9);
        return crow::response(200);
            });

    CROW_ROUTE(app, "/linear_move")
        ([](const crow::request& req) {
        auto sessionID = getSessionID(req);
        if (sessions.find(sessionID) == sessions.end()) {
            return crow::response(400, "Session not found");
        }

        CartesianPose cart;
        cart.tran.x = 345;
        cart.tran.y = -77;
        cart.tran.z = 450;
        cart.rpy.rx = toRadians(180);
        cart.rpy.ry = toRadians(0);
        cart.rpy.rz = toRadians(0);
        sessions[sessionID]->linear_move(cart, ABS, FALSE, 200, 100, 1, NULL);
        return crow::response(200);
            });*/