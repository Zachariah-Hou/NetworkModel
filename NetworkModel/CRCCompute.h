#pragma once

#include <stdint.h>

template <typename TYPE> class CRC
{
public:
	CRC();
	CRC(TYPE polynomial, TYPE init_remainder, TYPE final_xor_value);
	void build(TYPE polynomial, TYPE init_remainder, TYPE final_xor_value);
	/**
	* Compute the CRC checksum of a binary message block.
	* @para message, 用来计算的数据
	* @para nBytes, 数据的长度
	*/
	TYPE crcCompute(char * message, unsigned int nBytes);
	TYPE crcCompute(char * message, unsigned int nBytes, bool reinit);
protected:
	TYPE m_polynomial;
	TYPE m_initial_remainder;
	TYPE m_final_xor_value;
	TYPE m_remainder;
	TYPE crcTable[256];
	int m_width;
	int m_topbit;
	/**
	* Initialize the CRC lookup table.
	* This table is used by crcCompute() to make CRC computation faster.
	*/
	void crcInit(void);
};



class CRC32 : public CRC<uint32_t>
{
public:
	enum CRC32_TYPE
	{
		eADCCP, ePKZIP, eCRC32, eAAL5, eDECT_B, eB_CRC32, eBZIP2, eAUTOSAR,
		eCRC32C, eCRC32D, eMPEG2, ePOSIX, eCKSUM, eCRC32Q, eJAMCRC, eXFER
	};
	CRC32(CRC32_TYPE type);
};
