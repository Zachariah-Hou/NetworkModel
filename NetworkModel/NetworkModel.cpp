// NetworkModel.cpp: 定义控制台应用程序的入口点。

#include "stdafx.h"
#include "CRCCompute.h"

#define SERVER_PORT 12345
#define	BUF_SIZE 4096

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

#define IP_Version 4
#define PPPFlag 0x7E
#define PPPCon 0x03 //PPP控制域
#define PPPPro 0x0021 //PPP协议域


const string protocal = "TCP";
string ip1 = "172.20.10.2";
string ip2 = "172.20.10.8";
const string mac1 = "11-11-11-11-11-11";
const string mac2 = "11-11-11-11-11-12";
const int buffer[7] = { 0 };


int client(string message)
{
	WSADATA wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 新建客户端socket
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	const char* ip2_char = ip2.data();
	// 定义要连接的服务端地址
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(6000); // 连接端口6000
	sin.sin_addr.S_un.S_addr = inet_addr(ip2_char);
	int len = sizeof(sin);

	const char* sendData = message.data();
	//char* sendData = "来自客户端的数据包\n";
	sendto(sclient, sendData, strlen(sendData), 0, (sockaddr*)&sin, len);

	char recvData[255];
	int ret = recvfrom(sclient, recvData, 255, 0, (sockaddr*)&sin, &len);
	if (ret > 0) {
		recvData[ret] = 0x00;
		printf(recvData);
	}

	closesocket(sclient);
	WSACleanup();

	return 0;
}

char bitToAsc(char c[7]) // 将字符转换成其ASCII码
{
	int a[7] = { 0 };
	for (int i = 0; i < 7; i++) {
		a[i] = c[i] - '0';
		//cout << a[i];
	}
	//cout << endl;
	char code = a[6] + 2 * a[5] + 4 * a[4] + 8 * a[3] + 16 * a[2] + 32 * a[1] + 64 * a[0];
	return code;
}

void send()
{
	string data;
	int i;
	cout << "请输入要传输的数据: ";
	cin >> data;

	string lev1 = data;
	cout << "应用层处理: " << endl;
	for (i = 0; i < lev1.length(); i++)
		cout << lev1[i];
	cout << endl;

	// 传输层UDP协议
	string lev2 = "trans_head=" + ip1 + "<:>" + ip2 + "$" + lev1;
	cout << "传输层封装: " << endl;
	for (i = 0; i < lev2.length(); i++)
		cout << lev2[i];
	cout << endl;

	// 网络层
	// TODO: 网络层封装
	string lev3 = "4" + mac1 + "<" + protocal + ">" + mac2 + "$" + lev2;
	cout << "网络层封装: " << endl;
	for (i = 0; i < lev3.length(); i++)
		cout << lev3[i];
	cout << endl;

	time_t timer;
	struct tm* tblock;
	timer = time(NULL);
	tblock = localtime(&timer);
	string asct = asctime(tblock); //asct的长度为25
	string lev4 = "static_head=" + asct + "$" + lev3;

	// 数据链路层
	char* lev4_char = new char[lev4.length()];
	for (int i = 0; i < lev4.length(); i++) {
		lev4_char[i] = lev4[i];
	}
	lev4_char[i] = '\0';

	CRC32 crc32(CRC32::eADCCP);
	unsigned short c1;
	c1 = crc32.crcCompute(lev4_char, lev4.length());

	char* tmp = new char[256];
	lev4 = itoa(PPPFlag, tmp, 16) + ip1 + itoa(PPPCon, tmp, 16) + lev4 + itoa(c1, tmp, 16) + itoa(PPPFlag, tmp, 16);
	cout << "数据链路层封装：" << endl;
	for (i = 0; i < lev4.length(); i++)
		cout << lev4[i];
	cout << endl;

	// 物理层
	cout << "物理层开始处理 " << endl;
	int* lev5_asc = new int[lev4.length()];
	for (int i = 0; i < lev4.length(); i++) {
		lev5_asc[i] = int(lev4[i] - '\0');
	}

	string lev5 = "";
	for (int i = 0; i < lev4.length(); i++) {
		lev5 += itoa(lev5_asc[i], tmp, 2);
	}
	client(lev5);

	/*
	FILE* fp = fopen("data.txt", "w");
	for (i = 0; i < lev4.length(); i++)
	{
	int t = lev4[i];
	int origin_num = 64;
	while (origin_num)
	{
	if (t >= origin_num)
	{
	fprintf(fp, "1");
	t = t - origin_num;
	}
	else
	fprintf(fp, "0");
	origin_num /= 2;
	}
	}
	fclose(fp);
	*/

	cout << "处理完成，请接受数据！" << endl;
}

int main()
{
    return 0;
}

