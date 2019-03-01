#include "stdint.h"
#include "stm32l0xx_hal.h"
//#include "m_gpio_config.h"
#include "m_lcd_hal.h"
#include "m_lcd.h"

LCD_HandleTypeDef LCDHandle;
uint16_t LCDHandleStat=0;
//const uint8_t  LCDDigitalIndexTable[]="0123456789abcdefghijklmnopqrstuvwxyz.-: GHCNOPSRAB";
const uint8_t  LCDDigitalIndexTable[]="0123456789abcdefghijklmnopqrstuvwxyz.-: GHCNOPSAR";
const uint8_t LCDDigitalTable[]=
{
    LCD_CODE_0,LCD_CODE_1,LCD_CODE_2,LCD_CODE_3,
    LCD_CODE_4,LCD_CODE_5,LCD_CODE_6,LCD_CODE_7,
    LCD_CODE_8,LCD_CODE_9,LCD_CODE_A,LCD_CODE_B,
    LCD_CODE_C_U,LCD_CODE_D,LCD_CODE_E,LCD_CODE_F,
    LCD_CODE_G_U,LCD_CODE_H_U,LCD_CODE_I,LCD_CODE_J,
    LCD_CODE_K,LCD_CODE_L,LCD_CODE_M,LCD_CODE_N_U,
    LCD_CODE_O_U,LCD_CODE_P,LCD_CODE_Q,LCD_CODE_R,
    LCD_CODE_S,LCD_CODE_T,LCD_CODE_U,LCD_CODE_V,
    LCD_CODE_W,LCD_CODE_X,LCD_CODE_Y,LCD_CODE_Z,
    LCD_CODE_DOT,LCD_CODE__,LCD_CODE_DDOT,0x00,
	LCD_CODE_G_U,LCD_CODE_H_U,LCD_CODE_C_U,LCD_CODE_N_U,
	LCD_CODE_O_U,LCD_CODE_P,LCD_CODE_S,LCD_CODE_A,LCD_CODE_R,
};

void m_lcd_disp_dig(uint8_t loc,uint8_t dig)
{
	uint16_t i;
	uint32_t __bit=(0x01ul<<(loc+1));
	for(i=0;i<8;i++){
		if(dig & 0x01){
			//HAL_LCD_Write(&LCDHandle, (uint32_t)i*2, ~(__bit), __bit);
			LCDHandle.Instance->RAM[i*2] |= __bit;
		}else{
			//HAL_LCD_Write(&LCDHandle, (uint32_t)i*2, (__bit), ~(__bit));
			//LCDHandle.Instance->RAM[i*2] &= ~(__bit);
		}
		dig>>=1;
	}
}

void m_lcd_disp_chr(uint8_t loc,uint8_t chr)
{
	uint8_t i;
	for(i=0;i<LCD_TABLE_LEN;i++){
		if(chr==LCDDigitalIndexTable[i])break;
	}
	if(i<LCD_TABLE_LEN){
		m_lcd_disp_dig(loc,LCDDigitalTable[i]);
	}
}

void m_lcd_disp_str(uint8_t* str)
{
    uint8_t loc=0;
    uint8_t index=0;
    while(*str!='\0'){
        for(index=0;index<LCD_TABLE_LEN;index++){
            if(*str==LCDDigitalIndexTable[index])break;
            
        }
        if(index<LCD_TABLE_LEN){
			m_lcd_disp_dig(loc,LCDDigitalTable[index]);
			loc++;
		}
        str++;
        
		if(loc>8)return;
    }
	//HAL_LCD_UpdateDisplayRequest(&LCDHandle);
}



void m_lcd_disp_str_refresh(uint8_t* str)
{
	//m_lcd_clear_all();
	m_lcd_clear();
    uint8_t loc=0;
    uint8_t index=0;
    while(*str!='\0'){
        for(index=0;index<LCD_TABLE_LEN;index++){
            if(*str==LCDDigitalIndexTable[index])break;
            
        }
        if(index<LCD_TABLE_LEN){
			m_lcd_disp_dig(loc,LCDDigitalTable[index]);
			loc++;
		}
        str++;
        
		if(loc>8)return;
    }
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
}
void m_lcd_disp_seg(uint8_t c,uint8_t s)
{
	//uint32_t r
	if(c>7 || s>15)return;
	LCDHandle.Instance->RAM[c<<1] |= (1u<<s);
}
void m_lcd_clear_seg(uint8_t c,uint8_t s)
{
	if(c>7 || s>15)return;
	LCDHandle.Instance->RAM[c<<1] &=  (~(1u<<s));
}


//
void m_lcd_refresh(void)
{
	HAL_LCD_UpdateDisplayRequest(&LCDHandle);
}

void m_lcd_ram_set(uint32_t t)
{
	uint8_t i;
	for(i=0;i<8;i++){
		LCDHandle.Instance->RAM[i<<1]=t;
	}
}

void m_lcd_ram_clear(void)
{
	m_lcd_ram_set(0x0ul);
}


void m_lcd_all_on(void)
{
	m_lcd_ram_set(0x0000fffful);
	m_lcd_refresh();
}
void m_lcd_all_off(void)
{
	m_lcd_ram_set(~0x0000fffful);
	m_lcd_refresh();	
}

void m_lcd_disp_dp(uint8_t loc)
{
	switch(loc){
		case 1:m_lcd_disp_seg_dp1();break;
		case 2:m_lcd_disp_seg_dp2();break;
		case 3:m_lcd_disp_seg_dp3();break;
		case 4:m_lcd_disp_seg_dp4();break;
		//case 5:m_lcd_disp_seg_dp5();break;
		//case 6:m_lcd_disp_seg_dp6();break;
		default:break;	
	}
}

void m_lcd_init(void)
{
	m_lcd_class_init();
	
}
void m_lcd_reinit(void)
{
	HAL_LCD_MspInit(&LCDHandle);
}
void m_lcd_clear(void)
{
	m_lcd_class_clear();
}


//apl

//file end
//@
