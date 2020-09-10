//
// Created by hwp on 2017/11/23.
//

#include <memory.h>
#include <stdint.h>

int mm_string_strstr(char *big, char *key, int where_start) {
	char *p = big;
	unsigned len = strlen(key);
	for (; (p = (char *) strchr(p, (char) *key)) != 0; p++) {
		if ((p - big) < where_start)continue;
		if (strncmp(p, key, len) == 0)return (p - big);//return (char*)p;
	}
	return -1;
}

// 根据开始结束位置截取字符, 首位置为 0。返回截取出的字符长度。with_0=1 时结尾补'\0'
int mm_string_cut_by_where_start_and_end(const uint8_t *s_in, int start, int end, uint8_t *s_out, int with_0) {
	int n = end - start;
	memcpy(s_out, (s_in + start), n);
	if (with_0 == 1)*(s_out + n) = '\0';
	return n;
}


// 可打印字符串转换为字节数据
// 如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// pSrc: 源字符串指针
// pDst: 目标数据指针
// nSrcLength: 源字符串长度
// 返回: 目标数据长度
int string_to_bytes(const char* pSrc, uint8_t * pDst, int nSrcLength)
{
	for(int i=0; i<nSrcLength; i+=2) {
		// 输出高4位
		*pDst = (*pSrc >= '0' && *pSrc <= '9' ? *pSrc - '0' : *pSrc - 'a' + 10) << 4;

		pSrc++;

		// 输出低4位
		if(*pSrc>='0' && *pSrc<='9') {
			*pDst |= *pSrc - '0';
		} else {
			*pDst |= *pSrc - 'a' + 10;
		}
		pSrc++;
		pDst++;
	}

	// 返回目标数据长度
	return nSrcLength / 2;
}

// 字节数据转换为可打印字符串
// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01"
// pSrc: 源数据指针
// pDst: 目标字符串指针
// nSrcLength: 源数据长度
// 返回: 目标字符串长度
int bytes_to_string(const uint8_t * pSrc, uint8_t * pDst, int nSrcLength)
{
	const char tab[]="0123456789abcdef";    // 0x0-0xf的字符查找表

	for(int i=0; i<nSrcLength; i++) {
		// 输出低4位
		*pDst++ = tab[*pSrc >> 4];

		// 输出高4位
		*pDst++ = tab[*pSrc & 0x0f];

		pSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return nSrcLength * 2;
}

int itoa_bin(uint16_t data, char *str)
{
	if(str == NULL)
		return -1;

	char *start = str;

	while(data) {
		if(data & 0x1)
			*str++ = 0x31;
		else
			*str++ = 0x30;

		data >>= 1;
	}

	*str = 0;

	//reverse the order
	char *low, *high, temp;
	low = start, high = str - 1;

	while(low < high) {
		temp = *low;
		*low = *high;
		*high = temp;

		++low;
		--high;
	}

	return 0;
}

/*截取src字符串中,从下标为start开始到end-1(end前面)的字符串保存在dest中(下标从0开始)*/
void substring(char *dest,char *src,int start,int end)
{
	int i=start;
	if(start>strlen(src))return;
	if(end>strlen(src))
		end=strlen(src);
	while(i<end)
	{
		dest[i-start]=src[i];
		i++;
	}
	dest[i-start]='\0';
	return;
}