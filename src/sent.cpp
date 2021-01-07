#include <ros/ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <string>
#include <vector>
#include "simple_udp.h"
simple_udp udp0("192.168.253.1",9070);

std::vector<float> send_data_array;
std::string send_data;
std::string separator = ",";

void callback(const std_msgs::Float32MultiArray& array)
{
  send_data_array = array.data;
  send_data = std::to_string(send_data_array[0]);
  //combine string data
  for(int i=1; i<send_data_array.size(); i++){
    send_data.append(separator + std::to_string(send_data_array[i]));
  }
    udp0.udp_send(send_data);
    //ROS_INFO("subscribe: %f", send_data_array.data);
}

int main(int argc, char **argv){
  ros::init(argc, argv, "sentUDP");
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe("sentUDP", 10, callback);
  
  ros::spin();
  
  return 0;
}
