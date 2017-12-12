#include "stdafx.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

/*
1 在给ip_header计算校验和之前   首先把ip_header的checksum字段置为0
2 计算得到checksum之后   赋值时要转换为网络字节序: ip_header.checksum =
htons(checksum);

计算方法：20字节的ip数据包
①每16位组成一个字并依次相加
②将计算结果的进位加到低16位上
③取反得checksum
*/

/*udp_checksum的计算时需要：伪头部+UDP头部+数据部分
伪头部：4 byte源ip地址 + 4 byte目的ip地址 + 0x00 + 1 byte协议 +
udp长度（udp包头长度+数据长度） udp包头：2 byte源端口 + 2 byte目的端口 + 2 byte
udp包长（包头自带的长度） + 0x0000(checksum) 数据部分

计算方法同get_ip_checksum
*/

struct ip_header
{
    unsigned char ihl;       // ip header length
    unsigned char version;   // version
    unsigned char tos;       // type of service
    unsigned short tot_len;  // 16位ip数据报总长度 total length
    unsigned short id;       // identification
    unsigned short frag_off; // fragment offset
    unsigned char ttl;       // time to live
    unsigned char protocol;  // protocol type
    unsigned short check;    // check sum
    unsigned int saddr;      // source address
    unsigned int daddr;      // destination address
};

unsigned int
IpCheckSum(struct ip_header* ip_head_buffer)
// unsigned int IpCheckSum(struct ip_header *ip_head_buffer, int ipher_len)
{
    ip_head_buffer->check = 0;
    char* temp_hdr = (char*)ip_head_buffer;
    unsigned int temp_high = 0, temp_low = 0, result = 0;
    int i = 0;
    /*生成共20个字节的ip包头*/
    for (i = 0; i < 10; i++) {
        temp_high = *((char*)(temp_hdr + 2 * i)) & 0x00ff; // 高8位
        printf("%02x ", temp_high);
        temp_low = *((char*)(temp_hdr + 2 * i + 1)) & 0x00ff;
        printf("%02x ", temp_low);
        result = result + ((temp_high << 8) + temp_low);
    }
    printf("\n");
    while (result & 0xffff0000) {
        result = ((result & 0xffff0000) >> 16) + (result & 0x0000ffff);
    }
    result = 0xffff - result;
    return result;
}

short int
UdpCheckSum(int ip_src_addr, int ip_dst_addr, int* udp_buffer, int udp_size)
{
    // 定义伪头部
    unsigned char rawBuffer[200000];
    struct pseudo_hdr
    {
        int src;       // 源ip地址，32bit
        int dst;       // 目的ip地址，32bit
        char mbz;      // 全0， 8bit
        char protocol; // 协议字段, 8bit
        short int len; // udp长度
    };
    struct pseudo_hdr* phead;
    phead = (struct pseudo_hdr*)rawBuffer; //缓存数组转换成结构体指针
    int phead_len = sizeof(struct pseudo_hdr);

    // 伪头部赋值，即数组中phead_len部分
    short int check_sum = 0;
    phead->src = ip_src_addr;
    phead->dst = ip_dst_addr;
    phead->mbz = 0;
    phead->protocol = 17; // udp协议代码为17
    phead->len = htons(udp_size);

    // 计算校验和
    memcpy(rawBuffer + phead_len, udp_buffer, udp_size);
    // check_sum = IpCheckSum((struct ip_header*)rawBuffer, phead_len +
    // udp_size);
    check_sum = IpCheckSum((struct ip_header*)rawBuffer);
    return check_sum;
}

// int main()
//{
//	struct ip_header *iphdr;
//	iphdr = (struct ip_header *)malloc(sizeof(struct ip_header));
//	printf("build IP header\n");
//	iphdr->version = 4;                  /* we create an IP header version 4
//*/ 	iphdr->ihl = 5;                      /* min. IPv4 header length (in
// 32-bit words) */ 	int len = 0; 	len += iphdr->ihl * 4; 	iphdr->tos = 0;
///* TOS is not important for the example */ 	iphdr->tot_len = htons(len +
// 18); 	printf("%02x\n", len + 18); 	iphdr->id = 0; /*  ID is not
// important for the example */ 	iphdr->frag_off = 0;
///* No packet fragmentation */ 	iphdr->ttl = 1;                      /*
// TTL is not important for the example */ 	iphdr->protocol = 134;
///* protocol number */ 	iphdr->check = 0; 	iphdr->saddr =
// htonl(0x01020304);    /* source address 1.2.3.4 */ 	iphdr->daddr =
// htonl(0x05060708);    /* destination addr. 5.6.7.8 */
//
//	//iphdr->check = IpCheckSum(iphdr, len);
//	iphdr->check = IpCheckSum(iphdr);
//	printf("%02x\n", iphdr->check);
//
//	//iphdr->check = get_ip_checksum((char*) iphdr);
//	return 0;
//}

/*
unsigned short get_ip_checksum(char * ip_hdr)
{
char* ptr_data = ip_hdr;
unsigned long tmp = 0;
unsigned long sum = 0;
for (int i = 0; i < 20; i += 2)
{
tmp += (unsigned char)ptr_data[i] << 8;
tmp += (unsigned char)ptr_data[i + 1];
sum += tmp;
tmp = 0;
}
unsigned short lWord = sum & 0x0000FFFF;
unsigned short hWord = sum >> 16;
unsigned short checksum = lWord + hWord;
checksum = ~checksum;
return checksum;
}
*/
