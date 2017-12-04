#include "stdafx.h"
#include "CRCCompute.h"

template <typename TYPE>
CRC<TYPE>::CRC()
{
	m_width = 8 * sizeof(TYPE);
	m_topbit = 1 << (m_width - 1);
}

template <typename TYPE>
CRC<TYPE>::CRC(TYPE polynomial, TYPE init_remainder, TYPE final_xor_value)
{
	m_width = 8 * sizeof(TYPE);
	m_topbit = 1 << (m_width - 1);
	m_polynomial = polynomial;
	m_initial_remainder = init_remainder;
	m_final_xor_value = final_xor_value;

	crcInit();
}


template <typename TYPE>
void CRC<TYPE>::build(TYPE polynomial, TYPE init_remainder, TYPE final_xor_value)
{
	m_polynomial = polynomial;
	m_initial_remainder = init_remainder;
	m_final_xor_value = final_xor_value;

	crcInit();
}

template <typename TYPE>
TYPE CRC<TYPE>::crcCompute(char * message, unsigned int nBytes)
{
	unsigned int offset;
	unsigned char byte;
	TYPE remainder = m_initial_remainder;
	/* Divide the message by the polynomial, a byte at a time. */
	for (offset = 0; offset < nBytes; offset++) {
		byte = (remainder >> (m_width - 8)) ^ message[offset];
		remainder = crcTable[byte] ^ (remainder << 8);
	}
	/* The final remainder is the CRC result. */
	return (remainder ^ m_final_xor_value);
}

template <typename TYPE>
TYPE CRC<TYPE>::crcCompute(char * message, unsigned int nBytes, bool reinit)
{
	unsigned int offset;
	unsigned char byte;
	if (reinit) {
		m_remainder = m_initial_remainder;
	}
	/* Divide the message by the polynomial, a byte at a time. */
	for (offset = 0; offset < nBytes; offset++) {
		byte = (m_remainder >> (m_width - 8)) ^ message[offset];
		m_remainder = crcTable[byte] ^ (m_remainder << 8);
	}
	/* The final remainder is the CRC result. */
	return (m_remainder ^ m_final_xor_value);
}

template <typename TYPE>
void CRC<TYPE>::crcInit(void)
{
	TYPE remainder;
	TYPE dividend;
	int bit;
	/* Perform binary long division, a bit at a time. */
	for (dividend = 0; dividend < 256; dividend++) {
		/* Initialize the remainder.  */
		remainder = dividend << (m_width - 8);
		/* Shift and XOR with the polynomial.   */
		for (bit = 0; bit < 8; bit++) {
			/* Try to divide the current data bit.  */
			if (remainder & m_topbit) {
				remainder = (remainder << 1) ^ m_polynomial;
			} else {
				remainder = remainder << 1;
			}
		}
		/* Save the result in the table. */
		crcTable[dividend] = remainder;
	}
}

CRC32::CRC32(CRC32_TYPE type)
{
	switch (type) {
		case eADCCP:
		case ePKZIP:
		case eCRC32:
		case eBZIP2:
		case eAAL5:
		case eDECT_B:
		case eB_CRC32:
			m_polynomial = 0x04c11db7;
			m_initial_remainder = 0xFFFFFFFF;
			m_final_xor_value = 0xFFFFFFFF;
			break;
		case eAUTOSAR:
			m_polynomial = 0xf4acfb13;
			m_initial_remainder = 0xFFFFFFFF;
			m_final_xor_value = 0xFFFFFFFF;
			break;
		case eCRC32C:
			m_polynomial = 0x1edc6f41;
			m_initial_remainder = 0xFFFFFFFF;
			m_final_xor_value = 0xFFFFFFFF;
			break;
		case eCRC32D:
			m_polynomial = 0xa833982b;
			m_initial_remainder = 0xFFFFFFFF;
			m_final_xor_value = 0xFFFFFFFF;
			break;
		case eMPEG2:
		case eJAMCRC:
			m_polynomial = 0x04c11db7;
			m_initial_remainder = 0xFFFFFFFF;
			m_final_xor_value = 0x00000000;
			break;
		case ePOSIX:
		case eCKSUM:
			m_polynomial = 0x04c11db7;
			m_initial_remainder = 0x00000000;
			m_final_xor_value = 0xFFFFFFFF;
			break;
		case eCRC32Q:
			m_polynomial = 0x814141ab;
			m_initial_remainder = 0x00000000;
			m_final_xor_value = 0x00000000;
			break;
		case eXFER:
			m_polynomial = 0x000000af;
			m_initial_remainder = 0x00000000;
			m_final_xor_value = 0x00000000;
			break;
		default:
			m_polynomial = 0x04C11DB7;
			m_initial_remainder = 0xFFFFFFFF;
			m_final_xor_value = 0xFFFFFFFF;
			break;
	}
	crcInit();
}