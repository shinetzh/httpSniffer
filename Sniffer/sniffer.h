

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


//ipЭ��ö������
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

//�̲߳����ṹ��
struct ThreadArg{
	LPVOID pcapHandle;
	LPVOID pFunc;
};

//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ�����������
//////////////////////////////////////////////////////////////////////////
//�߳̾��
extern HANDLE hThread;
//���ݰ��洢����
extern vector<Packet> packets;
//���ݰ��ݴ�ṹ��
extern Packet p;
//pcap_t���
extern pcap_t *pcapHandle;
//�̲߳���
extern ThreadArg arg;
//���ݰ�����
extern unsigned int PacketCount;
extern vector<pcap_if_t*> netDevs;
//������������
//////////////////////////////////////////////////////////////////////////
//��̫�������ص�����
void EtherPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
//IP�����ص�����
BOOL IpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
//UDP�����ص�����
BOOL UdpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
//Tcp�����ص�����
BOOL TcpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent);
BOOL InsertPacket(Packet p);
//�̻߳ص�����
BOOL Func(LPVOID p);
//��ʼ��̽
BOOL StartSniffer(pcap_if_t *pcapDev);
//ֹͣ��̽
BOOL StopSniffer(pcap_t *pcapHandle);
//Winpcap��ʼ��
int InitWpcap(pcap_if_t *pcapDev);

//����������Ϣ���ļ���
void WriteNetLayer(Packet p);
long long int GetMilliTime();