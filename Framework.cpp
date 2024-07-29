#include <iostream>
#include <vector>
#include <JAKAZuRobot.h>
#include <windows.h>
#include <thread>
#include <chrono>

constexpr double PI = 3.1415926;
using namespace std;

struct RobotSession {
	JointValue joint_position;
	BOOL digital_output1;
	BOOL digital_output2;
};

vector<RobotSession> sessionStorage;

double toRadians(double degrees) {
	return degrees * PI / 180.0;
}

errno_t login_in(const char* ipaddr)
{
	JAKAZuRobot robotController;
	errno_t ret = robotController.login_in(ipaddr);
	return ret;
}

errno_t login_out()
{
	JAKAZuRobot robotController;
	errno_t ret = robotController.login_out();
	return ret;
}

errno_t power_on()
{
	JAKAZuRobot robotController;
	errno_t ret = robotController.power_on();
	return ret;
}

errno_t power_off()
{
	JAKAZuRobot robotController;
	errno_t ret = robotController.power_off();
	return ret;
}

errno_t enable_robot()
{
	JAKAZuRobot robotController;
	errno_t ret = robotController.enable_robot();
	return ret;
}

errno_t disable_robot(const char* ipaddr)
{
	JAKAZuRobot robotController;
	errno_t ret;

	ret = robotController.login_in(ipaddr);
	if (ret != ERR_SUCC) {
		std::cout << "Login failed with error: " << ret << std::endl;
		return ret;
	}

	ret = robotController.power_on();
	if (ret != ERR_SUCC) {
		std::cout << "Power on failed with error: " << ret << std::endl;
		robotController.login_out(); // Log out before returning
		return ret;
	}

	ret = robotController.enable_robot();
	if (ret != ERR_SUCC) {
		std::cout << "Enable robot failed with error: " << ret << std::endl;
		robotController.login_out(); // Log out before returning
		return ret;
	}

	ret = robotController.disable_robot();
	if (ret != ERR_SUCC) {
		std::cout << "Disable robot failed with error: " << ret << std::endl;
	}

	robotController.login_out();

	return ret;
}


void checklogin(int ret, const string& ipaddr)
{
    if (ret == 0)
    {
        printf("\nlogin robot:%s success\n", ipaddr.c_str());
		
    }
    else
    {
        printf("\nlogin failed ,please check ip addr\n");
		
    }
}

//const JointValue* joint_pos is a pointer to a JointValue object. The joint_pos->jVal[i] syntax is used to access the jVal member of the JointValue object that joint_pos points to.
//using a pointer to JointValue allows the function to access and potentially modify the original JointValue object without making a copy of it. being more efficient and flexible.
errno_t joint_move(const std::string& ipaddr, const JointValue* joint_pos, MoveMode move_mode, BOOL is_block, double speed)
{
	errno_t ret = 1;

	JAKAZuRobot robotController;

	ret = robotController.login_in(ipaddr.c_str());
	checklogin(ret, ipaddr);

	robotController.power_on();

	robotController.enable_robot();
	printf("start move\n");

	for (int i = 0; i < 6; i++) {
		printf("Joint %d position: %f\n", i, joint_pos->jVal[i] * (180.0 / PI));
	}

	ret = robotController.joint_move(joint_pos, move_mode, is_block, speed);
	printf("ret=%d\n", ret);
	return ret;
}


int linear_move(const CartesianPose& cart, const char* ipaddr, MoveMode move_mode, BOOL is_block, double speed, double acc, double jerk, const char* tcp)
{
    int ret;
    
    JAKAZuRobot robotController;
    RobotStatus robstatus;
    
    ret = robotController.login_in(ipaddr);
    
    checklogin(ret, ipaddr);
    
    robotController.power_on();
    
    robotController.enable_robot();

    printf("x=%f, y=%f, z=%f, rx=%f, ry=%f, rz=%f\n", cart.tran.x, cart.tran.y, cart.tran.z, cart.rpy.rx * (180.0 / PI), cart.rpy.ry * (180.0 / PI), cart.rpy.rz * (180.0 / PI));
    
    OptionalCond* option_cond = NULL; // initialize this as needed
    ret = robotController.linear_move(&cart, move_mode, is_block, speed, acc, jerk, option_cond);
    
    printf("ret=%d", ret);

    robotController.login_out();
    return 0;
}

