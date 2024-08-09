#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "crow_all.h" // Crow web framework
#include <JAKAZuRobot.h> 

#include "json.hpp"

using json = nlohmann::json;

using namespace std;

// Class to encapsulate API interactions with the Pionoid robot
class APIPionoid {
public:
    // Constructor to initialize with the robot's IP address
    APIPionoid(const string& ipaddr);

    // Methods to control the robot
    errno_t login_in(); 
    errno_t login_out(); 
    errno_t power_on(); 
    errno_t power_off(); 
    errno_t enable_robot(); 
    errno_t disable_robot(); 
    errno_t shut_down(); 

    
    errno_t joint_move(const JointValue* joint_pos, MoveMode move_mode, BOOL is_block, double speed); // Move robot joints
    int linear_move(const CartesianPose& cart, MoveMode move_mode, BOOL is_block, double speed, double acc, double jerk, const char* tcp); // Move robot linearly

    
    void printDigitalOutputStatus(int digitalOutputIndex1, int digitalOutputIndex2); // Print digital output status
    void useDigitalOutput(int outputIndex); // Use a specific digital output
    void useDigitalInput(int inputIndex);

    int get_robot_status();

    //New Methods
    errno_t save_robot_status_and_digital_output(int digitalOutputIndex1, int digitalOutputIndex2, crow::json::wvalue& responseJson); // Save robot status and digital output
    errno_t run_saved_movements(int repeatCount, MoveMode move_mode, BOOL is_block, double speed); // Run saved movements
    errno_t delete_robot_status(const std::string& nameToDelete);
    errno_t update_robot_status(const std::string& originalName, const json& updatedSessionJson);

    
private:
    string ipaddr; 
    JAKAZuRobot robotController; // Robot controller instance
};

// Session management functions
string generateSessionID(); // Function to generate a unique session ID
void setSessionCookie(crow::response& res, const string& sessionID); // Function to set a session cookie in the response
string getSessionID(const crow::request& req); // Function to get the session ID from the request

// Global map to manage sessions, mapping session IDs to APIPionoid instances
extern unordered_map<string, unique_ptr<APIPionoid>> sessions;

// Utility functions
double toRadians(double degrees); 
void checklogin(int ret, const string& ipaddr); 
