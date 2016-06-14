#include "PackIpPacket.h"

PackIpPacket::PackIpPacket()
{
    //ctor
    header.Ver_HLen_ServiceType=0x4;
    header.Ver_HLen_ServiceType<<=12;

    header.Flag_FraOffset=0;
    header.HeaderChecksum=0;
    header.Identification=0;
    header.IpDestinationAddr=0;
    header.IpSourceAddr=0;
    header.TotalLength=0;
    header.TTL_Protocol=0;

    OptionLength=0;
    SegmentLength=0;
    PacketLength=0;

    segment=(unsigned char*)malloc(MaxSegmentLength);
    options=(char*)malloc(MaxOptionLength);
    packet=(unsigned char*)malloc(MaxPacketLength);
}

PackIpPacket::~PackIpPacket()
{
    //dtor
    free(segment);
    free(options);
    free(packet);
}

int PackIpPacket::setVer(int ver)
{
    //IPv4 IPv6
    if(ver!=4&&ver!=6)
    {
        return -1;
    }
    header.Ver_HLen_ServiceType&=0x0FFF;
    ver<<=12;
    header.Ver_HLen_ServiceType|=ver;
    return 1;
}

int PackIpPacket::genHeaderLength()
{
    int l=HeaderLength+OptionLength;
    l=l/4;
    l<<=8;
    header.Ver_HLen_ServiceType|=l;
    return 1;
}

int PackIpPacket::setServiceType(int type)
{
    if(type>255)
    {
        return -1;
    }
    header.Ver_HLen_ServiceType&=0xFF00;
    header.Ver_HLen_ServiceType|=type;
    return 1;
}

int PackIpPacket::genTotalLength()
{
    header.TotalLength=HeaderLength+OptionLength+SegmentLength;
    PacketLength=header.TotalLength;
    return 1;
}

int PackIpPacket::setIdetification(int id)
{
    header.Identification=id;
    return 1;
}

int PackIpPacket::setDonnotFra(unsigned int fra)
{
    if(fra)
    {
        header.Flag_FraOffset|=0x4000;
    }
    else
    {
        header.Flag_FraOffset&=0xBFFF;
    }
    return 1;
}

int PackIpPacket::setMoreFra(unsigned int fra)
{
    if(fra)
    {
        header.Flag_FraOffset|=0x2000;
    }
    else
    {
        header.Flag_FraOffset&=0xDFFF;
    }
    return 1;
}

int PackIpPacket::setTTL(int ttl)
{
    if(ttl>255)
    {
        return -1;
    }
    ttl<<=8;
    header.TTL_Protocol|=ttl;
    return 1;
}

int PackIpPacket::setProtocol(int pro_type)
{
    if(pro_type!=6&&pro_type!=17)
    {
        return -1;
    }
    if(pro_type>255)
    {
        return -1;
    }
    header.TTL_Protocol&=0xFF00;
    header.TTL_Protocol|=pro_type;
    return 1;
}

int PackIpPacket::setIpSourceAddr(const char *addr)
{
    header.IpSourceAddr=htonl(inet_addr(addr));
    return 1;
}

int PackIpPacket::setIpDestinationAddr(const char *addr)
{
    header.IpDestinationAddr=htonl(inet_addr(addr));
    return 1;
}

int PackIpPacket::setRecordRoute(int len,int p)
{
    //主机不记录路由
    unsigned int rr=0x01070000;
    len<<=8;
    rr|=len;
    rr|=p;
    memcpy(options+OptionLength,&rr,4);
    OptionLength+=(len+1);
    return 1;
}

int PackIpPacket::setStrictSrcRoute(char **addr,int num,int len,int p)
{
    unsigned int ssr=0x01890000;
    unsigned int temp;
    len<<=8;
    ssr|=len;
    ssr|=p;
    memcpy(options+OptionLength,&ssr,4);
    OptionLength+=4;
    for(int i=0; i<num; i++)
    {
        temp=inet_addr((*addr)+i);
        memcpy(options+OptionLength,&temp,4);
        OptionLength+=4;
    }

    return 1;
}

int PackIpPacket::setLooseSrcRoute(char **addr,int num,int len,int p)
{
    unsigned int lsr=0x01830000;
    unsigned int temp;
    len<<=8;
    lsr|=len;
    lsr|=p;
    memcpy(options+OptionLength,&lsr,4);
    OptionLength+=4;
    for(int i=0; i<num; i++)
    {
        temp=inet_addr((*addr)+i);
        memcpy(options+OptionLength,&temp,4);
        OptionLength+=4;
    }
    OptionLength+=num*4;
    return 1;
}

