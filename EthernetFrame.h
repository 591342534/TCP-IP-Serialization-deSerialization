#ifndef ETHERNETFRAME_H
#define ETHERNETFRAME_H

#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>

struct FrameHeader
{
    char SourceAddr[6];
    char DestinationAddr[6];
    unsigned short Len_Type;
};


class EthernetFrame
{
public:
    EthernetFrame();
    ~EthernetFrame();
    void parseFrame(unsigned char *d,int Len);
    unsigned char* getPacket();
    int getPacketLength();
    void setPacket(unsigned char *d,int len);
    void setMacSrc(char *src);
    void setMacDst(char *dst);
    void setProtocol(int porto_type);
    void genFrame();
    void writeFrame();

private:
    struct FrameHeader header;
    const static int HeaderLength=14;
    const static int MaxPacketLength=1500;//MTU on Ethernet
    const static int MaxFrameLength=1518;
    int PacketLength;
    int FrameLength;
    unsigned char *packet;
    unsigned char * frame;
};

#endif // ETHERNETFRAME_H
