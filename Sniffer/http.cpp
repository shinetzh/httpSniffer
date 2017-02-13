#include "http.h"
#include "MyString.h"
#include <time.h>
#include <sys/timeb.h>

HTTP_REQUEST HttpRequest;
HTTP_RESPONSE HttpResponse;

bool HttpRequestCallback(char *Content, int ContentLength)
{
	memset(&HttpRequest, 0, sizeof(HTTP_REQUEST));
	char *buffer = Content;

	char Delimiter[3] = { (char)0x0d, (char)0x0a, '\0' };

	char Temp[3][100] = { 0 };     //存储http前三个字段
	char Temper[20][1000], *Value;
	memset(Temper, 0, sizeof(Temper));
	int iNumber = 0, jLength;
	for (jLength = 0; jLength < ContentLength; jLength++)
	{
		if (!strncmp(buffer + jLength, " ", 1))
		{
			strncpy(Temp[iNumber], buffer, jLength);
			buffer = buffer + jLength + 1;
			ContentLength = ContentLength - jLength - 1;
			jLength = 0;
			iNumber++;
		}
		if (iNumber >= 2)
			break;
	}
	strcpy(HttpRequest.Request_Method, Temp[0]);
	strcpy(HttpRequest.Request_URI, Temp[1]);

//将字符串切割后，存入缓存Temper中
	int i = 0,index =  0;
	while (true)
	{
		index = SubString(buffer, Delimiter);
		if (index == 0)
			break;
		strncpy(Temper[i], buffer, index);
		buffer = buffer + index + 2;
		i++;
	}
	strcpy(HttpRequest.Request_Version, Temper[0]);//http头的第三个字段

	i = 1;
	char Type[50] = { 0 };
	while (true)
	{
		index = SubString(Temper[i],": ");
		if (index == -1)
			break;
		strncpy(Type, Temper[i], index);
		Value = Temper[i] + index + 2;

		if (!strcmp(Type, "Cache-Control"))
		{
			strcpy(HttpRequest.CacheControl, Value);
		}
		else if (!strcmp(Type, "Host"))  //
		{
			strcpy(HttpRequest.Host, Value);
		}
		else if (!strcmp(Type, "Accept"))  //
		{
			strcpy(HttpRequest.Accept, Value);
		}
		else if (!strcmp(Type, "Accept-Language"))  //
		{
			strcpy(HttpRequest.AcceptLanguage, Value);
		}
		else if (!strcmp(Type, "Accept-Charset"))  //
		{
			strcpy(HttpRequest.AcceptCharset, Value);
		}
		else if (!strcmp(Type, "Accept-Encoding"))  //
		{
			strcpy(HttpRequest.AcceptEncoding, Value);
		}
		else if (!strcmp(Type, "Referer"))  //
		{
			strcpy(HttpRequest.Referer, Value);
		}
		else if (!strcmp(Type, "User-Agent"))  //
		{
			strcpy(HttpRequest.UserAgent, Value);
		}
		else if (!strcmp(Type, "Connection"))  //
		{
			strcpy(HttpRequest.Connection, Value);
		}
		else if (!strcmp(Type, "Date"))  //
		{
			strcpy(HttpRequest.Date, Value);
		}
		else if (!strcmp(Type, "Cookie"))
		{
			strcpy(HttpRequest.Cookie, Value);
		}
		memset(Type, 0, sizeof(Type));
		i++;   //取出下一个字段
	}
	WriteRequest();
	return true;
}



