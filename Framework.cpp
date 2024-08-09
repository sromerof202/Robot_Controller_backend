// Framework.cpp
#include "Framework.h"
#include <iostream>
#include <random>
#include <crow_all.h>
#include <vector>
#include <JAKAZuRobot.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include "json.hpp"

using json = nlohmann::json;

constexpr double PI = 3.1415926;
using namespace std;

APIPionoid::APIPionoid(const string& ipaddr) : ipaddr(ipaddr) {}

struct RobotSession {
	JointValue joint_position;
	CartesianTran cartesian_position;
	Rpy rpy_position;
	BOOL digital_output1;
	BOOL digital_output2;
	std::string name; // add name
};

vector<RobotSession> sessionStorage;

int sessionCounter = 1;

double toRadians(double degrees) {
	return degrees * PI / 180.0;
}

errno_t APIPionoid::login_in() {
	return robotController.login_in(ipaddr.c_str());
}

errno_t APIPionoid::login_out() {
	return robotController.login_out();
}

errno_t APIPionoid::power_on() {
	errno_t loginResult = login_in();
	if (loginResult != ERR_SUCC) {
		return loginResult;
	}
	errno_t ret = robotController.power_on();
	login_out();
	return ret;
}

errno_t APIPionoid::power_off() {
	errno_t loginResult = login_in();
	if (loginResult != ERR_SUCC) {
		return loginResult;
	}
	errno_t ret = robotController.power_off();
	login_out();
	return ret;
}

errno_t APIPionoid::enable_robot() {
	errno_t loginResult = login_in();
	if (loginResult != ERR_SUCC) {
		return loginResult;
	}
	errno_t ret = robotController.enable_robot();
	login_out();
	return ret;
}

errno_t APIPionoid::disable_robot() {
	errno_t loginResult = login_in();
	if (loginResult != ERR_SUCC) {
		return loginResult;
	}
	errno_t ret = robotController.disable_robot();
	login_out();
	return ret;
}

errno_t APIPionoid::shut_down() {
	errno_t loginResult = login_in();
	if (loginResult != ERR_SUCC) {
		return loginResult;
	}
	errno_t ret = robotController.shut_down();
	login_out();
	return ret;
}

void checklogin(int ret, const string& ipaddr) {
	if (ret == 0) {
		printf("\nlogin robot:%s success\n", ipaddr.c_str());
	}
	else {
		printf("\nlogin failed, please check ip addr\n");
	}
}


errno_t APIPionoid::joint_move(const JointValue* joint_pos, MoveMode move_mode, BOOL is_block, double speed) {
	errno_t ret = login_in();
	if (ret != ERR_SUCC) {
		checklogin(ret, ipaddr);
		return ret;
	}

	robotController.power_on();
	robotController.enable_robot();
	printf("start move\n");

	for (int i = 0; i < 6; i++) {
		printf("Joint %d position: %f\n", i, joint_pos->jVal[i] * (180.0 / PI));
	}

	ret = robotController.joint_move(joint_pos, move_mode, is_block, speed);
	printf("ret=%d\n", ret);
	login_out();
	return ret;
}

int APIPionoid::linear_move(const CartesianPose& cart, MoveMode move_mode, BOOL is_block, double speed, double acc, double jerk, const char* tcp) {
	int ret = login_in();
	if (ret != ERR_SUCC) {
		checklogin(ret, ipaddr);
		return ret;
	}

	robotController.power_on();
	robotController.enable_robot();

	printf("x=%f, y=%f, z=%f, rx=%f, ry=%f, rz=%f\n", cart.tran.x, cart.tran.y, cart.tran.z, cart.rpy.rx * (180.0 / PI), cart.rpy.ry * (180.0 / PI), cart.rpy.rz * (180.0 / PI));

	OptionalCond* option_cond = NULL; // initialize this as needed
	ret = robotController.linear_move(&cart, move_mode, is_block, speed, acc, jerk, option_cond);

	printf("ret=%d", ret);
	login_out();
	return ret;
}

void APIPionoid::printDigitalOutputStatus(int digitalOutputIndex1, int digitalOutputIndex2) {
	BOOL digitalOutput1, digitalOutput2;
	errno_t err = login_in();
	if (err != ERR_SUCC) {
		return;
	}

	err = robotController.get_digital_output(IO_TOOL, digitalOutputIndex1, &digitalOutput1);
	err = robotController.get_digital_output(IO_TOOL, digitalOutputIndex2, &digitalOutput2);

	cout << "DO_1: " << (digitalOutput1 ? "ON" : "OFF") << endl;
	cout << "DO_2: " << (digitalOutput2 ? "ON" : "OFF") << endl;

	login_out();
}

