#ifndef _HTTP_H_
#define _HTTP_H_

#include "sniffer.h"

struct HTTP_REQUEST
{
	char Request_Method[20];
	char Request_URI[100];
	char Request_Version[20];
	char CacheControl[50];
	char Host[50];
	char Accept[200];
	char AcceptLanguage[50];
	char AcceptCharset[50];
	char AcceptEncoding[50];
	char Referer[100];
	char UserAgent[200];
	char Connection[20];
	char Date[50];
	char Cookie[300];
};

struct HTTP_RESPONSE
{
	char Request_Version[20];
	char Status_Code[10];
	char Response_Phase[10];
	char ContentLength[10];
	char ContentType[50];
	char Server[200];
	char Date[50];
	char Connection[20];
	char Location[100];
	char ContentEncoding[20];
	char ContentDisposition[100];
	char ContentLanguage[20];
	char Refresh[5];
	char LastModified[50];
	char TransferEncoding[20];
	char CacheControl[50];
	char Etag[200];
};

extern HTTP_REQUEST HttpRequest;
extern HTTP_RESPONSE HttpResponse;

bool HttpRequestCallback(char *Content,int ContentLength);

bool HttpResponseCallback(char *Content,int ContentLength);

void WriteRequest();

void WriteResponse();
#endif