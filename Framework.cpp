#include <iostream>
#include <JAKAZuRobot.h>
#include <windows.h>
#include <thread>
#include <chrono>
constexpr double PI = 3.1415926;
using namespace std;

// convert degrees to radians
double toRadians(double degrees) {
    return degrees * PI / 180.0;
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
	cout << " dout is : {";
	for (int i = 1; i < (sizeof(robstatus.dout) / sizeof(robstatus.dout[0])); i++)
	{
		cout << robstatus.dout[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.dout[sizeof(robstatus.dout) / sizeof(robstatus.dout[0]) - 1] << "}" << endl;
	cout << " tio_dout is : {";
	for (int i = 1; i < (sizeof(robstatus.tio_dout) / sizeof(robstatus.tio_dout[0])); i++)
	{
		cout << robstatus.tio_dout[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.tio_dout[sizeof(robstatus.tio_dout) / sizeof(robstatus.tio_dout[0]) - 1] << "}" << endl;
	cout << " din is : {";
	for (int i = 1; i < (sizeof(robstatus.din) / sizeof(robstatus.din[0])); i++)
	{
		cout << robstatus.din[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.din[sizeof(robstatus.din) / sizeof(robstatus.din[0]) - 1] << "}" << endl;
	cout << " tio_din is : {";
	for (int i = 1; i < (sizeof(robstatus.tio_din) / sizeof(robstatus.tio_din[0])); i++)
	{
		cout << robstatus.tio_din[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.tio_din[sizeof(robstatus.tio_din) / sizeof(robstatus.tio_din[0]) - 1] << "}" << endl;
	cout << " ain is :\n {";
	for (int i = 1; i < (sizeof(robstatus.ain) / sizeof(robstatus.ain[0])); i++)
	{
		cout << robstatus.ain[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.ain[sizeof(robstatus.ain) / sizeof(robstatus.ain[0]) - 1] << "}" << endl;
	cout << " tio_ain is\n : {";
	for (int i = 1; i < (sizeof(robstatus.tio_ain) / sizeof(robstatus.tio_ain[0])); i++)
	{
		cout << robstatus.tio_ain[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.tio_ain[sizeof(robstatus.tio_ain) / sizeof(robstatus.tio_ain[0]) - 1] << "}" << endl;
	cout << " aout is : \n{";
	for (int i = 1; i < (sizeof(robstatus.aout) / sizeof(robstatus.aout[0])); i++)
	{
		cout << robstatus.aout[i] << ",";
		if (0 == i % 20)
			cout << endl;
	}
	cout << robstatus.aout[sizeof(robstatus.aout) / sizeof(robstatus.aout[0]) - 1] << "}" << endl;
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
	//robotController.disable_robot();
	//robotController.power_off();
	//robotController.login_out();
	return 0;
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

void useDigitalOutputs(const char* ipaddr, int outputIndex1, int outputIndex2) {
	errno_t err;

	JAKAZuRobot robotController;

	robotController.login_in(ipaddr);

	robotController.power_on();

	robotController.enable_robot();

	// Turn on DO_1
	err = robotController.set_digital_output(IO_TOOL, outputIndex1, TRUE);
	std::cout << "Error code for setting digital output 1: " << err << std::endl;

	// Wait for 2 seconds
	std::this_thread::sleep_for(std::chrono::seconds(2));

	// Turn off DO_1 before using DO_2
	err = robotController.set_digital_output(IO_TOOL, outputIndex1, FALSE);
	std::cout << "Error code for setting digital output 1: " << err << std::endl;

	// Wait for 2 seconds
	std::this_thread::sleep_for(std::chrono::seconds(2));

	// Turn on DO_2
	err = robotController.set_digital_output(IO_TOOL, outputIndex2, TRUE);
	std::cout << "Error code for setting digital output 2: " << err << std::endl;

	// Wait for 2 seconds
	std::this_thread::sleep_for(std::chrono::seconds(2));

	printDigitalOutputStatus(ipaddr, outputIndex1, outputIndex2); // Corrected here

	// Turn off DO_2 before using DO_1
	err = robotController.set_digital_output(IO_TOOL, outputIndex2, FALSE);
	std::cout << "Error code for setting digital output 2: " << err << std::endl;

	robotController.login_out();
}