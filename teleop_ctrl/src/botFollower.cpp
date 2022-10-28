//
// Created by sanket on 10/10/22.
//

#include "teleop_ctrl/botFollower.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h"
#include "sensor_msgs/Joy.h"
#include <sstream>
#include <tf/tf.h>
#include "../../../../bebop_ws/devel/include/bebop_msgs/CommonCommonStateBatteryStateChanged.h"

#include <tf/transform_datatypes.h>
#define PI 3.141592653589793238462


xBoxController xBoxControllerStruct;
std::string robot = "dog/";

geometry_msgs::Twist dogAttitude, droneAttitude;
geometry_msgs::Twist prevDogAttitude, prevDroneAttitude;
bebop_msgs::CommonCommonStateBatteryStateChanged percentBattery;

void joyCallback(const sensor_msgs::JoyPtr& msg) {
    memcpy((void *) &xBoxControllerStruct.axis.leftStickHorizontal, (const void *) &msg->axes[0],
           sizeof(xBoxControllerStruct.axis));
    memcpy((void *) &xBoxControllerStruct.buttons.buttonA, (const void *) &msg->buttons[0],
           sizeof(xBoxControllerStruct.buttons));
}

void battCallback(const bebop_msgs::CommonCommonStateBatteryStateChanged::ConstPtr msg)
{
    percentBattery.percent = msg->percent;
}

void getDogPoseCallback(const geometry_msgs::PoseStamped::ConstPtr msg)
{

    dogAttitude.linear.x = msg->pose.position.x;
    dogAttitude.linear.y = msg->pose.position.y - 1.0; // follow the dog at a distance
    dogAttitude.linear.z = msg->pose.position.z;

    tf::Quaternion q(
            msg->pose.orientation.x,
            msg->pose.orientation.y,
            msg->pose.orientation.z,
            msg->pose.orientation.w
            );
    tf::Matrix3x3 m(q);
    m.getRPY(dogAttitude.angular.x, dogAttitude.angular.y, dogAttitude.angular.z);
    dogAttitude.angular.z = dogAttitude.angular.z*(180/PI);
    //ROS_INFO(" Dog: X: %f Y: %f Z: %f  Roll: %f  Pitch: %f  Yaw: %f ", dogAttitude.linear.x, dogAttitude.linear.y, dogAttitude.linear.z,
    //         dogAttitude.angular.x, dogAttitude.angular.y, dogAttitude.angular.z);

}

void getDronePoseCallback(const geometry_msgs::PoseStamped::ConstPtr msg)
{
    droneAttitude.linear.x = msg->pose.position.x;
    droneAttitude.linear.y = msg->pose.position.y;
    droneAttitude.linear.z = msg->pose.position.z;

    tf::Quaternion q(
            msg->pose.orientation.x,
            msg->pose.orientation.y,
            msg->pose.orientation.z,
            msg->pose.orientation.w
    );
    tf::Matrix3x3 m(q);
    m.getRPY(droneAttitude.angular.x, droneAttitude.angular.y, droneAttitude.angular.z);
    droneAttitude.angular.z = droneAttitude.angular.z*(180/PI);
    //ROS_INFO(" Drone: X: %f Y: %f Z: %f  Roll: %f  Pitch: %f  Yaw: %f ", droneAttitude.linear.x, droneAttitude.linear.y, droneAttitude.linear.z,
     //        droneAttitude.angular.x, droneAttitude.angular.y, droneAttitude.angular.z);
}

inline float getErrorDistance(float desired, float present)
{
    return (desired-present);
}


inline float getVelocity(float present, float prev, float time)
{
    return ((present-prev)/time);
}

int main(int argc, char** argv) {
   ros::init(argc, argv, "botFollower");
   ros::NodeHandle n;
   // float kp = 0.25;
   // float kd = 0.3;
    float kp = 0.25;
    float kd = 0.3;
    ros::Subscriber joy_sub = n.subscribe("/joy", 100, joyCallback);
    ros::Subscriber battery_sub = n.subscribe("/bebop/states/common/CommonState/BatteryStateChanged", 1, battCallback);
    ros::Subscriber dog_pose_sub = n.subscribe("/dog/pose", 1000, getDogPoseCallback);
    ros::Subscriber drone_pose_sub = n.subscribe("/drone/pose", 1000, getDronePoseCallback);
    ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>("/bebop/takeoff", 100);
    ros::Publisher land_pub = n.advertise<std_msgs::Empty>("/bebop/land", 100);
    ros::Publisher cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/bebop/cmd_vel", 100);

    std_msgs::Empty takeoff_land;
    geometry_msgs::Twist cmd_vel_msg;
    ros::Rate loop_rate(100);
    float sendx, sendy, sendaz, timeDiff;
    float rTime = 0.00;
    float prevRTime = 0.00;
    while(ros::ok())
    {
        rTime+=0.01;
        if(xBoxControllerStruct.buttons.buttonStart)
        {
            takeoff_pub.publish(takeoff_land);
            ROS_INFO("Taking OFF");
        }

        if(xBoxControllerStruct.buttons.buttonBack)
        {
            land_pub.publish(takeoff_land);
            ROS_INFO("Landing");
        }
        timeDiff = rTime - prevRTime;
        sendx = -((kp * getErrorDistance(dogAttitude.linear.x, droneAttitude.linear.x)) - (kd * getVelocity(droneAttitude.linear.x, prevDroneAttitude.linear.x, timeDiff)));
        sendy = ((kp * getErrorDistance(dogAttitude.linear.y, droneAttitude.linear.y)) - (kd * getVelocity(droneAttitude.linear.y, prevDroneAttitude.linear.y, timeDiff)));
        sendaz = ((0.03 * getErrorDistance(dogAttitude.angular.z, droneAttitude.angular.z)) - ((0.01)* getVelocity(droneAttitude.angular.z, prevDroneAttitude.angular.z, timeDiff)));
        prevDogAttitude = dogAttitude;
        prevDroneAttitude = droneAttitude;
        prevRTime = rTime;
        cmd_vel_msg.linear.x = sendy;
        cmd_vel_msg.linear.y = sendx;
        cmd_vel_msg.linear.z = xBoxControllerStruct.axis.rightStickVertical;
        cmd_vel_msg.angular.x = 0.0;
        cmd_vel_msg.angular.y = 0.0;
        cmd_vel_msg.angular.z = sendaz;
        //ROS_INFO("Desired Velocity : %.3f\t%.3f\t %.3f\t %.3f\t", cmd_vel_msg.linear.x, cmd_vel_msg.linear.y, cmd_vel_msg.angular.z, timeDiff);
        ROS_INFO("Battery Percent %d\t " , percentBattery.percent);
        cmd_vel_pub.publish(cmd_vel_msg);
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}