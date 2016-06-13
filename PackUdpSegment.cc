#include "PackUdpSegment.h"

PackUdpSegment::PackUdpSegment()
{
    ipHeader.SourceIpAddr=0;
    ipHeader.DestinationIpAddr=0;
    ipHeader.SegmentSize=0;

    header.SourcePort = 0;
    header.DestinationPort = 0;

    header.CheckSum = 0;


    DataLength = 0;
    header.SegmentSize = 0;

    data = (unsigned char*) malloc(MaxDataLength);
    segment = (unsigned char*) malloc(MaxSegmentSize);
}

int PackUdpSegment::setIpSourceAddr(const char *addr)
{
    ipHeader.SourceIpAddr=htonl(inet_addr(addr));
    return 1;
}

int PackUdpSegment::setIpDestinationAddr(const char *addr)
{
    ipHeader.DestinationIpAddr=htonl(inet_addr(addr));
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

    header.SegmentSize=UdpHeaderLength+DataLength;

    header_hton();
    genCheckSum();
    memcpy(segment,&header,UdpHeaderLength);//copy header
    memcpy(segment+UdpHeaderLength,data,DataLength);

    ipHeader.SegmentSize=header.SegmentSize;
    ipHeader.pro_segmentsize=ipHeader.Protocol;
    ipHeader.pro_segmentsize<<=16;
    ipHeader.pro_segmentsize+=header.SegmentSize;
    ipHeader.pro_segmentsize=htonl(ipHeader.pro_segmentsize);
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
    for(int i=0; i<header.SegmentSize/2; i++)
    {
        cs+=(*(p+i));
        while((cs>>16)>0)
        {
            unsigned short temp=cs>>16;
            cs&=0x0000FFFF;
            cs+=temp;
        }
    }
    header.CheckSum=(unsigned short)cs;
    return 1;
}

int PackUdpSegment::writeSegment()
{
    std::fstream segmentFile("./udp_segment.txt",std::ios::out);
    unsigned short *output=(unsigned short *)segment;
    for(int i=0; i<header.SegmentSize/2; i++)
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
    return ntohs(header.SegmentSize);
}

void PackUdpSegment::header_hton()
{
    header.SourcePort=htons(header.SourcePort);
    header.DestinationPort=htons(header.DestinationPort);
    header.SegmentSize=htons(header.SegmentSize);
}

void PackUdpSegment::header_ntoh()
{
    header.SourcePort=ntohs(header.SourcePort);
    header.DestinationPort=ntohs(header.DestinationPort);
    header.SegmentSize=ntohs(header.SegmentSize);
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




