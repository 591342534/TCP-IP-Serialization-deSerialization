#include "PackUdpSegment.h"

PackUdpSegment::PackUdpSegment()
{
    ipHeader.SourceIpAddr=0;
    ipHeader.DestinationIpAddr=0;
    ipHeader.SegmentLength=0;
    ipHeader.Protocol=0x0011;

    header.SourcePort = 0;
    header.DestinationPort = 0;

    header.CheckSum = 0;


    DataLength = 0;
    header.SegmentLength = 0;

    data = (unsigned char*) malloc(MaxDataLength);
    segment = (unsigned char*) malloc(MaxSegmentLength);
}

int PackUdpSegment::setIpSourceAddr(const char *addr)
{
    ipHeader.SourceIpAddr=(inet_addr(addr));
    return 1;
}

int PackUdpSegment::setIpDestinationAddr(const char *addr)
{
    ipHeader.DestinationIpAddr=(inet_addr(addr));
    return 1;
}

int PackUdpSegment::setSourcePort(int port)
{
    //确保端口号有效
    if (port < 65536 && port >= 0)
    {
        header.SourcePort = port;
        return 0;
    }
    return -1;
}

int PackUdpSegment::setDestinationPort(int port)
{
    //确保端口号有效
    if (port < 65536 && port >= 0)
    {
        header.DestinationPort = port;
        return 1;
    }
    return -1;
}

int PackUdpSegment::setData(unsigned char *d, int len)
{
    if (len > MaxDataLength)
    {
        return -1;
    }
    if (len % 2 == 1)
    {
        DataLength = len + 1;
        memcpy(data, d, len);
        char temp = 0;
        memcpy(data + len, &temp, 1);
    }
    else
    {
        DataLength = len;
        memcpy(data, d, len);
    }
    return 1;
}

int PackUdpSegment::genSegment()
{

    header.SegmentLength=UdpHeaderLength+DataLength;
    SegmentLength=header.SegmentLength;
    header_hton();
    ipHeader.SegmentLength=(header.SegmentLength);

    memcpy(segment,&header,UdpHeaderLength);//copy header
    memcpy(segment+UdpHeaderLength,data,DataLength);

    genCheckSum();
    return 1;
}

int PackUdpSegment::genCheckSum()
{
    unsigned int cs=0;
    unsigned short * p=(unsigned short*) &ipHeader;
    //ip header校验和
    for(int i=0; i<6; i++)
    {
        cs+=(*(p+i));
        while((cs>>16)>0)
        {
            unsigned short temp=cs>>16;
            cs&=0x0000FFFF;
            cs+=temp;
        }
    }
    p=(unsigned short*)segment;
    for(int i=0; i<SegmentLength/2; i++)
    {
        cs+=(*(p+i));
        while((cs>>16)>0)
        {
            unsigned short temp=cs>>16;
            cs&=0x0000FFFF;
            cs+=temp;
        }
    }
    header.CheckSum=(unsigned short)cs^0xFFFF;
    memcpy(segment+6,&header.CheckSum,2);
    return 1;
}

int PackUdpSegment::writeSegment()
{
    std::fstream segmentFile("./udp_segment.txt",std::ios::out);
    unsigned short *output=(unsigned short *)segment;
    for(int i=0; i<header.SegmentLength/2; i++)
    {
        segmentFile<<std::hex<<(*(output+i))<<"  ";
        if((i+1)%8==0)
            segmentFile<<std::endl;
    }
    segmentFile.close();
    return 1;
}

unsigned char * PackUdpSegment::getSegment()
{
    return segment;
}

int PackUdpSegment::getSegmentLength()
{
    return SegmentLength;
}

void PackUdpSegment::header_hton()
{
    ipHeader.Protocol=htons(ipHeader.Protocol);
    header.SourcePort=htons(header.SourcePort);
    header.DestinationPort=htons(header.DestinationPort);
    header.SegmentLength=htons(header.SegmentLength);
}

void PackUdpSegment::header_ntoh()
{
    header.SourcePort=ntohs(header.SourcePort);
    header.DestinationPort=ntohs(header.DestinationPort);
    header.SegmentLength=ntohs(header.SegmentLength);
}


void PackUdpSegment::parseSegment(unsigned char *d,int Len)
{
    memcpy(&header,d,UdpHeaderLength);
    d+=UdpHeaderLength;
    header_ntoh();
    DataLength=Len-UdpHeaderLength;
    memcpy(data,d,DataLength);


    std::fstream dataFile("./app_data_udp.txt",std::ios::out);
    unsigned char *output=(unsigned char *)data;
    for(int i=0; i<DataLength; i++)
    {
        dataFile<<(*(output+i));

    }
    dataFile.close();
}

unsigned char * PackUdpSegment::getAppData()
{
    return data;
}

int PackUdpSegment::getDataLength()
{
    return DataLength;
}




