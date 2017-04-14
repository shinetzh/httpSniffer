#include "SnifferDlg.h"

LRESULT CALLBACK DlgProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	InitCommonControls();
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)DlgProc);
	return 0;
}


LRESULT CALLBACK DlgProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	LVCOLUMN LvCol;
	LVITEM LvItem;
	LPNMITEMACTIVATE lpnmitem;
	LPNMHDR lpnmhdr;
	//�ж�ץ��״̬
	static BOOL Flag = true;

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{

		hDlg = hwnd;



		//��ʼ��wpcap
		InitWpcapNetCards();



		HWND hList = GetDlgItem(hwnd, IDC_LIST);
		memset(&LvCol, 0, sizeof(LvCol));
		LvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		LvCol.fmt = LVCFMT_LEFT;
		LvCol.cx = 0x25;
		LvCol.pszText = "���";
		SendMessage(GetDlgItem(hwnd,IDC_LIST),LVM_INSERTCOLUMN,0,(LPARAM)&LvCol);
		LvCol.cx = 0x80;
		LvCol.pszText = "ʱ��";
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 1, (LPARAM)&LvCol);
		LvCol.pszText = "Э��";
		LvCol.cx = 0x25;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 2, (LPARAM)&LvCol);
		LvCol.pszText = "ԴMAC";
		LvCol.cx = 0x80;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 3, (LPARAM)&LvCol);
		LvCol.pszText = "ԴIP";
		LvCol.cx = 0x60;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 4, (LPARAM)&LvCol);
		LvCol.pszText = "ԴPORT";
		LvCol.cx = 0x40;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 5, (LPARAM)&LvCol);
		LvCol.pszText = "Ŀ��MAC";
		LvCol.cx = 0x80;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 6, (LPARAM)&LvCol);
		LvCol.pszText = "Ŀ��IP";
		LvCol.cx = 0x60;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 7, (LPARAM)&LvCol);
		LvCol.pszText = "Ŀ��PORT";
		LvCol.cx = 0x40;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 8, (LPARAM)&LvCol);
		LvCol.pszText = "���ݳ���";
		LvCol.cx = 0x40;
		SendMessage(GetDlgItem(hwnd, IDC_LIST), LVM_INSERTCOLUMN, 9, (LPARAM)&LvCol);
		ListView_SetExtendedListViewStyleEx(
			GetDlgItem(hwnd, IDC_LIST),
			0,
			LVS_EX_FULLROWSELECT |
			LVS_EX_HEADERDRAGDROP |
			LVS_EX_GRIDLINES
			);
		memset(&LvItem, 0, sizeof(LvItem));
		break;
	}
	case WM_NOTIFY:
		lpnmhdr = (LPNMHDR)lParam;
		if (lpnmhdr->hwndFrom == GetDlgItem(hwnd, IDC_LIST))
			if (lpnmhdr->code == NM_CLICK)
			{
				lpnmitem = (LPNMITEMACTIVATE)lParam;
				int iSelect = ListView_GetSelectionMark(GetDlgItem(hwnd, IDC_LIST));
				if (iSelect > packets.size())
					break;
				Packet P = packets[iSelect];
				char Buf[65535 * 3] = { 0 };
				memset(Buf, 0, sizeof(Buf));
				for (unsigned int i = 0; i<p.Length; i++)
				{
					//�����ַ���ָ��
					char *ptr = p.pContent;
					//ÿ����ʾ16���ַ�
					if (i % 16 == 0 && i != 0)
					{
						strcat_s(Buf, "\r\n");
					}
					if (ptr[i] == '\0')
					{
						strcat_s(Buf, "00 ");
					}
					else
					{
						char temp[10] = { 0 };
						sprintf_s(temp, "%02x ", ptr[i]);  //һ��һ��int�ͱ�����Ҳ�ǰ�λ������ƥ��
						if (strstr(temp, "ffffff") != NULL)//ΪʲôҪ������жϣ�����
						{
							strcpy_s(temp, temp + 6);
						}
						strcat_s(Buf, temp);
					}
				}
				HWND hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
				SetWindowText(hEdit1, Buf);
				//��ʾ�ַ�
				memset(Buf, 0, sizeof(Buf));
				for (unsigned int i = 0; i<p.Length; i++)
				{
					char *ptr = p.pContent;
					if (i % 16 == 0 && i != 0)
					{
						strcat(Buf, "\r\n");
					}
					char ch = ptr[i];
					char temp[10] = { 0 };
					sprintf_s(temp, "%c", ch);
					strcat_s(Buf, temp);
				}
				HWND hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);
				SetWindowText(hEdit2, Buf);
			}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON1)
		{
			if (Flag)
			{
				//ɾ��������ʾ����
				ListView_DeleteAllItems(GetDlgItem(hwnd,IDC_LIST));
				//��ʼ��̽
				pcap_if_t *pcapDev;
				LRESULT result = SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				while (result == CB_ERR){
					MessageBox(NULL, "��ѡ������", "����", MB_OK);
					result = SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				}
				pcapDev = netDevs[result];
				StartSniffer(pcapDev);
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT1), "");
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT2), "");
				SetWindowText(GetDlgItem(hwnd, IDC_BUTTON1), "ֹͣ��̽");

				Flag = !Flag;
			}
			else
			{
				//ֹͣ��̽
				StopSniffer(pcapHandle);
				SetWindowText(GetDlgItem(hwnd, IDC_BUTTON1), "��ʼץ��");

				Flag = !Flag;
			}
		}
		break;
	case WM_CLOSE:
		//ֹͣ��̽
		EndDialog(hwnd, NULL);
		DestroyWindow(hwnd);
		break;
	default:
		break;
	}
	

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//��һ�����ݰ���List Control
//////////////////////////////////////////////////////////////////////////
BOOL InsertPacket(Packet p)
{
	char Buf[1024];
	LVITEM LvItem;
	LvItem.mask = LVIF_TEXT;
	LvItem.iItem = PacketCount++;
	LvItem.iSubItem = 0;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%d", PacketCount);
	LvItem.pszText = Buf;
	SendMessage(GetDlgItem(hDlg,IDC_LIST), LVM_INSERTITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 1;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%I64d", p.SrcMac[0], p.time);
	LvItem.pszText = _strupr(Buf);
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 2;
	memset(Buf, 0, sizeof(Buf));
	if (p.Protocol == TCP)
	{
		memcpy(Buf, "TCP", sizeof("TCP"));
	}
	else
	{
		memcpy(Buf, "UDP", sizeof("TCP"));
	}
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 3;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%02x:%02x:%02x:%02x:%02x:%02x", p.SrcMac[0], p.SrcMac[1], p.SrcMac[2], p.SrcMac[3], p.SrcMac[4], p.SrcMac[5]);
	LvItem.pszText = _strupr(Buf);
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 4;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%s", inet_ntoa(p.SrcIp));
	LvItem.pszText = Buf;
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 5;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%d", ntohs(p.SrcPort));
	LvItem.pszText = Buf;
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 6;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%02x:%02x:%02x:%02x:%02x:%02x", p.DestMac[0], p.DestMac[1], p.DestMac[2], p.SrcMac[3], p.DestMac[4], p.DestMac[5]);
	LvItem.pszText = _strupr(Buf);
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 7;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%s", inet_ntoa(p.DestIp));
	LvItem.pszText = Buf;
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 8;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%d", ntohs(p.DestPort));
	LvItem.pszText = Buf;
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	LvItem.iSubItem = 9;
	memset(Buf, 0, sizeof(Buf));
	sprintf(Buf, "%d", p.Length);
	LvItem.pszText = Buf;
	SendMessage(GetDlgItem(hDlg, IDC_LIST), LVM_SETITEM, (WPARAM)0, (LPARAM)&LvItem);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//��ʼ��wpcap
//////////////////////////////////////////////////////////////////////////
int InitWpcapNetCards()
{
	//�����豸ָ��
	pcap_if_t *pcapDev;
	//�洢������Ϣ
	char errContent[PCAP_ERRBUF_SIZE];

	//�����������������
	vector<pcap_if_t*>::iterator iter;

	// ��������豸ָ��
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &pcapDev, errContent) == -1)
	{
		MessageBox(NULL, "��ȡ�����豸ʧ��", "����", MB_OK);
		return -1;
	}
	//ѡȡ��������
	while (pcapDev != NULL)
	{
		netDevs.push_back(pcapDev);
		pcapDev = pcapDev->next;
	}


	for (iter = netDevs.begin(); iter != netDevs.end(); iter++){
		SendMessage(GetDlgItem(hDlg, IDC_COMBO1), CB_ADDSTRING, (WPARAM)0, (LPARAM)(*iter)->description);
	}
	return 0;
}