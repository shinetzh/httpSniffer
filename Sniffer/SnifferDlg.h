#include "sniffer.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"Comctl32.lib")
HWND hDlg;
vector<pcap_if_t*> netDevs;
int InitWpcapNetCards();