#ifndef __M_CLOCK_CONFIG_H__
#define __M_CLOCK_CONFIG_H__
	//#include "stm32l0xx_hal.h"

	#ifdef __cplusplus
		extern "C" {
	#endif
	#define __myReg_RCC_CR 			(*(( volatile uint32_t*)(0x40021000+0x0000)))
	#define __myReg_RCC_ICSCR 		(*(( volatile uint32_t*)(0x40021000+0x0004)))
	#define __myReg_RCC_CFGR 		(*(( volatile uint32_t*)(0x40021000+0x000c)))
	#define RCC_CR_DEFAULT_VALUE 	(0x00000300ul)
	#define RCC_ICSCR_DEFAULT_VALUE (0x0000b000ul)			
			
			
	#ifndef portNVIC_SYSTICK_CTRL
	#define portNVIC_SYSTICK_CTRL		( *( ( volatile uint32_t *) 0xe000e010 ))
	#endif
	//
	#ifndef portNVIC_SYSTICK_LOAD
	#define portNVIC_SYSTICK_LOAD		( *( ( volatile uint32_t *) 0xe000e014 ))
	#endif
	//
	#ifndef portNVIC_SYSTICK_CURRENT_VALUE
	#define portNVIC_SYSTICK_CURRENT_VALUE	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
	#endif
	//
	#ifndef portNVIC_INT_CTRL
	#define portNVIC_INT_CTRL			( ( volatile uint32_t *) 0xe000ed04 )
	#endif
	//
	#ifndef portNVIC_SYSPRI2
	#define portNVIC_SYSPRI2			( ( volatile uint32_t *) 0xe000ed20 )
	#endif
	//
	#ifndef portNVIC_SYSTICK_CLK
	#define portNVIC_SYSTICK_CLK		0x00000004
	#endif
	//
	#ifndef portNVIC_SYSTICK_INT
	#define portNVIC_SYSTICK_INT		0x00000002
	#endif
	//
	#ifndef portNVIC_SYSTICK_ENABLE
	#define portNVIC_SYSTICK_ENABLE		0x00000001
	#endif
	//
	#ifndef portNVIC_SYSTICK_COUNT_FLAG
	#define portNVIC_SYSTICK_COUNT_FLAG		( 1UL << 16UL )
	#endif
	//
	#ifndef portNVIC_PENDSVSET
	#define portNVIC_PENDSVSET			0x10000000
	#endif
	//
	#ifndef portMIN_INTERRUPT_PRIORITY
	#define portMIN_INTERRUPT_PRIORITY	( 255UL )
	#endif
	//
	#ifndef portNVIC_PENDSV_PRI
	#define portNVIC_PENDSV_PRI			( portMIN_INTERRUPT_PRIORITY << 16UL )
	#endif
	//
	#ifndef portNVIC_SYSTICK_PRI
	#define portNVIC_SYSTICK_PRI		( portMIN_INTERRUPT_PRIORITY << 24UL )			
	#endif
	//
	#ifndef portSY_FULL_READ_WRITE
	#define portSY_FULL_READ_WRITE		( 15 )
	#endif
	extern void m_clock_config_enable_msi(void);
	
	extern void m_clock_config_enable_lse(void);
	
	extern void m_clock_set_rtc_clock(void);
	
	//extern void m_clock_config_system(void);
	extern void m_clock_set_system_clock_hsi(void);
	extern void m_clock_set_system_clock_msi(void);
	extern void m_clock_select_msi(void);	
	extern void m_clock_select_hsi(void);
	//
	//extern void m_clock_set_system_clock_msi_low(void);
	#ifdef __cplusplus
		}
	#endif			
#endif
		/*
		user\interface\m_clock_config.h(4): error:  #20: identifier "class" is undefined
		*/
//file end
//@