void printDigitalOutputStatus(const char* ipaddr, int digitalOutputIndex1, int digitalOutputIndex2) {
	BOOL digitalOutput1, digitalOutput2;
	errno_t err;

	JAKAZuRobot robotController;
	robotController.login_in(ipaddr);

	err = robotController.get_digital_output(IO_TOOL, digitalOutputIndex1, &digitalOutput1);
	err = robotController.get_digital_output(IO_TOOL, digitalOutputIndex2, &digitalOutput2);

	cout << "DO_1: " << (digitalOutput1 ? "ON" : "OFF") << endl;
	cout << "DO_2: " << (digitalOutput2 ? "ON" : "OFF") << endl;

	robotController.login_out();
}

void useDigitalOutput(const char* ipaddr, int outputIndex) {
	errno_t err;

	JAKAZuRobot robotController;

	robotController.login_in(ipaddr);

	robotController.power_on();

	robotController.enable_robot();

	// Turn on DO
	err = robotController.set_digital_output(IO_TOOL, outputIndex, TRUE);
	std::cout << "Error code for setting digital output 0: " << err << std::endl;

	// Wait for 2 seconds
	std::this_thread::sleep_for(std::chrono::seconds(2));

	// Turn off DO
	err = robotController.set_digital_output(IO_TOOL, outputIndex, FALSE);

	robotController.login_out();
}


errno_t save_robot_status_and_digital_output(const string& ipaddr, int digitalOutputIndex1, int digitalOutputIndex2) {
	JAKAZuRobot robotController;
	RobotStatus robstatus;
	BOOL digitalOutput1, digitalOutput2;
	errno_t ret;

	ret = robotController.login_in(ipaddr.c_str());
	if (ret != ERR_SUCC) return ret;

	robotController.power_on();
	robotController.enable_robot();

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

	robotController.login_out();
	return ERR_SUCC;
}

errno_t run_saved_movements(const string& ipaddr, int repeatCount, MoveMode move_mode, BOOL is_block, double speed) {
	JAKAZuRobot robotController;
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

int get_robot_status(const string& ipaddr)
{
	JAKAZuRobot robotController;
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
	cout << " enabled is :" << robstatus.errcode << endl;
	cout << " rapidrate is :" << robstatus.rapidrate << endl;
	cout << " protective_stop is :" << robstatus.protective_stop << endl;
	cout << " emergency_stop is :" << robstatus.emergency_stop << endl;
	cout << " rapidrate is :" << robstatus.rapidrate << endl;
	cout << " current_tool_id is :" << robstatus.current_tool_id << endl;
	cout << "tcp_pos is : x: " << robstatus.cartesiantran_position[0] << "  y: " << robstatus.cartesiantran_position[1] << "  z: " << robstatus.cartesiantran_position[2];
	cout << "rx: " << robstatus.cartesiantran_position[3] * (180.0 / PI) << "  ry: " << robstatus.cartesiantran_position[4] * (180.0 / PI) << "  rz: " << robstatus.cartesiantran_position[5] * (180.0 / PI) << endl;
	cout << " joint_pos is : {";
	for (int i = 0; i < (sizeof(robstatus.joint_position) / sizeof(robstatus.joint_position[0])) - 1; i++)
	{
		cout << robstatus.joint_position[i] * (180.0 / PI) << ",";
	}
	cout << robstatus.joint_position[sizeof(robstatus.joint_position) / sizeof(robstatus.joint_position[0]) - 1] << "}" << endl;
	cout << " on_soft_limit is :" << robstatus.on_soft_limit << endl;
	cout << " current_user_coordinate_id is :" << robstatus.current_user_id << endl;
	cout << " drag_status is :" << robstatus.drag_status << endl;
	cout << " is_socket_connect is :" << robstatus.is_socket_connect << endl;
	return 0;
}
