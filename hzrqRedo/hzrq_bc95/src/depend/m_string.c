#include <stdint.h>
#include <stdlib.h>
#include "m_protocol.h"

void m_mem_set(uint8_t* buf,uint8_t x,uint16_t len)
{
	while(len--)
	{
		*buf++=x;
	}
}
void m_mem_cpy(uint8_t* d,uint8_t* s)
{
	while(*s!='\0')
	{
		*d++=*s++;
	}
	*d='\0';
}
void m_mem_cpy_len(uint8_t* d,uint8_t* s,uint16_t len)
{
	
	while(len)
	{
		*d++=*s++;
		len--;
	}
}
uint16_t m_str_cmp(uint8_t* d,uint8_t* s)
{
	uint16_t ret=1;
	while(*d!='\0' && *s!='\0')
	{
		if(*d != *s)
		{
			ret=0;
			break;
		}
		s++;d++;
	}
	return ret;
}
uint16_t m_str_match(uint8_t* b,uint8_t* c)
{
	uint8_t *bb,*cc;
	uint16_t ret=0;
	while(*b!='\0' )
	{
		cc=c;
		bb=b++;
		ret=0;
		while(*bb!='\0' && *cc!='\0')
		{
			if(*bb!=*cc)
			{
				ret=0;
				break;
			}
			bb++;
			cc++;
		}
		if(*cc=='\0')
		{
			//if(ret)return ret;
			//ret=1;
			return 1;
		}
	}
	return ret;
}
int32_t m_math_pow(int32_t x,int32_t y)
{
	int32_t ret=1;
	if(y==0)return 1;
	while(y--)
	{
		ret*=x;
	}
	return ret;
}

const uint8_t HexTable[]="0123456789ABCDEF";
uint16_t m_str_b2h(uint8_t* h,uint8_t* b,uint16_t len)
 {
	 uint16_t i=0;
	 uint8_t t8;
	 for(i=0;i<len;i++)
	 {
		 t8=*b++;
		 *h++=HexTable[(t8&0xf0)>>4];
		 *h++=HexTable[(t8&0x0f)];
	 }
	 return (len<<1);
 }
 #define _chr2hex(h,c) do{ \
    if(c>='0' && c <='9')h|=(c-'0'); \
    else if(c>='A' && c<='F')h|=(c-'A'+0x0a); \
    else if(c>='a' && c<='f')h|=(c-'a'+0x0a); \
    else h|=0; \
}while(0);
 uint16_t m_str_h2b(uint8_t* b,uint8_t* h,uint16_t len)
 {
	uint16_t i=len;
	uint8_t bb,s;
	while(i>=2)
	{
        bb=0;
        s=*h++;
        _chr2hex(bb,s);
        bb<<=4;
        s=*h++;
        _chr2hex(bb,s);
        *b++=bb;
		i-=2;
	}
	return (len>>1);
 }
//file end
