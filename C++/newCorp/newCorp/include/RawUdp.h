#ifndef RAWUDP_H
#define RAWUDP_H

#include<stdio.h>
#include<string.h> //memset
#include<sys/socket.h>    //for socket ofcourse
#include<stdlib.h> //for exit(0);
#include<errno.h> //For errno - the error number
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<arpa/inet.h>

#define BUFFERSIZE 2048
typedef unsigned short (*CSumFuncPtr)(unsigned short*, int);
 
class RawUdp
{
private:
/* 
    96 bit (12 bytes) pseudo header needed for udp header checksum calculation 
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t udp_length;
};
 
    char datagram[BUFFERSIZE], pseudogram[BUFFERSIZE];
    u_int32_t source_ip, dest_ip;
    u_int16_t source_port, dest_port;

public:
    RawUdp(u_int32_t destIp, u_int16_t destPort);
    void buildRawUdpDataGram(u_int32_t sourceIp, u_int16_t sourcePort, const u_char* payload, int payload_len, u_char* builtDG, int& builtDG_len, CSumFuncPtr fp);
    ~RawUdp();
};

#endif
