#ifndef SIMPLE_UDP
#define SIMPLE_UDP

#include <ros/ros.h>
#include <sys/types.h> //socket type
#include <sys/socket.h> //address domain
#include <sys/time.h>
#include <sys/types.h>
//#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h> //translate byte order
#include <unistd.h> //for close()
#include <string.h>
#include <iostream>

class simple_udp{
    int sock;
    struct sockaddr_in addr; //Structure for information of connection(ipv4)
    fd_set fds, readfds;
    /*struct timeval{
       long tv_sec;
       long tv_usec;
    };*/
    struct timeval tv;
public:
    //generate socket
    simple_udp(std::string address, int port){
        sock = socket(AF_INET, SOCK_DGRAM, 0); //Address domain, socket type, protocol
        addr.sin_family = AF_INET; //address family(ipv4)
        addr.sin_addr.s_addr = inet_addr(address.c_str()); //IP adress,inet_addr() : translate address
        addr.sin_port = htons(port); //port number
        tv.tv_sec = 1;
        tv.tv_usec = 0;
    }
    void udp_send(std::string word){
        sendto(sock, word.c_str(), word.length(), 0, (struct sockaddr *)&addr, sizeof(addr));
    }
    //regist socket
    void udp_bind(){
        bind(sock, (const struct sockaddr *)&addr, sizeof(addr)); //socket, address pointer, address size
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

    }
    std::string udp_recv(){
        #define BUFFER_MAX 200
        char buf[BUFFER_MAX]; //storing received data
        memcpy(&fds, &readfds, sizeof(fd_set));
        
        int n = select(sock+1, &fds, NULL, NULL, &tv);
        // timeout
        if (n == 0) {
		    ROS_ERROR("timeout");
            return "timeout";
	    }
        //receive
        if (FD_ISSET(sock, &fds)) {
            memset(buf, 0, sizeof(buf)); //initialize
            recv(sock, buf, sizeof(buf), 0); //receive
        }
        return std::string(buf);
    }
    void udp_recv(char *buf, int size){
        memset(buf, 0, size);
        recv(sock, buf, size, 0);
    }

    ~simple_udp(){
        close(sock); //socket close
    }
};

#endif