//
// Created by sanket on 10/10/22.
//

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include "sensor_msgs/Joy.h"
#include "std_msgs/UInt8.h"
#include <sstream>
#include "teleop_ctrl/teleop_ctrl.h"
#include "../../../../bebop_ws/devel/include/bebop_msgs/CommonCommonStateBatteryStateChanged.h"


xBoxController xBoxControllerStruct;
std::string robot = "/bebop";


void joyCallback(const sensor_msgs::JoyPtr& msg)
{

    //memcpy((void*)&xBoxControllerStruct.axis.lAxisHorizontal , (const void*) &msg->axes[0], sizeof(xBoxControllerStruct.axis));
    //memcpy((void*)&xBoxControllerStruct.buttons.button1 , (const void*) &msg->buttons[0], sizeof(xBoxControllerStruct.buttons));

    memcpy((void*)&xBoxControllerStruct.axis.leftStickHorizontal , (const void*) &msg->axes[0], sizeof(xBoxControllerStruct.axis));
    memcpy((void*)&xBoxControllerStruct.buttons.buttonA , (const void*) &msg->buttons[0], sizeof(xBoxControllerStruct.buttons));

#ifdef myDebug
    ROS_INFO("I heard: [%f] [%f] [%f] [%f] [%f] [%f] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d]", xBoxControllerStruct.axis.lAxisHorizontal,
                                                   xBoxControllerStruct.axis.lAxisVertical,
                                                   xBoxControllerStruct.axis.rAxisHorizontal,
                                                   xBoxControllerStruct.axis.rAxisVertical,
                                                   xBoxControllerStruct.axis.arrowHorizontal,
                                                   xBoxControllerStruct.axis.arrowVertical,
                                                   xBoxControllerStruct.buttons.button1,
                                                   xBoxControllerStruct.buttons.button2,
                                                   xBoxControllerStruct.buttons.button3,
                                                   xBoxControllerStruct.buttons.button4,
                                                   xBoxControllerStruct.buttons.button5,
                                                   xBoxControllerStruct.buttons.button6,
                                                   xBoxControllerStruct.buttons.button7,
                                                   xBoxControllerStruct.buttons.button8,
                                                   xBoxControllerStruct.buttons.button9,
                                                   xBoxControllerStruct.buttons.button10,
                                                   xBoxControllerStruct.buttons.lAxisClick,
                                                   xBoxControllerStruct.buttons.rAxisClick
                     );
#endif
}


//void turtleCallback(const turtlesim::PosePtr msg)
//{
//    ROS_INFO("Turtle is at %f %f Yaw: %f  with Linear Vel: %f and Angular vel : %f", msg->x, msg->y, msg->theta, msg->linear_velocity, msg->angular_velocity);
//}


void battCallback(const bebop_msgs::CommonCommonStateBatteryStateChanged::ConstPtr msg)
{
    //msg->percent;
    ROS_INFO("Battery Left: %d", msg->percent );
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "teleop_control");

    ros::NodeHandle n;
    std_msgs::Empty takeoff_land;
    ros::Subscriber joy_sub = n.subscribe("joy", 10, joyCallback);
    //ros::Subscriber pose_sub = n.subscribe(robot+"/pose", 10, turtleCallback);
    ros::Subscriber battery_sub = n.subscribe("/bebop/states/common/CommonState/BatteryStateChanged", 1, battCallback);
    ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>("/bebop/takeoff",10);
    ros::Publisher land_pub = n.advertise<std_msgs::Empty>("/bebop/land",10);
    ros::Publisher cmd_vel_pub = n.advertise<geometry_msgs::Twist>(robot+"/cmd_vel", 10);
    ros::Publisher flip_pub = n.advertise<std_msgs::UInt8>("/bebop/flip",1);

    geometry_msgs::Twist cmd_vel_msg;
    std_msgs::UInt8 flipForward, flipBackward, flipRight, flipLeft;
    flipForward.data = 0;
    flipBackward.data =1 ;
    flipRight.data =2;
    flipLeft.data = 3;

    ros::Rate loop_rate(100);
    int count = 0;
    while (ros::ok())
    {
        if(xBoxControllerStruct.buttons.buttonStart) { ROS_INFO("Taking OFF"); takeoff_pub.publish(takeoff_land); sleep(1);}
        if(xBoxControllerStruct.buttons.buttonBack)  { ROS_INFO("Landing"); land_pub.publish(takeoff_land); sleep(1);}
        if(xBoxControllerStruct.buttons.buttonY) {ROS_INFO("Flip Forward"); flip_pub.publish(flipForward); sleep(1);}
        if(xBoxControllerStruct.buttons.buttonA) {ROS_INFO("Flip Backward"); flip_pub.publish(flipBackward); sleep(1);}
        if(xBoxControllerStruct.buttons.buttonX) {ROS_INFO("Flip Left"); flip_pub.publish(flipLeft); sleep(1);}
        if(xBoxControllerStruct.buttons.buttonB) {ROS_INFO("Flip Right"); flip_pub.publish(flipRight); sleep(1);}

        cmd_vel_msg.linear.x = xBoxControllerStruct.axis.leftStickVertical;
        cmd_vel_msg.linear.y = xBoxControllerStruct.axis.leftStickHorizontal;
        cmd_vel_msg.linear.z = xBoxControllerStruct.axis.rightStickVertical;

        cmd_vel_msg.angular.x = 0;
        cmd_vel_msg.angular.y = 0;
        cmd_vel_msg.angular.z = xBoxControllerStruct.axis.rightStickHorizontal;

        cmd_vel_pub.publish(cmd_vel_msg);

        ros::spinOnce();

        loop_rate.sleep();
        //++count;
    }

return 0;
}

