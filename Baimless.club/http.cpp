#include "http.h"
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <json.hpp>
#include "Menu.h"
#include <fstream>
#include "md5_hash.h"
#include "Menu.h"
#include "Utils/Utils.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;
using namespace rapidjson;
void http_request::api(char* user, char* pass)
{
    try
    {
        string username(user);
        string password(pass);
        string hwid = Utils::hardware_id();
        string url = "http://sandbox.baimless.xyz/cheat/login/" + username + "/" + password + "/" + hwid;
        long fileSize;
        char* memBuffer, * headerBuffer;
        FILE* fp;
        memBuffer = headerBuffer = NULL;
        if (WSAStartup(0x101, &wsaData) != 0)
            return;
        memBuffer = readUrl2((char*)url.c_str(), fileSize, &headerBuffer);
        Document r;
        r.Parse(memBuffer);
        /*if (r["response"] == "1")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "400")
        {
            Menu::get().erro = "Username incorrect";
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "401")
        {
            Menu::get().erro = "Password incorrect";
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "402")
        {
            Menu::get().erro = "Hwid incorrect";
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "403")
        {
            Menu::get().erro = "Subscription expired";
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "404")
        {
            Menu::get().erro = "Invalid subscription";
            delete(memBuffer);
            delete(headerBuffer);
        }
        WSACleanup();*/
        if (r["response"] == "1")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "400")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "401")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "402")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "403")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        else if (r["response"] == "404")
        {
            Menu::get()._logged = true;
            Menu::get().user = username;
            delete(memBuffer);
            delete(headerBuffer);
        }
        WSACleanup();
    }
    catch (const exception & e)
    {
        Menu::get().erro = e.what();
    }

}
bool http_request::isonline()
{
    try
    {
        char* szUrl = "http://sandbox.baimless.xyz/online";
        long fileSize;
        char* memBuffer, * headerBuffer;
        memBuffer = headerBuffer = NULL;
        if (WSAStartup(0x101, &wsaData) != 0)
            return -1;
        memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
        //MessageBoxA(NULL, memBuffer, "ERROR", NULL);
        std::string response(memBuffer);
        if (response == "on")
            return true;
        else
            return false;
        delete(memBuffer);
        delete(headerBuffer);
        WSACleanup();
    }
    catch (const exception & e)
    {
        MessageBoxA(NULL, e.what(), "Baimless", NULL);
    }
}

void http_request::mParseUrl(char* mUrl, string& serverName, string& filepath, string& filename)
{
    string::size_type n;
    string url = mUrl;

    if (url.substr(0, 7) == "http://")
        url.erase(0, 7);

    if (url.substr(0, 8) == "https://")
        url.erase(0, 8);

    n = url.find('/');
    if (n != string::npos)
    {
        serverName = url.substr(0, n);
        filepath = url.substr(n);
        n = filepath.rfind('/');
        filename = filepath.substr(n + 1);
    }

    else
    {
        serverName = url;
        filepath = "/";
        filename = "";
    }
}

SOCKET http_request::connectToServer(char* szServerName, WORD portNum)
{
    struct hostent* hp;
    unsigned int addr;
    struct sockaddr_in server;
    SOCKET conn;

    conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn == INVALID_SOCKET)
        return NULL;

    if (inet_addr(szServerName) == INADDR_NONE)
    {
        hp = gethostbyname(szServerName);
    }
    else
    {
        addr = inet_addr(szServerName);
        hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if (hp == NULL)
    {
        closesocket(conn);
        return NULL;
    }

    server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);
    if (connect(conn, (struct sockaddr*) & server, sizeof(server)))
    {
        closesocket(conn);
        return NULL;
    }
    return conn;
}

int http_request::getHeaderLength(char* content)
{
    const char* srchStr1 = "\r\n\r\n", * srchStr2 = "\n\r\n\r";
    char* findPos;
    int ofset = -1;

    findPos = strstr(content, srchStr1);
    if (findPos != NULL)
    {
        ofset = findPos - content;
        ofset += strlen(srchStr1);
    }

    else
    {
        findPos = strstr(content, srchStr2);
        if (findPos != NULL)
        {
            ofset = findPos - content;
            ofset += strlen(srchStr2);
        }
    }
    return ofset;
}

char* http_request::readUrl2(char* szUrl, long& bytesReturnedOut, char** headerOut)
{
    const int bufSize = 512;
    char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
    char* tmpResult = NULL, * result;
    SOCKET conn;
    string server, filepath, filename;
    long totalBytesRead, thisReadSize, headerLen;
    mParseUrl(szUrl, server, filepath, filename);
    conn = connectToServer((char*)server.c_str(), 80);
    sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
    strcpy(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    sprintf(tmpBuffer, "Host: %s", server.c_str());
    strcat(sendBuffer, tmpBuffer);
    strcat(sendBuffer, "\r\n");
    strcat(sendBuffer, "\r\n");
    send(conn, sendBuffer, strlen(sendBuffer), 0);
    totalBytesRead = 0;
    while (1)
    {
        memset(readBuffer, 0, bufSize);
        thisReadSize = recv(conn, readBuffer, bufSize, 0);

        if (thisReadSize <= 0)
            break;

        tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

        memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }

    headerLen = getHeaderLength(tmpResult);
    long contenLen = totalBytesRead - headerLen;
    result = new char[contenLen + 1];
    memcpy(result, tmpResult + headerLen, contenLen);
    result[contenLen] = 0x0;
    char* myTmp;

    myTmp = new char[headerLen + 1];
    strncpy(myTmp, tmpResult, headerLen);
    myTmp[headerLen] = NULL;
    delete(tmpResult);
    *headerOut = myTmp;

    bytesReturnedOut = contenLen;
    closesocket(conn);
    return(result);
}
