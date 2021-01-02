#include <ros/ros.h>
#include <stdio.h>
#include <std_msgs/Float32MultiArray.h>
#include <string>
#include <vector>
#include "simple_udp.h"
simple_udp udp0("192.168.253.1",9080);

int main(int argc, char **argv){
  ros::init(argc, argv, "sent");
  ros::NodeHandle nh;
  ros::Rate loop_rate(10);
  
  while(ros::ok()){
      udp0.udp_send("0.5");
      ros::spinOnce();
      loop_rate.sleep();
  }
  return 0;
}
