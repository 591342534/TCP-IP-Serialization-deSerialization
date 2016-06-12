#ifndef PACKIPPACKET_H
#define PACKIPPACKET_H

#include "PackTcpSegment.h"
#include "PackUdpSegment.h"

struct Ipheader
{
    unsigned short Ver_HLen_ServiceType;//0-3 4-7 8-15
    unsigned short TotalLength;//header+data
    unsigned short Identification;//
    unsigned short Flag_FraOffset;//
    unsigned short TTL_Protocol;//8 8
    unsigned short HeaderChecksum;
    unsigned int IpSourceAddr;
    unsigned int IpDestinationAddr;
};


class PackIpPacket
{
public:
    PackIpPacket();
    ~PackIpPacket();
    int setVer(int ver);
    int genHeaderLength();
    int setServiceType(int type);
    int genTotalLength();
    int setIdetification(int id);
    int setDonnotFra(unsigned int fra);
    int setMoreFra(unsigned int fra);
    int setTTL(int ttl);
    int setProtocol(int pro_type);
    int genHeaderChecksum();
    int setIpSourceAddr(const char *addr);
    int setIpDestinationAddr(const char *addr);
    int setRecordRoute(int len,int p);
    int setStrictSrcRoute(char **addr,int num,int len,int p);
    int setLooseSrcRoute(char **addr,int num,int len,int p);
    int setTimetStamp(int len,int p,int flag,char **addr,int num);

    int genPacket();
    int setSegment(unsigned char *d,int len);

    int writePacket();
    void header_hton();
    void header_ntoh();

    void parsePacket(unsigned char * d,int Len);
    unsigned char *getSegment();
    unsigned char *getPacket();
    int getSegmentLength();
    int getPacketLength();
    int getProtocolType();

private:
    const static int MaxPacketLength=1500;
    const static int MaxSegmentLength=1480;
    const static int MaxOptionLength=40;
    const static int HeaderLength=20;

    struct Ipheader header;
    PackTcpSegment *tcp;
    PackUdpSegment *udp;
    char *options;
    unsigned char *segment;
    unsigned char *packet;
    int OptionLength;
    int SegmentLength;


};

#endif // PACKIPPACKET_H