bool HttpResponseCallback(char *Content, int ContentLength)
{
	memset(&HttpResponse, 0, sizeof(HTTP_RESPONSE));
	char *buffer = Content;

	char Delimiter[3] = { (char)0x0d, (char)0x0a, '\0' };
	char Temp[3][100] = {0};     //存储http前三个字段
	char Temper[20][1000];
	char *Value, Type[50] = { 0 };   //字段的值和类型
	memset(Temper, 0, sizeof(Temper));
	int iNumber = 0, jLength;
	for (jLength = 0; jLength < ContentLength; jLength++)
	{
		if (!strncmp(buffer + jLength, " ", 1))
		{
			strncpy(Temp[iNumber], buffer, jLength);
			buffer = buffer + jLength + 1;
			ContentLength = ContentLength - jLength - 1;
			jLength = 0;
			iNumber++;
		}
		if (iNumber >= 2)
			break;
	}
	strcpy(HttpResponse.Request_Version, Temp[0]);
	strcpy(HttpResponse.Status_Code, Temp[1]);


	//将字符串切割后，存入缓存Temper中
	int i = 0, index = 0;
	while (true)
	{
		index = SubString(buffer, Delimiter);
		if (index == 0)
			break;
		strncpy(Temper[i], buffer, index);
		buffer = buffer + index + 2;
		i++;
	}
	strcpy(HttpResponse.Response_Phase, Temper[0]);//http头的第三个字段

		i = 1;
		while (true)
		{
			index = SubString(Temper[i], ": ");
			if (index == -1)
				break;
			strncpy(Type, Temper[i], index);
			Value = Temper[i] + index + 2;
			
			if (!strcmp(Type, "Content-Length"))
			{

				strcpy(HttpResponse.ContentLength, Value);
			}
			else if (!strcmp(Type, "Content-Type"))  //
			{
				strcpy(HttpResponse.ContentType, Value);
			}
			else if (!strcmp(Type, "Server"))  //
			{
				strcpy(HttpResponse.Server, Value);
			}
			else if (!strcmp(Type, "Date"))  //
			{
				strcpy(HttpResponse.Date, Value);
			}
			else if (!strcmp(Type, "Connection"))  //
			{
				strcpy(HttpResponse.Connection, Value);
			}
			else if (!strcmp(Type, "Location"))  //
			{
				strcpy(HttpResponse.Location, Value);
			}
			else if (!strcmp(Type, "Content-Encoding"))  //
			{
				strcpy(HttpResponse.ContentEncoding, Value);
			}
			else if (!strcmp(Type, "Content-Disposition"))  //
			{
				strcpy(HttpResponse.ContentDisposition, Value);
			}
			else if (!strcmp(Type, "Content-Language"))  //
			{
				strcpy(HttpResponse.ContentLanguage, Value);
			}
			else if (!strcmp(Type, "Refresh"))  //
			{
				strcpy(HttpResponse.Refresh, Value);
			}
			else if (!strcmp(Type, "Last-Modified"))  //
			{
				strcpy(HttpResponse.LastModified, Value);
			}
			else if (!strcmp(Type, "Transfer-Encoding"))  //
			{
				strcpy(HttpResponse.TransferEncoding, Value);
			}
			else if (!strcmp(Type, "Cache-Control"))  //
			{
				strcpy(HttpResponse.CacheControl, Value);
			}
			else if (!strcmp(Type, "Etag"))  //
			{
				strcpy(HttpResponse.Etag, Value);
			}
			memset(Type, 0, sizeof(Type));
			i++;
		}
		WriteResponse();
	return true;
}

