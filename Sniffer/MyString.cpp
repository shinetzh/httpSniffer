#include "MyString.h"

int SubString(char *string, char *sub)    //�����ҵ����ִ����ڵ�index��0��ʼ
{
	int i, samelen = 0;
	char *pstr;
	int subLen = strlen(sub);
	for (i = 0, pstr = string; pstr[i] != '\0'; i++)
	{
		if (pstr[i] == sub[samelen])
		{
			samelen++;
		}
		else samelen = 0;
		if (samelen == subLen)
		{
			break;
		}
	}
	return i - subLen + 1;
}