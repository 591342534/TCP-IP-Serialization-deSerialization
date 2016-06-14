#include "PackTcpSegment.h"

PackTcpSegment::PackTcpSegment()
{
    ipHeader.SourceIpAddr=0;
    ipHeader.DestinationIpAddr=0;
    ipHeader.SegmentLength=0;
    ipHeader.Protocol=0x0006;

    header.SourcePort = 0;
    header.DestinationPort = 0;
    header.SequenceNumber = 0;
    header.AckNumber = 0;
    header.HeaderLength_Reserver_Ctrl = 0;
    header.WndSize = 0;
    header.CheckSum = 0;
    header.UrgentPointer = 0;

    MSS = 0;
    WindowScaleFator = 0;
    SACK_permitted = 0;
    SACK = 0;
    TimeStamp[0] = 0;
    TimeStamp[1] = 0;
    TimeStamp[2] = 0;

    OptionLength = 0;
    DataLength = 0;
    SegmentLength = 0;

    options=(unsigned char*)malloc(MaxOptionLength);
    data = (unsigned char*) malloc(MaxDataLength);
    segment = (unsigned char*) malloc(MaxSegmentLength);
}

PackTcpSegment::~PackTcpSegment()
{
    free(options);
    free(data);
    free(segment);
}

int PackTcpSegment::setIpSourceAddr(const char *addr)
{
    ipHeader.SourceIpAddr=(inet_addr(addr));
    return 1;
}

int PackTcpSegment::setIpDestinationAddr(const char *addr)
{
    ipHeader.DestinationIpAddr=(inet_addr(addr));
    return 1;
}

int PackTcpSegment::setSourcePort(int port)
{
    //确保端口号有效
    if (port < 65536 && port >= 0)
    {
        header.SourcePort = port;
        return 0;
    }
    return -1;
}

int PackTcpSegment::setDestinationPort(int port)
{
    //确保端口号有效
    if (port < 65536 && port >= 0)
    {
        header.DestinationPort = port;
        return 1;
    }
    return -1;
}

int PackTcpSegment::setSequnceNumber(int seq)
{
    header.SequenceNumber = seq;
    return 1;
}

int PackTcpSegment::setAckNumber(int ack)
{
    header.AckNumber = ack;
    return 1;
}

int PackTcpSegment::setURG(int urg)
{
    if (urg)
    {
        header.HeaderLength_Reserver_Ctrl |= 0x0020;
    }
    else
    {
        header.HeaderLength_Reserver_Ctrl &= 0xFFDF;
    }
    return 1;
}

int PackTcpSegment::setACK(int ack)
{
    if (ack)
    {
        header.HeaderLength_Reserver_Ctrl |= 0x0010;
    }
    else
    {
        header.HeaderLength_Reserver_Ctrl &= 0xFFEF;
    }
    return 1;
}

int PackTcpSegment::setPSH(int psh)
{
    if (psh)
    {
        header.HeaderLength_Reserver_Ctrl |= 0x0008;
    }
    else
    {
        header.HeaderLength_Reserver_Ctrl &= 0xFF7F;
    }
    return 1;
}

int PackTcpSegment::setRST(int rst)
{
    if (rst)
    {
        header.HeaderLength_Reserver_Ctrl |= 0x0040;
    }
    else
    {
        header.HeaderLength_Reserver_Ctrl &= 0xFFBF;
    }
    return 1;
}

int PackTcpSegment::setSYN(int syn)
{
    if (syn)
    {
        header.HeaderLength_Reserver_Ctrl |= 0x0002;
    }
    else
    {
        header.HeaderLength_Reserver_Ctrl &= 0xFFFD;
    }
    return 1;
}

int PackTcpSegment::setFIN(int fin)
{
    if (fin)
    {
        header.HeaderLength_Reserver_Ctrl |= 0x0001;
    }
    else
    {
        header.HeaderLength_Reserver_Ctrl &= 0xFFFE;
    }
    return 1;
}

int PackTcpSegment::setWndSize(int size)
{
    header.WndSize = size;
    return 1;
}

int PackTcpSegment::setUrgentPointer(int p)
{
    header.UrgentPointer = p;
    return 1;
}

int PackTcpSegment::setMSS(int mss)
{
    if((OptionLength+4)>MaxOptionLength)
        return -1;
    //type 2,length 4
    MSS |= 0x02040000;
    if (mss < 65536)
    {
        MSS |= mss;
        memcpy(options+OptionLength,&MSS,4);
        OptionLength += 4;
        return 1;
    }
    return -1;
}

int PackTcpSegment::setWSF(int wsf)
{
    if((OptionLength+4)>MaxOptionLength)
        return -1;
    if (wsf > 255)
        return -1;
    //type 3,length 3
    WindowScaleFator |= 0x01030300;
    WindowScaleFator |= wsf;
    memcpy(options+OptionLength,&WindowScaleFator,4);
    OptionLength += 4;
    return 1;
}

int PackTcpSegment::setTimeStamp(int TSval, int TSecr)
{
    if((OptionLength+12)>MaxOptionLength)
        return -1;
    //type 8,length 10
    TimeStamp[0] |= 0x0101080A;
    TimeStamp[1] |= TSval;
    TimeStamp[2] |= TSecr;
    memcpy(options+OptionLength,TimeStamp,12);
    OptionLength += 12;
    return 1;
}

