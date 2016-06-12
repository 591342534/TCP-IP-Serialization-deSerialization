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
    unsigned short SegmentSize;
    unsigned short CheckSum;

};

struct PseudoIpHeader_UDP
{
    unsigned int SourceIpAddr;
    unsigned int DestinationIpAddr;
    unsigned int pro_segmentsize;
    const static unsigned short Protocol=0x0011;
    unsigned short SegmentSize;
};

class PackUdpSegment
{
private:
    const static int MaxDataLength=1472;
    const static int MaxSegmentSize=1480;
    const static int UdpHeaderLength=8;

    //IP header
    struct PseudoIpHeader_UDP ipHeader;

    //standard header
    struct SegmentHeader header;

    int DataLength;

    //data
    unsigned char *data;//dynamic allocate memory

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
