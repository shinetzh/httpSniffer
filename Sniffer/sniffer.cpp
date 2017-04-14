#include "sniffer.h"
#include "http.h"

//////////////////////////////////////////////////////////////////////////
//ȫ�ֱ������岿��
//////////////////////////////////////////////////////////////////////////
//�߳̾��
HANDLE hThread;
//���ݰ��洢����
vector<Packet> packets;
//���ݰ��ݴ�ṹ��
Packet p;
//pcap_t���
pcap_t *pcapHandle;
//�̲߳���
ThreadArg arg;
//���ݰ�����
unsigned int PacketCount;


//////////////////////////////////////////////////////////////////////////
//���ĺ���ʵ��
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//IP�����ص�����
//////////////////////////////////////////////////////////////////////////
BOOL IpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent)
{
	//IPЭ�����
	IP_HEADER *IpProtocol;

	//���IPЭ������
	IpProtocol = (IP_HEADER *)(packetContent + sizeof(ETHER_HEADER));

	//���ԴIP��Ŀ��IP
	p.SrcIp = IpProtocol->SrcIp;
	p.DestIp = IpProtocol->DestIp;

	//����IPЭ������
	switch (IpProtocol->proto)
	{
	case IPPROTO_TCP:
		p.Protocol = TCP;
		//��һ��TCPЭ��
		TcpPacketCallback(argument, packetHdr, packetContent);
		break;
	case IPPROTO_UDP:
		p.Protocol = UDP;
		//��һ������UDPЭ��
		UdpPacketCallback(argument, packetHdr, packetContent);
		break;
	default:
		break;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//TCP�����ص�����
//////////////////////////////////////////////////////////////////////////
BOOL TcpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent)
{
	//TCPЭ�����
	TCP_HEADER	*TcpProtocol;

	BOOL find_http = false;
	//���TCPЭ������
	TcpProtocol = (TCP_HEADER *)(packetContent + 14 + 20);

	//�ֱ����TCPЭ���Դ�˿ڡ�Ŀ�Ķ˿ڼ����ݳ���
	p.SrcPort = TcpProtocol->SrcPort;
	p.DestPort = TcpProtocol->DestPort;
	//����TCP�������ݳ���
	IP_HEADER *IpHdr = (IP_HEADER *)(packetContent + 14);
	int totallength = ntohs(IpHdr->TotalLen);
	int HeaderLength = totallength - 40;
	p.Length = HeaderLength;

	//�����������
	char *pBuf = new char[HeaderLength + 1];
	memset(pBuf, 0, HeaderLength + 1);
	char *ptr = (char *)((PBYTE)(TcpProtocol)+sizeof(TCP_HEADER));
	memcpy(pBuf, ptr, HeaderLength);

	p.pContent = pBuf;

	//�����񵽵����ݰ���Ϣ���������б�
	packets.push_back(p);

	InsertPacket(p);
	WriteNetLayer(p);

	char *Content = (char*)TcpProtocol + 20;
	int ContentLength = HeaderLength - 20;
	//http get or post
	if (!find_http && ((strncmp(Content, "GET", strlen("GET")) == 0)
		|| (strncmp(Content, "POST", strlen("POST")) == 0)))
	{
		find_http = true;
		HttpRequestCallback(Content,ContentLength);     //����http�������ݰ�
	}

	/* http response */
	if (!find_http && strncmp(Content, "HTTP/1.1", strlen("HTTP/1.1")) == 0)
	{
		find_http = true;
		HttpResponseCallback(Content, ContentLength);    //����http��Ӧ���ݰ�
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//UDP�����ص�����
//////////////////////////////////////////////////////////////////////////
BOOL UdpPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent)
{
	//UDP Э�����
	UDP_HEADER *UdpProtocol;

	//���UDPЭ������
	UdpProtocol = (UDP_HEADER *)(packetContent + 14 + 20);

	//�ֱ����UDPЭ���Դ�˿ڡ�Ŀ�Ķ˿ڼ����ݳ���
	p.SrcPort = UdpProtocol->SrcPort;
	p.DestPort = UdpProtocol->DestPort;
	int	HeaderLength = ntohs(UdpProtocol->Length) - sizeof(UDP_HEADER);
	p.Length = HeaderLength;

	//�����������
	char *pBuf = new char[HeaderLength + 1];
	memset(pBuf, 0, HeaderLength + 1);
	char *ptr = (char *)((PBYTE)(UdpProtocol)+sizeof(UDP_HEADER));
	memcpy(pBuf, ptr, HeaderLength);
	p.pContent = pBuf;

	//�����񵽵����ݰ���Ϣ���������б�
	packets.push_back(p);

	InsertPacket(p);
	WriteNetLayer(p);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��̫�������ص�����
//////////////////////////////////////////////////////////////////////////
void EtherPacketCallback(u_char *argument, const struct pcap_pkthdr *packetHdr, const u_char *packetContent)
{
	//��̫��Э�����
	ETHER_HEADER *EtherProtocol;

	//�����̫��Э������
	EtherProtocol = (ETHER_HEADER *)(packetContent);

	//���ԴMAC��Ŀ��MAC
	memcpy(p.SrcMac, EtherProtocol->EtherSrcHost, 6);
	memcpy(p.DestMac, EtherProtocol->EtherDestHost, 6);
	p.time = GetMilliTime();
	//IPЭ�����
	IpPacketCallback(argument, packetHdr, packetContent);
}

//////////////////////////////////////////////////////////////////////////
//�̻߳ص�����
//////////////////////////////////////////////////////////////////////////
BOOL Func(LPVOID p)
{
	ThreadArg *arg = (ThreadArg *)p;

	//ע��ص�������ѭ���������ݰ���ÿ����һ�����ݰ����Զ�����pFun()���з���
	pcap_loop((pcap_t*)arg->pcapHandle, -1, (pcap_handler)arg->pFunc, NULL);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��ʼ��̽
//////////////////////////////////////////////////////////////////////////
BOOL StartSniffer(pcap_if_t *pcapDev)
{
	InitWpcap(pcapDev);
	//��Ŀ����
	PacketCount = 0;

	//������ݰ���Ϣ
	for (unsigned int i = 0; i<packets.size(); i++)
	{
		delete packets[i].pContent;
	}
	packets.clear();

	arg.pcapHandle = pcapHandle;
	arg.pFunc = EtherPacketCallback;

	//���ûص������������ݰ�
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Func, (LPVOID)&arg, NULL, NULL);
	if (hThread == NULL)
	{
		MessageBox(NULL, "��̽ʧ��", NULL, NULL);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//ֹͣ��̽
//////////////////////////////////////////////////////////////////////////
BOOL StopSniffer(pcap_t *pcapHandle)
{
	//������̽�߳�
	if (TerminateThread(hThread, NULL) == FALSE)
	{
		MessageBox(NULL, "�߳��޷�����!", NULL, NULL);
		return FALSE;
	}
	pcap_close(pcapHandle);

	return TRUE;
}

long long int GetMilliTime()
{
	long long time_last;
	time_last = time(NULL);     //������  
	struct timeb t1;
	ftime(&t1);
	time_last = t1.millitm + t1.time * 1000;
	return time_last;
}




int InitWpcap(pcap_if_t *pcapDev){
	//�洢������Ϣ
	char errContent[PCAP_ERRBUF_SIZE];
	//BPF���˹���
	bpf_program filter;
	//���˹����ַ���
	char bpfFilterString[] = "ip";
	//����
	bpf_u_int32	netMask;
	//�����ַ
	bpf_u_int32 netIp = 0;

	//������
	if ((pcapHandle = pcap_open(pcapDev->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errContent)) == NULL)
	{
		MessageBoxA(NULL, "������ʧ��", "����", MB_OK);
		return -1;
	}

	// �����·�㣬ֻ֧����̫��
	if (pcap_datalink(pcapHandle) != DLT_EN10MB)
	{
		MessageBoxA(NULL, "ֻ֧����̫��", "����", MB_OK);
		pcap_freealldevs(pcapDev);
		return -1;
	}
	if (pcapDev->addresses != NULL)
	{
		//ȡ������ӿ���������
		netMask = ((struct sockaddr_in *)(pcapDev->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netMask = 0xFFFFFFFF;
	}

	//����BPF���˹���
	pcap_compile(pcapHandle, &filter, bpfFilterString, 0, netIp);

	//���ù��˹���
	pcap_setfilter(pcapHandle, &filter);

	arg.pcapHandle = pcapHandle;

	return 0;
}

void WriteNetLayer(Packet p){
	ofstream outfile;
	char Buf[1024];
	outfile.open("NetLayer.txt",ios_base::app);
	outfile << setw(10) << setiosflags(ios::left) << p.time;
	if (p.Protocol == TCP)
	{
		outfile << setw(10) << setiosflags(ios::left) << "TCP";
	}
	else
	{
		outfile << setw(10) << setiosflags(ios::left) << "UDP";
	}
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%02x:%02x:%02x:%02x:%02x:%02x", p.SrcMac[0], p.SrcMac[1], p.SrcMac[2], p.SrcMac[3], p.SrcMac[4], p.SrcMac[5]);
	outfile << setw(10) << setiosflags(ios::left) << _strupr(Buf);
	outfile << setw(10) << setiosflags(ios::left) << inet_ntoa(p.SrcIp);
	outfile << setw(10) << setiosflags(ios::left) << p.SrcPort;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%02x:%02x:%02x:%02x:%02x:%02x", p.DestMac[0], p.DestMac[1], p.DestMac[2], p.DestMac[3], p.DestMac[4], p.DestMac[5]);
	outfile << setw(10) << setiosflags(ios::left) << _strupr(Buf);
	outfile << setw(10) << setiosflags(ios::left) << inet_ntoa(p.DestIp);
	outfile << setw(10) << setiosflags(ios::left) << p.DestPort;
	outfile << setw(10) << setiosflags(ios::left) << p.Length;
	outfile << endl;
	outfile.close();
}