#include "stdafx.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

/*
1 �ڸ�ip_header����У���֮ǰ   ���Ȱ�ip_header��checksum�ֶ���Ϊ0
2 ����õ�checksum֮��   ��ֵʱҪת��Ϊ�����ֽ���: ip_header.checksum = htons(checksum);

���㷽����20�ֽڵ�ip���ݰ�
��ÿ16λ���һ���ֲ��������
�ڽ��������Ľ�λ�ӵ���16λ��
��ȡ����checksum
*/

/*udp_checksum�ļ���ʱ��Ҫ��αͷ��+UDPͷ��+���ݲ���
αͷ����4 byteԴip��ַ + 4 byteĿ��ip��ַ + 0x00 + 1 byteЭ�� + udp���ȣ�udp��ͷ����+���ݳ��ȣ�
udp��ͷ��2 byteԴ�˿� + 2 byteĿ�Ķ˿� + 2 byte udp��������ͷ�Դ��ĳ��ȣ� + 0x0000(checksum)
���ݲ���

���㷽��ͬget_ip_checksum
*/

struct ip_header
{
	unsigned char ihl; //ip header length
	unsigned char version; //version
	unsigned char tos; //type of service
	unsigned short tot_len;  // 16λip���ݱ��ܳ��� total length
	unsigned short id;  //identification
	unsigned short frag_off; // fragment offset
	unsigned char ttl; //time to live
	unsigned char protocol; //protocol type
	unsigned short check; //check sum
	unsigned int saddr; //source address
	unsigned int daddr; // destination address
};

unsigned int IpCheckSum(struct ip_header *ip_head_buffer)
//unsigned int IpCheckSum(struct ip_header *ip_head_buffer, int ipher_len)
{
	ip_head_buffer->check = 0;
	char *temp_hdr = (char *)ip_head_buffer;
	unsigned int temp_high = 0, temp_low = 0, result = 0;
	int i = 0;
	/*���ɹ�20���ֽڵ�ip��ͷ*/
	for (i = 0; i < 10; i++) {
		temp_high = *((char *)(temp_hdr + 2 * i)) & 0x00ff;// ��8λ
		printf("%02x ", temp_high);
		temp_low = *((char *)(temp_hdr + 2 * i + 1)) & 0x00ff;
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

short int UdpCheckSum(int ip_src_addr, int ip_dst_addr, int *udp_buffer, int udp_size)
{
	//����αͷ��
	unsigned char rawBuffer[200000];
	struct  pseudo_hdr
	{
		int src; // Դip��ַ��32bit
		int dst; //Ŀ��ip��ַ��32bit
		char mbz; //ȫ0�� 8bit
		char protocol; //Э���ֶ�, 8bit
		short int len; //udp����
	};
	struct  pseudo_hdr *phead;
	phead = (struct pseudo_hdr *) rawBuffer;//��������ת���ɽṹ��ָ��
	int phead_len = sizeof(struct pseudo_hdr);

	//αͷ����ֵ����������phead_len����
	short int check_sum = 0;
	phead->src = ip_src_addr;
	phead->dst = ip_dst_addr;
	phead->mbz = 0;
	phead->protocol = 17; //udpЭ�����Ϊ17
	phead->len = htons(udp_size);

	//����У���
	memcpy(rawBuffer + phead_len, udp_buffer, udp_size);
	//check_sum = IpCheckSum((struct ip_header*)rawBuffer, phead_len + udp_size);
	check_sum = IpCheckSum((struct ip_header*)rawBuffer);
	return check_sum;
}


//int main()
//{
//	struct ip_header *iphdr;
//	iphdr = (struct ip_header *)malloc(sizeof(struct ip_header));
//	printf("build IP header\n");
//	iphdr->version = 4;                  /* we create an IP header version 4 */
//	iphdr->ihl = 5;                      /* min. IPv4 header length (in 32-bit words) */
//	int len = 0;
//	len += iphdr->ihl * 4;
//	iphdr->tos = 0;                      /* TOS is not important for the example */
//	iphdr->tot_len = htons(len + 18);
//	printf("%02x\n", len + 18);
//	iphdr->id = 0;                       /* ID is not important for the example */
//	iphdr->frag_off = 0;                 /* No packet fragmentation */
//	iphdr->ttl = 1;                      /* TTL is not important for the example */
//	iphdr->protocol = 134;               /* protocol number */
//	iphdr->check = 0;
//	iphdr->saddr = htonl(0x01020304);    /* source address 1.2.3.4 */
//	iphdr->daddr = htonl(0x05060708);    /* destination addr. 5.6.7.8 */
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