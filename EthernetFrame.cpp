#include "EthernetFrame.h"


EthernetFrame::EthernetFrame()
{
    //ctor
    header.Len_Type=0;
    PacketLength=0;
    packet=(unsigned char *)malloc(MaxPacketLength);
    frame=(unsigned char *)malloc(MaxFrameLength);
}

EthernetFrame::~EthernetFrame()
{
    //dtor
}

void EthernetFrame::parseFrame(unsigned char *d,int Len)
{
    memcpy(&header,d,HeaderLength);
    PacketLength=Len-HeaderLength;//4-CRC
    d+=HeaderLength;
    memcpy(packet,d,PacketLength);
}

unsigned char * EthernetFrame::getPacket()
{
    return packet;
}

int EthernetFrame::getPacketLength()
{
    return PacketLength;
}

void EthernetFrame::setMacSrc(char *src)
{
    memcpy(header.SourceAddr,src,6);
}

void EthernetFrame::setMacDst(char *dst)
{
    memcpy(header.DestinationAddr,dst,6);
}

void EthernetFrame::setPacket(unsigned char * d,int len)
{
    memcpy(packet,d,len);
    PacketLength=len;
}

void EthernetFrame::genFrame()
{
    memcpy(frame,&header,HeaderLength);
    memcpy(frame+HeaderLength,packet,PacketLength);
}

void EthernetFrame::writeFrame()
{
    std::fstream frameFile("./eth_frame.txt",std::ios::out|std::ios::app);
    unsigned char *output=(unsigned char *)frame;
    for(int i=0; i<(FrameLength); i++)
    {
        frameFile<<(*(output+i));

    }
    frameFile.close();
}
