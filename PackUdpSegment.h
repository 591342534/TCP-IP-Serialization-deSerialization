#include "iostream"
#include "cmath"
#include "cstdlib"
#include "cstring"
#include <arpa/inet.h>
#include <fstream>

struct SegmentHeader
{
    //20 bytes header
    unsigned short SourcePort;
    unsigned short DestinationPort;
    unsigned short SegmentLength;
    unsigned short CheckSum;

};

struct PseudoIpHeader_UDP
{
    unsigned int SourceIpAddr;
    unsigned int DestinationIpAddr;
    unsigned short Protocol;
    unsigned short SegmentLength;
};

class PackUdpSegment
{
private:
    const static int MaxDataLength=1472;
    const static int MaxSegmentLength=1480;
    const static int UdpHeaderLength=8;

    //IP header
    struct PseudoIpHeader_UDP ipHeader;

    //standard header
    struct SegmentHeader header;

    int DataLength;//来自应用层的数据长度 or 解封后递交给上层的数据长度
    int SegmentLength;//封装后的报文长度

    //data
    unsigned char *data;

    //segment
    unsigned char *segment;

public:
    PackUdpSegment();
    ~PackUdpSegment();

    int setIpSourceAddr(const char *addr);
    int setIpDestinationAddr(const char *addr);

    int setSourcePort(int port);
    int setDestinationPort(int port);

    int setData(unsigned char *d,int len);

    int genSegment();

    int genCheckSum();

    int writeSegment();

    unsigned char *getSegment();
    int getSegmentLength();
    void header_hton();
    void header_ntoh();
    void parseSegment(unsigned char *d,int Len);
    unsigned char * getAppData();
    int getDataLength();
};
