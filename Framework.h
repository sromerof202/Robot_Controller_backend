#pragma once
#include <string>
#include <JAKAZuRobot.h>
using namespace std;


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

void useDigitalOutputs(const char* ipaddr, int outputIndex1, int outputIndex2);