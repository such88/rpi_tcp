#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/stat.h>


enum ack {OK,NOK,NA};
enum ack  packet_ACK = OK;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned char uchar;
