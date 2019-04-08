#ifndef __M_FLASH_H__
#define __M_FLASH_H__


	#ifdef __cplusplus
		extern "C"
		{
	#endif
	extern void m_flash_erase_eeprom(uint32_t addr,uint32_t len);
	extern void m_flash_write_eeprom(uint32_t addr,uint8_t* buf,uint32_t len);
	extern void m_flash_read_eeprom(uint32_t addr,uint8_t* buf,uint32_t len);
	int16_t m_flash_modify_eeprom(uint32_t offset,uint8_t* buf,uint32_t len);
	#ifdef __cplusplus
		}
	#endif

#endif
