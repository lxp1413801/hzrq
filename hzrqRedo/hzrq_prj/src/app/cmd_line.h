//file name :cmd_line.h
#ifndef __cmd_line_h__
#define __cmd_line_h__
#ifdef __cplusplus
	extern "C"{
#endif

//	#define flgCMD_LINE_USRT_ERR	(0x01UL<<0)
//	#define flgCMD_LINE_USRT_LF		(0x01UL<<1)
//	
//	#define flgCMD_LINE_ALL_BITS	(flgCMD_LINE_USRT_ERR | flgCMD_LINE_USRT_LF)
		
	extern void cmd_line_process(void);

#ifdef __cplusplus
	}
#endif
#endif
//file end
