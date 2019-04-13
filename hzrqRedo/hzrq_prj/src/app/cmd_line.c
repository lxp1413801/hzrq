#include "./includes/includes.h"

void __str_lower(uint8_t* str,uint16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++){
		if(str[i]>='A' && str[i]<='Z'){
			str[i]-=('A'-'a');
		}
	}
}

uint16_t _cmd_len_set_id(uint8_t* buf,uint16_t len)
{
	uint8_t idbuf[16];
	uint8_t *p;
	uint16_t loc,t16,ret=0;
	//if(m_str_match_ex(buf))
	do{
		ret=m_str_match_ex(buf,(uint8_t*)"setid",&loc);
		if(!ret)break;
		p=buf+loc+1;
		//t16=loc+1
		loc++;
		while(*p<'0' || *p>'9'){
			if(loc>=len)break;
			p++;
			loc++;
		}
		if(loc>=len){ret=2;break;}
		t16=0;
		while(loc<len){
			
			if(*p>='0' && *p<='9'){
				idbuf[t16]=*p;
				t16++;
			}
			p++;
			loc++;
			if(t16>=16)break;
		}
		if(t16<16){ret=2;break;};
		//
		ret=0;
		for(t16=1;t16<16;t16++){
			ret+=(idbuf[t16]-'0');
		}
		idbuf[0]='0'+(uint8_t)(ret%10);
		//
		api_sysdata_save();
		m_mem_cpy_len(sysData.hzrqMeterId,idbuf,16);
		api_sysdata_save();
		ret=1;
	}while(0);
	if(ret==1){
		m_usart1_se_send_str((uint8_t*)"setid ok!id=");
		m_usart1_se_send_len(idbuf,16);
		m_usart1_se_send_str((uint8_t*)"\r\n\r\n");
	}else if(ret==2){
		m_usart1_se_send_str((uint8_t*)"setid error\r\n\r\n");
	}
	return ret;
}
uint16_t _cmd_len_get_id(uint8_t* buf,uint16_t len)
{

	uint16_t loc,t16,ret=0;
	do{
		ret=m_str_match_ex(buf,(uint8_t*)"getid",&loc);
		if(!ret)break;
		
	}while(0);
	if(ret==1){
		m_usart1_se_send_str((uint8_t*)"getid ok!id=");
		m_usart1_se_send_len(sysData.hzrqMeterId,16);
		m_usart1_se_send_str((uint8_t*)"\r\n\r\n");		
	}
	return ret;
}
void cmd_line_process(void)
{
	uint8_t buf[128];
	uint16_t len;
	//if(__uartSeRceivedBufCount)
	len=__uartSeRceivedBufCount>sizeof(buf)?sizeof(buf):__uartSeRceivedBufCount;
	
	m_mem_cpy_len(buf,__uartSeReceivedBuf,len);
	__uartSeRceivedBufCount=0;
	__str_lower(buf,len);
	uint16_t ret=0;
	do{
		ret=_cmd_len_set_id(buf,len);if(ret)break;
		ret=_cmd_len_get_id(buf,len);if(ret)break;
	}while(0);
	if(ret==0){
		m_usart1_se_send_str((uint8_t*)"cmd error!\r\n\r\n");	
	}
}


//file end
