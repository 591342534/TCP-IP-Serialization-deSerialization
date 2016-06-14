#include "iostream"
#include "cmath"
#include "cstdlib"
#include "cstring"
#include <arpa/inet.h>
#include <fstream>

struct SegmentHeaderWithoutOptions
{
    //20 bytes header
    unsigned short SourcePort;
    unsigned short DestinationPort;
    unsigned int SequenceNumber;
    unsigned int AckNumber;
    unsigned short HeaderLength_Reserver_Ctrl;//0-3 header length(n*4 bytes n->5~15), 4-9 reserver, 10-15 control
    unsigned short WndSize;
    unsigned short CheckSum;
    unsigned short UrgentPointer;
};

struct PseudoIpHeader_TCP
{
    unsigned int SourceIpAddr;
    unsigned int DestinationIpAddr;
    unsigned short Protocol;
    unsigned short SegmentLength;
};

class PackTcpSegment
{
private:
    const static int MaxDataLength=1460;
    const static int MaxSegmentLength=1480;
    const static int MaxOptionLength=40;
    const static int HeaderLength=20;
    //IP header
    struct PseudoIpHeader_TCP ipHeader;

    //standard header
    struct SegmentHeaderWithoutOptions header;

    //options 0~40 bytes
    unsigned char *options;
    unsigned int MSS;//type:00000010 length:00000100 mss:2 bytes
    unsigned int WindowScaleFator;//NOP+type+length+wsf
    unsigned int TimeStamp[3];//NOP*2+type+length+TimaStamp Value(4 bytes)+TimeStamp Echo Reply(4 bytes)
    unsigned int SACK_permitted;//NOP*2+type+length
    unsigned int *SACK;//NOP*2+type+length   +block messages(n*4 bytes)
    int OptionLength;

    //来自应用层的数据字段的长度 or 解析出的数据长度
    int DataLength;

    //封装后报文长度
    int SegmentLength;

    //data
    unsigned char *data;//dynamic allocate memory

    //segment
    unsigned char *segment;

public:
    PackTcpSegment();
    ~PackTcpSegment();

    //设置伪首部参数
    int setIpSourceAddr(const char *addr);
    int setIpDestinationAddr(const char *addr);

    //设置标准头部参数
    int setSourcePort(int port);
    int setDestinationPort(int port);
    int setSequnceNumber(int seq);
    int setAckNumber(int ack);
    int genHeaderLength();

    int setURG(int urg);
    int setACK(int ack);
    int setPSH(int psh);
    int setRST(int rst);
    int setSYN(int syn);
    int setFIN(int fin);
    int setWndSize(int size);
    int genCheckSum();
    int setUrgentPointer(int p);

    //设置选项字段参数
    int setMSS(int mss);
    int setWSF(int wsf);
    int setTimeStamp(int TSval,int TSecr);
    int setSACK_permitted();
    int setSACK(unsigned int *block[2],int num_block);

    //设置数据
    int setData(unsigned char *d,int len);

    //封装报文
    int genSegment();

    int writeSegment();

    //获取封装后的报文
    unsigned char *getSegment();
    int getSegmentLength();

    //字节序转换
    void header_hton();
    void header_ntoh();

    //解析报文 提取数据
    void parseSegment(unsigned char *d,int Len);
    unsigned char * getAppData();
    int getDataLength();
};
