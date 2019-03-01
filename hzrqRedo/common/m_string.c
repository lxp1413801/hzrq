#include "stdint.h"
#include "m_string.h"
const uint8_t HexTable[]="0123456789ABCDEF";

const uint8_t Bcd2HexTable[]=
{
	 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,	//00
	10,11,12,13,14,15,16,17,18,19,00,00,00,00,00,00,	//10	
	20,21,22,23,24,25,26,27,28,29,00,00,00,00,00,00,	//20	
	30,31,32,33,34,35,36,37,38,39,00,00,00,00,00,00,	//30	
	40,41,42,43,44,45,46,47,48,49,00,00,00,00,00,00,	//40	
	50,51,52,53,54,55,56,57,58,59,00,00,00,00,00,00,	//50	
	60,61,62,63,64,65,66,67,68,69,00,00,00,00,00,00,	//60	
	70,71,72,73,74,75,76,77,78,79,00,00,00,00,00,00,	//70	
	80,81,82,83,84,85,86,87,88,89,00,00,00,00,00,00,	//80	
	90,91,92,93,94,95,96,97,98,99,00,00,00,00,00,00,	//90	
};
const uint8_t Hex2BcdTable[]=
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,  //0
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,  //1
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,  //2
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,  //3
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,  //4
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,  //5
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,  //6
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,  //7    
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,  //8
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,  //9 
};
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
uint16_t m_str_cpy(uint8_t* d,uint8_t* s)
{
	uint16_t len=0;
	while(*s!='\0'){
		*d++=*s++;
		len++;
	}
	*d='\0';	
	return (len+1);
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
		if((*d != *s) )
		{
			ret=0;
			break;
		}
		s++;d++;
	}
	return ret;
}
uint16_t m_str_cmp_ex(uint8_t* d,uint8_t* s)
{
	uint16_t ret=1;
	while(*d!='\0' && *s!='\0')
	{
		if(*s == '?' || *d== '?'){
		
		}
		else  if((*d != *s))
		{
			ret=0;
			break;
		}
		s++;d++;
	}
	return ret;
}
uint16_t m_str_cmp_len(uint8_t* d,uint8_t* s,uint16_t len)
{
	uint16_t ret=1;
	while(len--)
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
	uint16_t loc=0;
	while(*b!='\0' )
	{
		cc=c;
		bb=b++;
		ret=0;
		while(*bb!='\0' && *cc!='\0')
		{
			if(*bb!=*cc && *cc!='?')
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
		loc++;
	}
	return ret;
}
uint16_t m_str_match_ex(uint8_t* b,uint8_t* c,uint16_t *loc)
{
	uint8_t *bb,*cc;
	uint16_t local=0;
	while(*b!='\0' ){
		cc=c;
		bb=b++;
		while(*bb!='\0' && *cc!='\0'){
			if(*bb!=*cc && *cc!='\?'){
				break;
			}
			bb++;
			cc++;
		}
		if(*cc=='\0'){
			*loc=local;
			return 1;
			
		}
		local++;
	}
	*loc=local;
	return 0;
}
/*
return x^y
*/
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
/*
e.g.: 1234-->"1234"
*/
void m_int_2_str(uint8_t* buf,int32_t x,uint16_t len)
{
	int32_t t;
	if(!len)return;
	
	t=m_math_pow(10,len);
	if(x>=t-1)x=t-1;
	while(len--)
	{
		t=x%10;
		*buf--=HexTable[t];
		x/=10;
	}
}

uint16_t m_int_2_str_ex(uint8_t* buf,int32_t x,uint16_t ssize)
{
	int32_t t;
	int16_t i;
	t=m_math_pow(10,ssize-1);
	if(x>=t-1)x=t-1;
	buf[ssize-1]='\0';
	;
	for(i=ssize-2;i>=0;i--)
	{
		t=x%10L;
		buf[i]=(uint8_t)t+'0';

		x/=10L;
		if(x==0)break;
	}
	//return buf+i;
	return m_str_cpy(buf,buf+i);
}
/*
e.g.: "001234"-->"  1234"
*/
uint16_t m_str_head_hide(uint8_t* str,uint16_t loc)
{
	uint16_t ret=0;
	while(loc--)
	{
		if(*str=='0')
		{
			*str++=' ';
			ret++;
		}
		else
		{
			break;
		}
	}
	return ret;
}
/*
e.g.: 0x3f-->string "3F"
*/
uint16_t HEX8(uint8_t* hex,uint8_t x)
{
	*hex++=HexTable[(x&0xf0)>>4];
	*hex++=HexTable[(x&0x0f)];
	hex='\0';
    return 3;
}

/*
e.g.: 0x01,0x02,0x03,0x04-->"01020304"
*/
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
/*
#define _chr2hex(h,c) do{ \
    if(c>='0' && c <='9')h|=(c-'0'); \
    else if(c>='A' && c<='F')h|=(c-'A'+0x0a); \
    else if(c>='a' && c<='f')h|=(c-'a'+0x0a); \
    else h|=0; \
}while(0);
*/
/*
e.g.:"01020304" --> 0x01,0x02,0x03,0x04
*/
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

uint32_t swap_uint32(uint32_t x)
{
    uint32_t ret;
    uint8_t *d,*s;
    s=(uint8_t*)&x;
    d=(uint8_t*)&ret;
    d[0]=s[3];
    d[1]=s[2];
    d[2]=s[1];
    d[3]=s[0];
    return ret;
}

uint16_t swap_uint16(uint16_t x)
{
    uint16_t ret;
    uint8_t *d,*s;
    s=(uint8_t*)&x;
    d=(uint8_t*)&ret;
    d[0]=s[1];
    d[1]=s[0];
    return ret;    
}

void int32_array_shift_right(int32_t *p32,uint16_t eoc)
{
	while(eoc>=1){
		p32[eoc]=p32[eoc-1];
		eoc--;
	}
}
//add by lxp at 2017.04.20
void m_str_format_lowercase(uint8_t* str,uint16_t len){
	while(len--){
		if(*str>='A' && *str<='Z'){
			*str=*str+('a'-'A');
			str++;
		}
	}
}

uint8_t int32_2_d(uint8_t* str,uint32_t t32)
{
	uint8_t buf[16]={0};
	uint8_t* p=buf+14;
	uint8_t t8=0;
	while(t32){
		*p--=(uint8_t)(t32%10)+'0';
		t32/=10;
		t8++;
	}
	p++;
	m_mem_cpy(str,p);
	return t8;
}

uint16_t my_scanf_int16(uint8_t* str,uint8_t eof,uint16_t* loc)
{
	uint16_t t16,i=0;
	uint16_t ret=0x00;
	uint8_t chr;
	do{
		chr=*str;
		if(chr==eof)break;
		ret*=10;
		
		t16=0;
		_chr2hex(t16,chr);
		ret += t16;
		str++;
		i++;
		if(i>=4)break;
		//;
	}while(1);
	if(loc){
		*loc=i;
	}
	return ret;
}
//file end
