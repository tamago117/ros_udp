#include <ros/ros.h>
#include <string>
#include <vector>
#include <std_msgs/Float32MultiArray.h>
#include <iostream>
#include "simple_udp.h"
simple_udp udp0("192.168.253.2",9090);

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

int main(int argc, char **argv){
  udp0.udp_bind();
  ros::init(argc, argv, "UDPreceive");
  ros::NodeHandle nh;
  ros::Rate loop_rate(10);
  ros::Publisher pub= nh.advertise<std_msgs::Float32MultiArray>("receivedUDP",10);

  while (ros::ok())
  {
    std_msgs::Float32MultiArray array;
    std::string rdata = udp0.udp_recv();
    if(rdata=="timeout"){
      return 0;
    }
    std::vector<std::string> ary = split(rdata, ",");

    //string->float
    for(int i=0; i<ary.size(); i++){
      array.data.resize(ary.size());
      array.data[i] = std::stof(ary[i]);
      //ROS_INFO("receive: %f", array.data[i]);
    }
    std::cout<<" "<<std::endl;

    pub.publish(array);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
