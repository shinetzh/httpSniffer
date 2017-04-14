

#pragma once

#define HAVE_REMOTE
#define BUFFER_MAX_LENGTH 65536 /* buffer max length */
#include <stdlib.h>
#include <vector>
#include <WinSock2.h>
#include <CommCtrl.h>
#include <Windows.h>
#include <time.h>
#include <sys/timeb.h>
#include <fstream>
#include <iomanip>
using namespace std;

#include <pcap.h>
#include "remote-ext.h"
#include "Protocol.h"
#include "resource.h"


//ip协议枚举类型
enum Proto {TCP=0,UDP};
struct Packet{
	Proto			Protocol;
	long long       time;
	unsigned char	SrcMac[6];
	unsigned char	DestMac[6];
	in_addr			SrcIp;
	in_addr			DestIp;
	unsigned short	SrcPort;
	unsigned short  DestPort;
	unsigned int	Length;
	char			*pContent;
};

//线程参数结构体
struct ThreadArg{
	LPVOID pcapHandle;
	LPVOID pFunc;
};

//////////////////////////////////////////////////////////////////////////
//全局变量声明部分
//////////////////////////////////////////////////////////////////////////
//线程句柄
extern HANDLE hThread;
//数据包存储向量
extern vector<Packet> packets;
//数据包暂存结构体
extern Packet p;
//pcap_t句柄
extern pcap_t *pcapHandle;
//线程参数
extern ThreadArg arg;
//数据包数量
extern unsigned int PacketCount;
extern vector<pcap_if_t*> netDevs;
//函数声明部分
//////////////////////////////////////////////////////////////////////////
//以太网分析回调函数
void EtherPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
//IP分析回调函数
BOOL IpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
//UDP分析回调函数
BOOL UdpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
//Tcp分析回调函数
BOOL TcpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
BOOL InsertPacket(Packet p);
//线程回调函数
BOOL Func(LPVOID p);
//开始嗅探
BOOL StartSniffer(pcap_if_t *pcapDev);
//停止嗅探
BOOL StopSniffer(pcap_t *pcapHandle);
//Winpcap初始化
int InitWpcap(pcap_if_t *pcapDev);

//输出网络层信息到文件中
void WriteNetLayer(Packet p);
long long int GetMilliTime();