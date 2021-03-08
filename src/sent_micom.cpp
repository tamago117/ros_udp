#include <ros/ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <string>
#include <vector>
#include "simple_udp.h"

std::vector<int> send_data_i;
void intData_callback(const std_msgs::Int32MultiArray& int_message)
{
    send_data_i = int_message.data;
}

std::vector<float> send_data_f;
void floatData_callback(const std_msgs::Float32MultiArray& float_message)
{
    send_data_f = float_message.data;
}

std::string int_toString(int32_t Int){
    char Byte[4];
    Byte[0] = (char)((Int >> 24) /*& 0xFF*/);
    Byte[1] = (char)((Int >> 16) /*& 0xFF*/);
    Byte[2] = (char)((Int >>  8)/* & 0xFF*/);
    Byte[3] = (char)((Int >>  0)/* & 0xFF*/);

    return Byte;
}

//shift operator can't use float,double.
//So this problem is able to solve by union.
//union assign variables to one memory.
//on one memory : int32_t = float
std::string float_toString(int32_t Float){
    char Byte[4];
    Byte[0] = (char)((Float >> 24) /*& 0xFF*/);
    Byte[1] = (char)((Float >> 16) /*& 0xFF*/);
    Byte[2] = (char)((Float >>  8)/* & 0xFF*/);
    Byte[3] = (char)((Float >>  0)/* & 0xFF*/);

    return Byte;
}

int main(int argc, char **argv){
  std::string IPadress = "192.168.0.200";
  int portNumber = 9070;

  ros::init(argc, argv, "sentUDP");
  ros::NodeHandle nh;
  ros::NodeHandle pnh("~");
  pnh.getParam("clientIP", IPadress);
  pnh.getParam("clientPort", portNumber);
  simple_udp udp0(IPadress, portNumber);
  ros::Subscriber sub_i = nh.subscribe("sentUDP_i", 10, intData_callback);
  ros::Subscriber sub_f = nh.subscribe("sentUDP_f", 10, floatData_callback);

  std::string send_data;
  ros::Rate loop_rate(10);
  while(ros::ok()){
      send_data.clear();
      //int number
      send_data += std::to_string(send_data_i.size());
      //float number
      send_data += std::to_string(send_data_f.size());
      //int contents
      for(const auto& intData : send_data_i){
          send_data += int_toString(intData);
      }
      //float contents
      for(const auto& floatData : send_data_f){
          send_data += float_toString(floatData);
      }

      udp0.udp_send(send_data);
      ros::spinOnce();
      loop_rate.sleep();
  }
  return 0;
}
