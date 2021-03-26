#include <ros/ros.h>
#include <string>
#include <vector>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32MultiArray.h>
#include <iostream>
#include "simple_udp.h"

template<class T> std::vector<std::string> split(const std::string& s, const T& separator, bool ignore_empty = 0, bool split_empty = 0) {
  struct {
    auto len(const std::string&             s) { return s.length(); }
    auto len(const std::string::value_type* p) { return p ? std::char_traits<std::string::value_type>::length(p) : 0; }
    auto len(const std::string::value_type  c) { return c == std::string::value_type() ? 0 : 1; /*return 1;*/ }
  } util;
  
  if (s.empty()) { /// empty string ///
    if (!split_empty || util.len(separator)) return {""};
    return {};
  }
  
  auto v = std::vector<std::string>();
  auto n = static_cast<std::string::size_type>(util.len(separator));
  if (n == 0) {    /// empty separator ///
    if (!split_empty) return {s};
    for (auto&& c : s) v.emplace_back(1, c);
    return v;
  }
  
  auto p = std::string::size_type(0);
  while (1) {      /// split with separator ///
    auto pos = s.find(separator, p);
    if (pos == std::string::npos) {
      if (ignore_empty && p - n + 1 == s.size()) break;
      v.emplace_back(s.begin() + p, s.end());
      break;
    }
    if (!ignore_empty || p != pos)
      v.emplace_back(s.begin() + p, s.begin() + pos);
    p = pos + n;
  }
  return v;
}

int byte_toInt(char bytearr[4]){
  return (int) (
            ((((int32_t)(bytearr[0])) << 24) & 0xFF000000)
          | ((((int32_t)(bytearr[1])) << 16) & 0x00FF0000)
          | ((((int32_t)(bytearr[2])) <<  8) & 0x0000FF00)
          | ((((int32_t)(bytearr[3])) <<  0) & 0x000000FF));
}

float byte_toFloat(char bytearr[4]){
  return (float) (
            ((((int32_t)(bytearr[0])) << 24) & 0xFF000000)
          | ((((int32_t)(bytearr[1])) << 16) & 0x00FF0000)
          | ((((int32_t)(bytearr[2])) <<  8) & 0x0000FF00)
          | ((((int32_t)(bytearr[3])) <<  0) & 0x000000FF));
}

int main(int argc, char **argv){
    std::string IPadress = "192.168.1.177";
    int portNumber = 8888;

    ros::init(argc, argv, "UDPreceive");
    ros::NodeHandle nh;
    ros::NodeHandle pnh("~");
    ros::Publisher pub_int = nh.advertise<std_msgs::Float32MultiArray>("receivedUDP_i",10);
    ros::Publisher pub_float = nh.advertise<std_msgs::Int32MultiArray>("receiveUDP_f", 10);
    pnh.getParam("ReceiverIP", IPadress);
    pnh.getParam("ReceiverPort", portNumber);
    std_msgs::Int32MultiArray array_int;
    std_msgs::Float32MultiArray array_float;

    while (ros::ok())
    {
        array_int.data.clear();
        array_float.data.clear();
        simple_udp udp0(IPadress, portNumber);
        udp0.udp_bind();
        std::string rdata = udp0.udp_recv();
        if(rdata=="timeout"){
            return 0;
        }
        std::cout<<rdata<<std::endl;

        int iNumber = (int8_t)rdata[0];
        int fNumber = (int8_t)rdata[1];
        array_int.data.resize(iNumber*4);
        array_float.data.resize(fNumber*4);
        //int
        for(int i=0; i<iNumber; i++){
            char Byte[4];
            for(int j=0; j<4; j++){
                Byte[j] = rdata[i*4+j+2];
            }
            array_int.data.push_back(byte_toInt(Byte));
        }
        //float
        for(int i=iNumber; i<iNumber+fNumber; i++){
            char Byte[4];
            for(int j=0; j<4; j++){
                Byte[j] = rdata[i*4+j+2];
            }
            array_float.data.push_back(byte_toInt(Byte));
        }

        pub_int.publish(array_int);
        pub_float.publish(array_float);
        ros::spinOnce();
    }
    return 0;
}