void APIPionoid::useDigitalOutput(int outputIndex) {
	errno_t err = login_in();
	if (err != ERR_SUCC) {
		return;
	}

	robotController.power_on();
	robotController.enable_robot();

	BOOL currentState;
	err = robotController.get_digital_output(IO_CABINET, outputIndex, &currentState);
	if (err != ERR_SUCC) {
		std::cout << "Error getting digital output state: " << err << std::endl;
		login_out();
		return;
	}

	BOOL newState = !currentState;
	err = robotController.set_digital_output(IO_CABINET, outputIndex, newState);
	if (err != ERR_SUCC) {
		std::cout << "Error setting digital output: " << err << std::endl;
	}
	else {
		std::cout << "Digital output " << outputIndex << " set to " << (newState ? "ON" : "OFF") << std::endl;
	}

	login_out();
}


errno_t APIPionoid::save_robot_status_and_digital_output(int digitalOutputIndex1, int digitalOutputIndex2, crow::json::wvalue& responseJson) {
	errno_t ret = login_in();
	if (ret != ERR_SUCC) {
		return ret;
	}

	robotController.power_on();
	robotController.enable_robot();

	RobotStatus robstatus;
	BOOL digitalOutput1, digitalOutput2;

	robotController.get_robot_status(&robstatus);
	robotController.get_digital_output(IO_CABINET, digitalOutputIndex1, &digitalOutput1);
	robotController.get_digital_output(IO_CABINET, digitalOutputIndex2, &digitalOutput2);

	RobotSession session;

	// CartesianTran
	session.cartesian_position.x = robstatus.cartesiantran_position[0];
	session.cartesian_position.y = robstatus.cartesiantran_position[1];
	session.cartesian_position.z = robstatus.cartesiantran_position[2];

	// Rpy (rad to deg)
	session.rpy_position.rx = robstatus.cartesiantran_position[3] * 180 / PI;
	session.rpy_position.ry = robstatus.cartesiantran_position[4] * 180 / PI;
	session.rpy_position.rz = robstatus.cartesiantran_position[5] * 180 / PI;


	for (int i = 0; i < 6; ++i) {
		session.joint_position.jVal[i] = robstatus.joint_position[i];
	}
	session.digital_output1 = digitalOutput1;
	session.digital_output2 = digitalOutput2;

	// Create a name for the new session
	/*std::string sessionName = "Move " + std::to_string(sessionStorage.size() + 1);*/
	std::string sessionName = "Move " + std::to_string(sessionCounter);
	session.name = sessionName;

	// Bundle data into JSON
	responseJson = crow::json::wvalue();
	responseJson["name"] = session.name;
	responseJson["x"] = session.cartesian_position.x;
	responseJson["y"] = session.cartesian_position.y;
	responseJson["z"] = session.cartesian_position.z;
	responseJson["rx"] = session.rpy_position.rx;
	responseJson["ry"] = session.rpy_position.ry;
	responseJson["rz"] = session.rpy_position.rz;

	sessionStorage.push_back(session);

	sessionCounter++;

	login_out();
	return ERR_SUCC;
}

errno_t APIPionoid::delete_robot_status(const std::string& nameToDelete) {
	errno_t ret = login_in();
	if (ret != ERR_SUCC) {
		return ret;
	}

	std::cout << "Deleting session with name: " << nameToDelete << std::endl;

	auto it = std::remove_if(sessionStorage.begin(), sessionStorage.end(),
		[&nameToDelete](const RobotSession& session) {
			return session.name == nameToDelete;
		});
	sessionStorage.erase(it, sessionStorage.end());

	std::cout << "Remaining sessions (" << sessionStorage.size() << "):" << std::endl;
	for (const auto& session : sessionStorage) {
		std::cout << "Name: " << session.name << std::endl;
		std::cout << "Cartesian Position: x=" << session.cartesian_position.x
			<< ", y=" << session.cartesian_position.y
			<< ", z=" << session.cartesian_position.z << std::endl;
		std::cout << "Rpy Position: rx=" << session.rpy_position.rx
			<< ", ry=" << session.rpy_position.ry
			<< ", rz=" << session.rpy_position.rz << std::endl;
	}

	login_out();
	return ERR_SUCC;
}