int PackTcpSegment::setSACK_permitted()
{
    if((OptionLength+4)>MaxOptionLength)
        return -1;
    //type 4,length 2
    SACK_permitted |= 0x01010402;
    memcpy(options+OptionLength,&SACK_permitted,4);
    OptionLength+=4;
    return 1;
}

int PackTcpSegment::setSACK(unsigned int *block[2], int num_block)
{

    //type 5,length 2+num_block*2
    int l = 2 * num_block + 1;
    if((OptionLength+4*l)>MaxOptionLength)
        return -1;
    SACK = (unsigned int*) malloc(4 * l);
    unsigned int temp = 0x01010500;
    unsigned int * p = SACK + 1;
    temp += l;
    for (int i = 0; i < num_block; i++)
    {
        memcpy(p, block[0], 4);
        memcpy(p + 1, block[1], 4);
        p += 2;
        block++;
    }
    memcpy(options+OptionLength,SACK,4*l);
    OptionLength += 4 * l;
    return 1;
}

int PackTcpSegment::setData(unsigned char *d, int len)
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

int PackTcpSegment::genHeaderLength()
{
    int hl = (20 + OptionLength) / 4;
    hl = hl << 12;
    header.HeaderLength_Reserver_Ctrl |= hl;
    return 1;
}

int PackTcpSegment::genSegment()
{
    header_hton();
    memcpy(segment,&header,HeaderLength);//copy header
    if(OptionLength>0)
    {
        memcpy(segment+HeaderLength,options,OptionLength);
    }
    memcpy(segment+HeaderLength+OptionLength,data,DataLength);

    SegmentLength=HeaderLength+DataLength+OptionLength;
    ipHeader.SegmentLength=htons(SegmentLength);

    genCheckSum();
    return 1;
}

int PackTcpSegment::genCheckSum()
{
    unsigned int cs=0;
    unsigned short * p=(unsigned short*) &ipHeader;
    //ip header校验和
    for(int i=0; i<6; i++)
    {
        cs+=(*(p+i));//二进制反码相加
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
        cs+=(*(p+i));//二进制反码相加
        while((cs>>16)>0)
        {
            unsigned short temp=cs>>16;
            cs&=0x0000FFFF;
            cs+=temp;
        }
    }
    header.CheckSum=(unsigned short)cs^0xFFFF;
    memcpy(segment+16,&header.CheckSum,2);
    return 1;
}

int PackTcpSegment::writeSegment()
{
    std::fstream segmentFile("./tcp_segment.txt",std::ios::out);
    unsigned char *output=(unsigned char *)segment;
    for(int i=0; i<SegmentLength; i++)
    {
        segmentFile<<(*(output+i));
    }
    segmentFile.close();
    return 1;
}

unsigned char * PackTcpSegment::getSegment()
{
    return segment;
}

int PackTcpSegment::getSegmentLength()
{
    return SegmentLength;
}

void PackTcpSegment::header_hton()
{
    ipHeader.Protocol=htons(ipHeader.Protocol);
    header.SourcePort=htons(header.SourcePort);
    header.DestinationPort=htons(header.DestinationPort);
    header.AckNumber=htonl(header.AckNumber);
    header.SequenceNumber=htonl(header.SequenceNumber);
    header.HeaderLength_Reserver_Ctrl=htons(header.HeaderLength_Reserver_Ctrl);
    header.WndSize=htons(header.WndSize);
    header.UrgentPointer=htons(header.UrgentPointer);
    unsigned int *temp=(unsigned int *)options;
    if(OptionLength>0)
    {
        for(int i=0; i<(OptionLength/4); i++)
        {
            (*temp)=htonl(*temp);
            temp++;
        }
    }
}

void PackTcpSegment::header_ntoh()
{
    header.SourcePort=ntohs(header.SourcePort);
    header.DestinationPort=ntohs(header.DestinationPort);
    header.AckNumber=ntohl(header.AckNumber);
    header.SequenceNumber=ntohl(header.SequenceNumber);
    header.HeaderLength_Reserver_Ctrl=ntohs(header.HeaderLength_Reserver_Ctrl);
    header.WndSize=ntohs(header.WndSize);
    header.UrgentPointer=ntohs(header.UrgentPointer);
    unsigned int *temp=(unsigned int *)options;
    unsigned short hl=header.HeaderLength_Reserver_Ctrl;
    hl>>=12;
    hl*=4;
    OptionLength=hl-HeaderLength;
    if(OptionLength>0)
    {
        for(int i=0; i<(OptionLength/4); i++)
        {
            (*temp)=ntohl(*temp);
            temp++;
        }
    }
}

void PackTcpSegment::parseSegment(unsigned char *d,int Len)
{
    memcpy(&header,d,HeaderLength);
    header_ntoh();
    d+=HeaderLength;
    if(OptionLength>0)
    {
        memcpy(options,d,OptionLength);
        d+=OptionLength;
    }
    DataLength=Len-HeaderLength-OptionLength;
    memcpy(data,d,DataLength);

    std::fstream dataFile("./app_data_tcp.txt",std::ios::out);
    unsigned char *output=(unsigned char *)data;
    for(int i=0; i<DataLength; i++)
    {
        dataFile<<(*(output+i));

    }
    dataFile.close();
}

unsigned char * PackTcpSegment::getAppData()
{
    return data;
}

int PackTcpSegment::getDataLength()
{
    return DataLength;
}