int PackIpPacket::setTimetStamp(int len,int p,int flag,char **addr,int num)
{
    unsigned int ts=0x44000000;
    unsigned int temp;
    len<<=16;
    ts|=len;
    ts|=flag;
    int overf=(len-(MaxOptionLength-OptionLength))/8;
    overf<<=4;
    ts|=overf;
    memcpy(options+OptionLength,&ts,4);
    if(flag==3)
    {
        for(int i=0; i<num&&OptionLength<=MaxOptionLength; i++)
        {
            temp=inet_addr((*addr)+i);
            memcpy(options+OptionLength,&temp,4);
            OptionLength+=8;
        }
    }
    return 1;
}

int PackIpPacket::setSegment(unsigned char *d,int len)
{
    memcpy(segment,d,len);
    SegmentLength=len;
    return 1;
}

int PackIpPacket::genPacket()
{
    genHeaderLength();
    genTotalLength();
    header_hton();
    genHeaderChecksum();
    memcpy(packet,&header,HeaderLength);
    if(OptionLength>0)
    {
        memcpy(packet+HeaderLength,&options,OptionLength);
    }
    memcpy(packet+OptionLength+HeaderLength,segment,SegmentLength);
    return 1;
}

int PackIpPacket::genHeaderChecksum()
{

    unsigned int cs=0;
    unsigned short * p=(unsigned short*) &header;
    //ip header校验和
    for(int i=0; i<(HeaderLength/2); i++)
    {
        cs+=(*(p+i));
        while((cs>>16)>0)
        {
            unsigned short temp=cs>>16;
            cs&=0x0000FFFF;
            cs+=temp;
        }
    }
    if(OptionLength==0)
    {
        header.HeaderChecksum=(unsigned short)cs^0xFFFF;
        return 1;
    }
    p=(unsigned short*)options;
    for(int i=0; i<(OptionLength/2); i++)
    {
        cs+=(*(p+i));
        while((cs>>16)>0)
        {
            unsigned short temp=cs>>16;
            cs&=0x0000FFFF;
            cs+=temp;
        }
    }
    header.HeaderChecksum+=(unsigned short)cs^0xFFFF;
    return 1;
}

int PackIpPacket::writePacket()
{
    std::fstream packetFile("./ip_packet.txt",std::ios::out);
    unsigned char *output=(unsigned char *)packet;
    for(int i=0; i<PacketLength; i++)
    {
        packetFile<<(*(output+i));
    }
    packetFile.close();
    return 1;
}


void PackIpPacket::header_hton()
{
    header.Ver_HLen_ServiceType=htons(header.Ver_HLen_ServiceType);
    header.TotalLength=htons(header.TotalLength);
    header.Identification=htons(header.Identification);
    header.Flag_FraOffset=htons(header.Flag_FraOffset);
    header.TTL_Protocol=htons(header.TTL_Protocol);
    header.HeaderChecksum=htons(header.HeaderChecksum);
    header.IpSourceAddr=htonl(header.IpSourceAddr);
    header.IpDestinationAddr=htonl(header.IpDestinationAddr);
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

void PackIpPacket::header_ntoh()
{
    header.Ver_HLen_ServiceType=ntohs(header.Ver_HLen_ServiceType);
    header.TotalLength=ntohs(header.TotalLength);
    header.Identification=ntohs(header.Identification);
    header.Flag_FraOffset=ntohs(header.Flag_FraOffset);
    header.TTL_Protocol=ntohs(header.TTL_Protocol);
    header.HeaderChecksum=ntohs(header.HeaderChecksum);
    header.IpSourceAddr=ntohl(header.IpSourceAddr);
    header.IpDestinationAddr=ntohl(header.IpDestinationAddr);
    unsigned int *temp=(unsigned int *)options;
    unsigned short hl=header.Ver_HLen_ServiceType;
    hl<<=4;
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

void PackIpPacket::parsePacket(unsigned char *d,int Len)
{
    memcpy(&header,d,HeaderLength);
    d+=HeaderLength;
    header_ntoh();
    if(OptionLength>0)
    {
        memcpy(options,d,OptionLength);
        d+=OptionLength;
    }
    SegmentLength=Len-HeaderLength-OptionLength;
    memcpy(segment,d,SegmentLength);
}

unsigned char * PackIpPacket::getSegment()
{
    return segment;
}

int PackIpPacket::getSegmentLength()
{
    return SegmentLength;
}

unsigned char * PackIpPacket::getPacket()
{
    return packet;
}

int PackIpPacket::getPacketLength()
{
    return PacketLength;
}

int PackIpPacket::getProtocolType()
{
    unsigned short type=header.TTL_Protocol;
    type<<=8;
    type>>=8;
    return (int)type;
}