errno_t APIPionoid::update_robot_status(const std::string& originalName, const json& updatedSessionJson) {
	errno_t ret = login_in();
	if (ret != ERR_SUCC) {
		return ret;
	}

	// Convert JSON data to RobotSession object
	RobotSession updatedSession;
	try {
		updatedSession.name = updatedSessionJson.at("name").get<std::string>();
		updatedSession.cartesian_position.x = updatedSessionJson.at("x").get<double>();
		updatedSession.cartesian_position.y = updatedSessionJson.at("y").get<double>();
		updatedSession.cartesian_position.z = updatedSessionJson.at("z").get<double>();
		updatedSession.rpy_position.rx = updatedSessionJson.at("rx").get<double>();
		updatedSession.rpy_position.ry = updatedSessionJson.at("ry").get<double>();
		updatedSession.rpy_position.rz = updatedSessionJson.at("rz").get<double>();
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing JSON: " << e.what() << std::endl;
		return ERR_SUCC;
	}

	// Find data to modify in an existing session
	auto it = std::find_if(sessionStorage.begin(), sessionStorage.end(),
		[&originalName](const RobotSession& session) {
			return session.name == originalName;
		});

	if (it != sessionStorage.end()) {
		// Update existing session data
		*it = updatedSession;
	}
	else {
		// If there is no session, add a new session (if necessary)
		sessionStorage.push_back(updatedSession);
	}

	std::cout << "Updated sessions (" << sessionStorage.size() << "):" << std::endl;
	for (const auto& session : sessionStorage) {
		std::cout << "Name: " << session.name << std::endl;
		std::cout << "Cartesian Position: x=" << session.cartesian_position.x
			<< ", y=" << session.cartesian_position.y
			<< ", z=" << session.cartesian_position.z << std::endl;
		std::cout << "Rpy Position: rx=" << session.rpy_position.rx
			<< ", ry=" << session.rpy_position.ry
			<< ", rz=" << session.rpy_position.rz << std::endl;
	}

	login_out();
	return ERR_SUCC;
}


errno_t APIPionoid::run_saved_movements(int repeatCount, MoveMode move_mode, BOOL is_block, double speed) {
	errno_t ret;

	ret = robotController.login_in(ipaddr.c_str());
	if (ret != ERR_SUCC) return ret;

	robotController.power_on();
	robotController.enable_robot();

	for (int i = 0; i < repeatCount; ++i) {
		for (const auto& session : sessionStorage) {
			ret = robotController.joint_move(&session.joint_position, move_mode, is_block, speed);
			if (ret != ERR_SUCC) return ret;

			robotController.set_digital_output(IO_TOOL, 0, session.digital_output1);
			robotController.set_digital_output(IO_TOOL, 1, session.digital_output2);
		}
	}

	robotController.login_out();
	return ERR_SUCC;
}

int APIPionoid::get_robot_status() {
	RobotStatus robstatus;
	int ret;

	ret = robotController.login_in(ipaddr.c_str());
	cout << " ret :" << ret << endl;

	robotController.power_on();
	robotController.enable_robot();

	robotController.get_robot_status(&robstatus);
	cout << " errcode is :" << robstatus.errcode << endl;
	cout << " inpos is :" << robstatus.inpos << endl;
	cout << " powered_on is :" << robstatus.powered_on << endl;
	cout << " enabled is :" << robstatus.enabled << endl;
	cout << " rapidrate is :" << robstatus.rapidrate << endl;
	cout << " protective_stop is :" << robstatus.protective_stop << endl;
	cout << " emergency_stop is :" << robstatus.emergency_stop << endl;
	cout << " current_tool_id is :" << robstatus.current_tool_id << endl;
	cout << "tcp_pos is : x: " << robstatus.cartesiantran_position[0] << "  y: " << robstatus.cartesiantran_position[1] << "  z: " << robstatus.cartesiantran_position[2];
	cout << "rx: " << robstatus.cartesiantran_position[3] * (180.0 / PI) << "  ry: " << robstatus.cartesiantran_position[4] * (180.0 / PI) << "  rz: " << robstatus.cartesiantran_position[5] * (180.0 / PI) << endl;
	cout << " joint_pos is : {";
	for (int i = 0; i < (sizeof(robstatus.joint_position) / sizeof(robstatus.joint_position[0])) - 1; i++) {
		cout << robstatus.joint_position[i] * (180.0 / PI) << ",";
	}
	cout << robstatus.joint_position[sizeof(robstatus.joint_position) / sizeof(robstatus.joint_position[0]) - 1] << "}" << endl;
	cout << " on_soft_limit is :" << robstatus.on_soft_limit << endl;
	cout << " current_user_coordinate_id is :" << robstatus.current_user_id << endl;
	cout << " drag_status is :" << robstatus.drag_status << endl;
	cout << " is_socket_connect is :" << robstatus.is_socket_connect << endl;

	robotController.login_out();
	return 0;
}


unordered_map<string, unique_ptr<APIPionoid>> sessions;

string generateSessionID() {
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<> dis(0, 15);
	static const char* chars = "0123456789abcdef";

	string sessionID(32, '0');
	for (auto& c : sessionID) {
		c = chars[dis(gen)];
	}
	return sessionID;
}

void setSessionCookie(crow::response& res, const string& sessionID) {
	res.add_header("Set-Cookie", "session_id=" + sessionID + "; Path=/; HttpOnly, SameSite=none; Secure");
}

string getSessionID(const crow::request& req) {
	auto cookie = req.get_header_value("Cookie");
	auto pos = cookie.find("session_id=");
	if (pos != string::npos) {
		return cookie.substr(pos + 11, 32);
	}
	return "";
}
