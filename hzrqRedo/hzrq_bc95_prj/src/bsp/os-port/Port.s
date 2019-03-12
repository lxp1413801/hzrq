;--------------------------------------------------
;modify by lxp
;--------------------------------------------------
	PRESERVE8
	THUMB
	AREA    |.text|, CODE, READONLY

	
	import pxCurrentTCB
prvPortStartFirstTask proc
	export prvPortStartFirstTask
	ldr r3, =pxCurrentTCB  
	ldr r1, [r3]
	ldr r0, [r1]  
	adds r0, #32  
	msr psp, r0  
	movs r0, #2  
	msr CONTROL, r0
	pop {r0-r5}  
	mov lr, r5  
	cpsie i  
	pop {pc}  

	ALIGN
	ENDP	

ulSetInterruptMaskFromISR proc
	export ulSetInterruptMaskFromISR
	mrs r0, PRIMASK
	cpsid i
	bx lr
	ENDP

vClearInterruptMaskFromISR PROC
	export vClearInterruptMaskFromISR
	msr PRIMASK, r0
	bx lr
	ENDP
		

xPortPendSVHandler PROC
	
	extern vTaskSwitchContext
	extern pxCurrentTCB
	export xPortPendSVHandler
	mrs r0, psp
	ldr r3, =pxCurrentTCB  
	ldr r2, [r3]
	subs r0, #32  
	str r0, [r2]  
	stmia r0!, {r4-r7}  
	mov r4, r8  
	mov r5, r9
	mov r6, r10
	mov r7, r11
	stmia r0!, {r4-r7}

	push {r3, r14}
	cpsid i
	bl vTaskSwitchContext
	cpsie i
	pop {r2, r3}  

	ldr r1, [r2]
	ldr r0, [r1]  
	adds r0, #16  
	ldmia r0!, {r4-r7}  
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r11, r7

	msr psp, r0  

	subs r0, #32  
	ldmia r0!, {r4-r7}  

	bx r3
	ALIGN
	ENDP			
	END
