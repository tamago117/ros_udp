#include <ros/ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <string>
#include <vector>
#include "simple_udp.h"

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
    //ROS_INFO("subscribe: %f", send_data_array.data);
}

int main(int argc, char **argv){
  std::string IPadress = "192.168.253.1";
  int portNumber = 9070;


  ros::init(argc, argv, "sentUDP");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");
  pnh.getParam("clientIP", IPadress);
  pnh.getParam("clientPort", portNumber);

  simple_udp udp0(IPadress, portNumber);
  ros::Subscriber sub = nh.subscribe("sentUDP", 10, callback);
  
  ros::Rate loop_rate(10);
  while(ros::ok()){
    udp0.udp_send(send_data);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
