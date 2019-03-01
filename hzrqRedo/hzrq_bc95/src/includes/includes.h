#ifndef __prj_include_h__
#define __prj_include_h__
#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef __xT
	#define __xT(conStr) (uint8_t*)conStr
#endif

#include "common.h"
#include "./inc/stm32l0xx_hal.h"
#include "./configs/stm32l0xx_hal_conf.h"
#include "./configs/FreeRTOSConfig.h"
#include "./configs/user_configs.h"
#include "cmsis_os.h"
#include "./bsp/system/stm32l0xx_it.h"
#include "./bsp/interface/m_interface.h"
//#include "./depend/m_depend.h"
#include "qc-protocol.h"

#include "./api/m_api.h"
#include "./app/app.h"

#ifdef __cplusplus
}
#endif
#endif
//file end
