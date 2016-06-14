#ifndef PACKIPPACKET_H
#define PACKIPPACKET_H

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
    //设置协议相关参数
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

    //生成IP分组
    int genPacket();

    //将生成的分组写入文本文件
    int writePacket();

    //主机字节序和网络字节序的转换
    void header_hton();
    void header_ntoh();

    //获取TCP/UDP报文
    int setSegment(unsigned char *d,int len);

    //解析IP分组 提取TCP/UDP报文
    void parsePacket(unsigned char * d,int Len);//解析IP分组

    //获取解析后的数据字段 即TCP/UDP报文
    unsigned char *getSegment();

    //获取生成的IP分组
    unsigned char *getPacket();

    //获取解析后报文长度
    int getSegmentLength();

    //获取生成的分组长度
    int getPacketLength();

    //获取解析后的报文类型TCP or UDP
    int getProtocolType();

private:
    const static int MaxPacketLength=1500;
    const static int MaxSegmentLength=1480;
    const static int MaxOptionLength=40;
    const static int HeaderLength=20;

    //IP分组头部
    struct Ipheader header;

    //头部选项字段 长度不定
    char *options;

    //TCP/UDP报文
    unsigned char *segment;

    //生成的分组
    unsigned char *packet;

    //选项字段的长度
    int OptionLength;

    //报文长度
    int SegmentLength;

    //分组长度
    int PacketLength;


};

#endif // PACKIPPACKET_H
