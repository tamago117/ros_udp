#include <ros/ros.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <std_msgs/Float32MultiArray.h>
#include <iostream>
#include "simple_udp.h"
simple_udp udp0("192.168.253.2",9090);

std::vector<string> split(string str, string separator){
  if (separator == "") return {str};
  std::vector<string> result;
  std::string tstr = str + separator;
  long l = tstr.length(), sl = separator.length();
  std::string::size_type pos = 0, prev = 0;

  for(; pos < 1 && (pos = tstr.find(separator, pos)) != string::npos; prev = (pos += sl)){
    result.emplace_back(tstr, prev, pos - prev);
  }

  return result;
}

int main(int argc, char **argv){
  udp0.udp_bind();
  ros::init(argc, argv, "UDPreceive");
  ros::NodeHandle nh;
  ros::Rate loop_rate(10);
  ros::Publisher pub= nh.advertise<std_msgs::Float32MultiArray>("array",10);

  while (ros::ok())
  {
    std_msgs::Float32MultiArray array;
    std::string rdata = udp0.udp_recv();
    std::vector<string> ary = split(rdata, ",");

    //string->float
    for(int i=0; i<ary.size(); i++){
      array.data[i] = std::stod(ary[i]);
    }

    ROS_INFO("receive: %s", rdata.c_str());

    pub.publish(array);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
