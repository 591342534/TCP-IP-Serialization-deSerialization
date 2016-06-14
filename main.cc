#include "PackIpPacket.h"
#include "EthernetFrame.h"
#include <stdio.h>

char src[20]="192.168.199.149";
char dst[20]="234.123.12.1";
unsigned char *raw_data;
unsigned char *frame_data;
int data_length;

PackTcpSegment *tcp=new PackTcpSegment();
PackUdpSegment *udp=new PackUdpSegment();
EthernetFrame *eth=new EthernetFrame();
PackIpPacket *ip=new PackIpPacket();

int main()
{
    void genTcpSegment();
    void genIpPacketFromTcp();
    void genUdpSegment();
    void genIpPacketFromUdp();

    raw_data=(unsigned char *)malloc(1518);
    frame_data=(unsigned char *)malloc(1518);
    std::ifstream fin;
    fin.open("raw_data.txt",std::ios::in);
    fin.seekg(0,std::ios::end);
    data_length=fin.tellg();
    fin.seekg(0,std::ios::beg);
    fin.read((char*)raw_data,data_length);
    fin.close();

    //生成源文件
    genUdpSegment();
    genIpPacketFromUdp();
    ip->writePacket();
    eth->setPacket(ip->getPacket(),ip->getPacketLength());
    eth->genFrame();
    eth->writeFrame();

    //解析源文件
    fin.open("eth_frame.txt",std::ios::in);
    fin.seekg(0,std::ios::end);
    data_length=fin.tellg();
    fin.seekg(0,std::ios::beg);
    fin.read((char*)frame_data,data_length);
    fin.close();
    eth->parseFrame(frame_data,data_length);
    ip->parsePacket(eth->getPacket(),eth->getPacketLength());
    if(ip->getProtocolType()==6)
    {
        tcp->parseSegment(ip->getSegment(),ip->getSegmentLength());
    }
    else if(ip->getProtocolType()==17)
    {
        udp->parseSegment(ip->getSegment(),ip->getSegmentLength());
    }
    return 0;
}

void genUdpSegment()
{
    udp->setIpSourceAddr(src);
    udp->setIpDestinationAddr(dst);
    udp->setSourcePort(20141);
    udp->setDestinationPort(20141);
    udp->setData(raw_data,data_length);
    udp->genSegment();
    //udp->writeSegment();
}

void genIpPacketFromUdp()
{
    ip->setIpSourceAddr(src);
    ip->setIpDestinationAddr(dst);
    ip->setVer(4);
    ip->setIdetification(0x55a9);
    ip->setTTL(255);
    ip->setProtocol(17);
    ip->setSegment(udp->getSegment(),udp->getSegmentLength());

    ip->genPacket();
    ip->writePacket();
}

void genTcpSegment()
{
    tcp->setIpSourceAddr(src);
    tcp->setIpDestinationAddr(dst);
    tcp->setSourcePort(53166);
    tcp->setDestinationPort(80);
    tcp->setSequnceNumber(0x998ee703);
    tcp->setAckNumber(0x73941717);
    tcp->setFIN(1);
    tcp->setACK(1);
    tcp->setWndSize(321);
    tcp->setTimeStamp(213600,3838757550);

    tcp->setData(raw_data,data_length);

    tcp->genHeaderLength();
    tcp->genSegment();
    tcp->writeSegment();
}

void genIpPacketFromTcp()
{
    ip->setIpSourceAddr(src);
    ip->setIpDestinationAddr(dst);
    ip->setVer(4);
    ip->setIdetification(14182);
    ip->setDonnotFra(1);
    ip->setTTL(64);
    ip->setProtocol(6);
    ip->setSegment(tcp->getSegment(),tcp->getSegmentLength());

    ip->genPacket();
    ip->writePacket();
}
