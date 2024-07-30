#pragma once
#include <string>
#include <JAKAZuRobot.h>
using namespace std;

errno_t login_in(const char* ipaddr);
errno_t login_out();
errno_t power_on(const char* ipaddr);
errno_t power_off(const char* ipaddr);
errno_t enable_robot(const char* ipaddr);
errno_t disable_robot(const char* ipaddr);
errno_t shut_down(const char* ipaddr);

double toRadians(double degrees); 

void checklogin(int ret, const string& ipaddr);

int get_robot_status(const string& ipaddr);

errno_t joint_move(
	const string& ipaddr,
	const JointValue *joint_pos,
	MoveMode move_mode, BOOL is_block,
	double speed
);
int linear_move(
	const CartesianPose& cart,
	const char* ipaddr,
	MoveMode move_mode,
	BOOL is_block,
	double speed,
	double acc,
	double jerk,
	const char* tcp
);

void printDigitalOutputStatus(
	const char* ipaddr,
	int digitalOutputIndex1, 
	int digitalOutputIndex2
);

void useDigitalOutput(const char* ipaddr, int outputIndex);

errno_t save_robot_status_and_digital_output(const string& ipaddr, int digitalOutputIndex1, int digitalOutputIndex2);

errno_t run_saved_movements(const string& ipaddr, int repeatCount, MoveMode move_mode, BOOL is_block, double speed);
