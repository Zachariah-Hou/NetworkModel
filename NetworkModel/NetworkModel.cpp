// NetworkModel.cpp: 定义控制台应用程序的入口点。

#include "CRCCompute.h"
#include "Checksum.cpp"
#include "stdafx.h"

#define SERVER_PORT 12345
#define BUF_SIZE 4096

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

using namespace std;

#define IP_Version 4
#define PPPFlag 0x7E
#define PPPCon 0x03 // PPP控制域
#define PPPPro 0x21 // PPP协议域

const string protocal = "TCP";
string ip1 = "172.20.10.2";
string ip2 = "172.20.10.8";
int ip1_num = 2 + 10 * 8 + 20 * 8 * 9 + 172 * 8 * 8 * 8;
int ip2_num = 8 + 10 * 8 + 20 * 8 * 9 + 172 * 8 * 8 * 8;
const string mac1 = "11-11-11-11-11-11";
const string mac2 = "11-11-11-11-11-12";
const int buffer[7] = { 0 };

/// TODO: 帧打印
void
printFrame(string& frame, int* flag, int flagLen)
{
	string UPPER_LEFT_CORNER = "┌";
	string LOWER_LEFT_CORNER = "└";
	string UPPER_RIGHT_CORNER = "┐";
	string LOWER_RIGHT_CORNER = "┘";
	string UPPER_MIDDLE_CORNER = "┬";
	string LOWER_MIDDLE_CORNER = "┴";
	string VERTICAL_LINE = "─";
	string HORIZONTAL_LINE = "│";

	size_t strLen = frame.length();
	// size_t flagLen = sizeof(flag) / sizeof(int);

	// int flag = 0b00;
	// for (size_t i = 0; i < strLen; i++) {
	//	if (frame[i] == '&') {
	//		flag += (int)pow(2, i);
	//	}
	//}

	cout << UPPER_LEFT_CORNER;
	for (int i = 0; i < flagLen - 1; i++) {
		for (int j = 0; j < flag[i]; j++) {
			cout << VERTICAL_LINE;
		}

		cout << UPPER_MIDDLE_CORNER;
	}

	for (int j = 0; j < flag[flagLen - 1]; j++) {
		cout << VERTICAL_LINE;
	}

	cout << UPPER_RIGHT_CORNER << endl << HORIZONTAL_LINE;
	int currHead = 0;
	for (int i = 0; i < flagLen; i++) {
		for (int j = currHead; j < flag[i] + currHead; j++) {
			cout << frame[j];
		}

		currHead += flag[i];
		cout << HORIZONTAL_LINE;
	}

	cout << endl << LOWER_LEFT_CORNER;
	for (int i = 0; i < flagLen - 1; i++) {
		for (int j = 0; j < flag[i]; j++) {
			cout << VERTICAL_LINE;
		}

		cout << LOWER_MIDDLE_CORNER;
	}

	for (int j = 0; j < flag[flagLen - 1]; j++) {
		cout << VERTICAL_LINE;
	}

	cout << LOWER_RIGHT_CORNER << endl;
}


// 传输客户端
int
client(string message)
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

char
bitToAsc(char c[7]) // 将字符转换成其ASCII码
{
	int a[7] = { 0 };
	for (int i = 0; i < 7; i++) {
		a[i] = c[i] - '0';
		// cout << a[i];
	}
	// cout << endl;
	char code =
		a[6] + 2 * a[5] + 4 * a[4] + 8 * a[3] + 16 * a[2] + 32 * a[1] + 64 * a[0];
	return code;
}

// 应用层封装
string
applicationLayer()
{
	string data;
	cout << "Please input the data to be send: ";
	cin >> data;
	return data;
}

// 传输层封装
string
transportLayer(string& str)
{
	// 传输层UDP协议
	srand((unsigned)time(NULL));
	int srcPort = rand() % 8192 + 1024;
	int dstPort = 8080;
	int msgLength = 64 + sizeof(str);

	int* udp_buffer = new int[256];
	int checkSum = UdpCheckSum(ip1_num, ip2_num, udp_buffer, msgLength);

	char* tmp = new char[256];

	string lev4 = itoa(srcPort, tmp, 16);
	lev4 += itoa(srcPort, tmp, 16);
	lev4 += itoa(msgLength, tmp, 16);
	lev4 += itoa(checkSum, tmp, 16);
	lev4 += str;

	return lev4;
}

// 网络层封装
string
networkLayer(string& str)
{
	int version = 0b0100;
	int headerLength = 0b0101;
	int typeofService = 0b00000000;
	size_t totalLength = headerLength + str.length();
	int identification = 0;
	int flag = 0b010;

	/// TODO: IP数据报分片
}

// 数据链路层封装
/// TODO: 修改校验封装
string
dataLinkLayer(string& str)
{
	// 数据链路层
	char* str_char = new char[str.length()];
	int i;
	for (i = 0; i < str.length(); i++) {
		str_char[i] = str[i];
	}
	str_char[i] = '\0';

	CRC32 crc32(CRC32::eADCCP);
	unsigned short c1;
	c1 = crc32.crcCompute(str_char, str.length());

	char* tmp = new char[256];
	str = itoa(PPPFlag, tmp, 16) + ip1 + itoa(PPPCon, tmp, 16) + str +
		itoa(c1, tmp, 16) + itoa(PPPFlag, tmp, 16);
	cout << "数据链路层封装：" << endl;
	for (i = 0; i < str.length(); i++)
		cout << str[i];
	cout << endl;
}

// 物理层封装
string
physicalLayer(string& str)
{
	// 物理层
	cout << "物理层开始处理 " << endl;
	int* lev5_asc = new int[str.length()];
	for (int i = 0; i < str.length(); i++) {
		lev5_asc[i] = int(lev4[i] - '\0');
	}

	string lev5 = "";
	char* tmp = new char[256];
	for (int i = 0; i < str.length(); i++) {
		lev5 += itoa(lev5_asc[i], tmp, 2);
	}
}

void
send()
{
	//string data;
	//int i;
	//cout << "请输入要传输的数据: ";
	//cin >> data;

	//string lev1 = data;
	//cout << "应用层处理: " << endl;
	//for (i = 0; i < lev1.length(); i++)
	//	cout << lev1[i];
	//cout << endl;

	string lev1 = applicationLayer();
	string lev2 = transportLayer(lev1);
	string lev3 = networkLayer(lev2);
	string lev4 = dataLinkLayer(lev3);
	string lev5 = physicalLayer(lev4);

	client(lev5);

	// 网络层
	// TODO: 网络层封装
	//string lev3 = "4" + mac1 + "<" + protocal + ">" + mac2 + "$" + lev2;
	//cout << "网络层封装: " << endl;
	//for (i = 0; i < lev3.length(); i++)
	//    cout << lev3[i];
	//cout << endl;

	//time_t timer;
	//struct tm* tblock;
	//timer = time(NULL);
	//tblock = localtime(&timer);
	//string asct = asctime(tblock); // asct的长度为25
	//string lev4 = "static_head=" + asct + "$" + lev3;




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

int
main()
{
	send();
	return 0;
}