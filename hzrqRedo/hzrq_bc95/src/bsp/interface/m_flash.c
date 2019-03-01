#include "../../includes/includes.h"
//#include "m_interface.h"
//#include "../protocol/m_string.h"
#define m_flash_erase_enable()  do{ \
	SET_BIT(FLASH->PECR, FLASH_PECR_ERASE); \
	SET_BIT(FLASH->PECR, FLASH_PECR_PROG); \
}while(0);

#define m_flash_erase_disable()  do{ \
	CLEAR_BIT(FLASH->PECR, FLASH_PECR_ERASE); \
	CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG); \
}while(0);
void m_flash_erase_eeprom(uint32_t addr,uint32_t len)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	//uint32_t index = 0;

	/* Process Locked */
	//__HAL_LOCK(&ProcFlash);
	
	/* Wait for last operation to be completed */
	if(!(IS_FLASH_DATA_ADDRESS(addr)))return;
	//__disable_irq();
	HAL_FLASH_Unlock();
	
	do{
		status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
	}while(status!=HAL_OK);
	
	if(status==HAL_OK){
		m_flash_erase_enable();
		while(len>0){
			*(__IO uint32_t *)(addr & ~0x03) =(uint32_t)0;
			addr+=4;			
			
			if(len>=4){
				len-=4;
			}else{
				len=0;
			}

			status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
			
		}
		m_flash_erase_disable();
	}
	HAL_FLASH_Lock();
	//__isr_enable();
	//__enable_irq();
}
void m_flash_write_eeprom(uint32_t addr,uint8_t* buf,uint32_t len)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	/* Process Locked */
	
	/* Wait for last operation to be completed */
	if(!(IS_FLASH_DATA_ADDRESS(addr)))return;
	//__disable_irq();
	HAL_FLASH_Unlock();
	
	do{
		status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
	}while(status!=HAL_OK);
	if(status==HAL_OK){
		//m_flash_erase_enable();
		while(len>0){
			if(len>=4){
				*(__IO uint32_t *)(addr) =*(uint32_t*)buf;
				addr+=4;buf+=4;
				len-=4;
			}else if(len>=2){
				*(__IO uint16_t *)(addr) =*(uint16_t*)buf;
				addr+=2;buf+=2;
				len-=2;
			}else{
				*(__IO uint8_t *)(addr) =*(uint8_t*)buf;
				addr+=1;buf+=1;
				len-=1;			
			}
			status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
		}
		//m_flash_erase_disable();
	}
	HAL_FLASH_Lock();
	//__isr_enable();
	//__enable_irq();
	
}
/*
int16_t m_flash_modify_eeprom(uint32_t offset,uint8_t* buf,uint32_t len)
{
	uint32_t* p; 
	uint32_t t1;
	uint32_t l;

	if((offset & 0x03ul)!=0)
	{

		p=(uint32_t*)(offset & ~0x03);
		t1=*p;
		l=4-(offset & 0x03);
		if(l>len)l=len;
		p=&t1;
		(uint8_t*)p+=(offset & 0x03ul);
		m_mem_cpy_len((uint8_t*)p,buf,l);

		m_flash_erase_eeprom((offset & ~0x03),4);
		m_flash_write_eeprom((offset & ~0x03),(uint8_t*)&t1,4);
		buf+=l;
		len-=l;	
		offset=(offset & ~0x03)+4;
	}
	while(len>=4)
	{
		m_flash_erase_eeprom(offset,4);
		m_mem_cpy_len((uint8_t*)&t1,buf,4);
		m_flash_write_eeprom(offset,(uint8_t*)&t1,4);
		offset+=4;
		buf+=4;
		len-=4;
	}
	if(len>0)
	{
		p=(uint32_t*)(offset);
		t1=*p;
		m_flash_erase_eeprom(offset,4);
		m_mem_cpy_len((uint8_t*)&t1,buf,len);
		m_flash_write_eeprom(offset,(uint8_t*)&t1,4);
	}
	return 0;
}
*/
//@file end
//@