void WriteRequest()
{
	ofstream ofile;
	ofile.open("http_request.txt", ios::app);
	if (strlen(HttpRequest.Request_Method) != 0)
	{
		ofile << setw(10) << setiosflags(ios::left) << HttpRequest.Request_Method;
	}
	else
		ofile << setw(10) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Request_URI) != 0)
	{
		ofile << setw(100) << setiosflags(ios::left) << HttpRequest.Request_URI;
	}
	else
		ofile << setw(100) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Accept) != 0)
	{
		ofile << setw(200) << setiosflags(ios::left) << HttpRequest.Accept;
	}
	else
		ofile << setw(200) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.AcceptCharset) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpRequest.AcceptCharset;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.AcceptEncoding) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpRequest.AcceptEncoding;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.AcceptLanguage) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpRequest.AcceptLanguage;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.CacheControl) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpRequest.CacheControl;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Connection) != 0)
	{
		ofile << setw(20) << setiosflags(ios::left) << HttpRequest.Connection;
	}
	else
		ofile << setw(20) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Cookie) != 0)
	{
		ofile << setw(300) << setiosflags(ios::left) << HttpRequest.Cookie;
	}
	else
		ofile << setw(300) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Date) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpRequest.Date;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Host) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpRequest.Host;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.Referer) != 0)
	{
		ofile << setw(100) << setiosflags(ios::left) << HttpRequest.Referer;
	}
	else
		ofile << setw(100) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpRequest.UserAgent) != 0)
	{
		ofile << setw(200) << setiosflags(ios::left) << HttpRequest.UserAgent;
	}
	else
		ofile << setw(200) << setiosflags(ios::left) << "NULL";
	ofile << endl;
	ofile.close();
}
void WriteResponse()
{
	ofstream ofile;
	ofile.open("http_response.txt", ios::app);
	if (strlen(HttpResponse.Status_Code) != 0)
	{
		ofile << setw(10) << setiosflags(ios::left) << HttpResponse.Status_Code;
	}
	else
		ofile << setw(10) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Response_Phase) != 0)
	{
		ofile << setw(10) << setiosflags(ios::left) << HttpResponse.Response_Phase;
	}
	else
		ofile << setw(10) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Request_Version) != 0)
	{
		ofile << setw(20) << setiosflags(ios::left) << HttpResponse.Request_Version;
	}
	else
		ofile << setw(20) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.CacheControl) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpResponse.CacheControl;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Connection) != 0)
	{
		ofile << setw(20) << setiosflags(ios::left) << HttpResponse.Connection;
	}
	else
		ofile << setw(20) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.ContentDisposition) != 0)
	{
		ofile << setw(100) << setiosflags(ios::left) << HttpResponse.ContentDisposition;
	}
	else
		ofile << setw(100) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.ContentEncoding) != 0)
	{
		ofile << setw(20) << setiosflags(ios::left) << HttpResponse.ContentEncoding;
	}
	else
		ofile << setw(20) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.ContentLanguage) != 0)
	{
		ofile << setw(20) << setiosflags(ios::left) << HttpResponse.ContentLanguage;
	}
	else
		ofile << setw(20) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.ContentLength) != 0)
	{
		ofile << setw(10) << setiosflags(ios::left) << HttpResponse.ContentLength;
	}
	else
		ofile << setw(10) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.ContentType) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpResponse.ContentType;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Date) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpResponse.Date;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Etag) != 0)
	{
		ofile << setw(200) << setiosflags(ios::left) << HttpResponse.Etag;
	}
	else
		ofile << setw(200) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.LastModified) != 0)
	{
		ofile << setw(50) << setiosflags(ios::left) << HttpResponse.LastModified;
	}
	else
		ofile << setw(50) << setiosflags(ios::left) << "NULL";


	if (strlen(HttpResponse.Location) != 0)
	{
		ofile << setw(100) << setiosflags(ios::left) << HttpResponse.Location;
	}
	else
		ofile << setw(100) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Refresh) != 0)
	{
		ofile << setw(5) << setiosflags(ios::left) << HttpResponse.Refresh;
	}
	else
		ofile << setw(5) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.Server) != 0)
	{
		ofile << setw(200) << setiosflags(ios::left) << HttpResponse.Server;
	}
	else
		ofile << setw(200) << setiosflags(ios::left) << "NULL";
	if (strlen(HttpResponse.TransferEncoding) != 0)
	{
		ofile << setw(20) << setiosflags(ios::left) << HttpResponse.TransferEncoding;
	}
	else
		ofile << setw(20) << setiosflags(ios::left) << "NULL";
	
	ofile << endl;
	ofile.close();
}