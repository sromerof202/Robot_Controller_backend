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

constexpr double PI = 3.1415926;
using namespace std;

APIPionoid::APIPionoid(const string& ipaddr) : ipaddr(ipaddr) {}

struct RobotSession {
	JointValue joint_position;
	BOOL digital_output1;
	BOOL digital_output2;
};

vector<RobotSession> sessionStorage;

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

	// Turn on DO
	err = robotController.set_digital_output(IO_TOOL, outputIndex, TRUE);
	std::cout << "Error code for setting digital output 0: " << err << std::endl;

	// Wait for 2 seconds
	std::this_thread::sleep_for(std::chrono::seconds(2));

	// Turn off DO
	err = robotController.set_digital_output(IO_TOOL, outputIndex, FALSE);

	login_out();
}


errno_t APIPionoid::save_robot_status_and_digital_output(int digitalOutputIndex1, int digitalOutputIndex2) {
	errno_t ret = login_in();
	if (ret != ERR_SUCC) {
		return ret;
	}

	robotController.power_on();
	robotController.enable_robot();

	RobotStatus robstatus;
	BOOL digitalOutput1, digitalOutput2;

	robotController.get_robot_status(&robstatus);
	robotController.get_digital_output(IO_TOOL, digitalOutputIndex1, &digitalOutput1);
	robotController.get_digital_output(IO_TOOL, digitalOutputIndex2, &digitalOutput2);

	RobotSession session;
	for (int i = 0; i < 6; ++i) {
		session.joint_position.jVal[i] = robstatus.joint_position[i];
	}
	session.digital_output1 = digitalOutput1;
	session.digital_output2 = digitalOutput2;

	sessionStorage.push_back(session);

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
	res.add_header("Set-Cookie", "session_id=" + sessionID + "; Path=/; HttpOnly");
}

string getSessionID(const crow::request& req) {
	auto cookie = req.get_header_value("Cookie");
	auto pos = cookie.find("session_id=");
	if (pos != string::npos) {
		return cookie.substr(pos + 11, 32);
	}
	return "";
}
