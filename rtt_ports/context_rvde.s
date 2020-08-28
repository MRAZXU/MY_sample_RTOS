;*************************************************************************
;                                 全局变量
;*************************************************************************
    IMPORT rt_thread_switch_interrupt_flag
    IMPORT rt_interrupt_from_thread
    IMPORT rt_interrupt_to_thread
		
;*************************************************************************
;                                 常量
;*************************************************************************
;-------------------------------------------------------------------------
;有关内核外设寄存器定义可参考官方文档：STM32F10xxx Cortex-M3 programming manual
;系统控制块外设SCB地址范围：0xE000ED00-0xE000ED3F
;-------------------------------------------------------------------------
SCB_VTOR        EQU     0xE000ED08     ; 向量表偏移寄存器
NVIC_INT_CTRL   EQU     0xE000ED04     ; 中断控制状态寄存器
NVIC_SYSPRI2    EQU     0xE000ED20     ; 系统优先级寄存器(2)
NVIC_PENDSV_PRI EQU     0x00FF0000     ; PendSV 优先级值 (lowest)
NVIC_PENDSVSET  EQU     0x10000000     ; 触发PendSV exception的值
	
;*************************************************************************
;                              代码产生指令
;*************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;cps指令用法
;CPSID I ;PRIMASK=1 ;关中断
;CPSIE I ;PRIMASK=0 ;开中断
;CPSID F ;FAULTMASK=1 ;关异常
;CPSIE F ;FAULTMASK=0 ;开异常

;/*
; * rt_base_t rt_hw_interrupt_disable();
; */
rt_hw_interrupt_disable    PROC
    EXPORT  rt_hw_interrupt_disable
    MRS     r0, PRIMASK;保存当前PRIMASK的值
    CPSID   I	;关中断
    BX      LR
    ENDP

;/*
; * void rt_hw_interrupt_enable(rt_base_t level);
; */
rt_hw_interrupt_enable    PROC
    EXPORT  rt_hw_interrupt_enable
    MSR     PRIMASK, r0;读取保存的PRIMASK的值
    BX      LR
    ENDP

;/*
; *-----------------------------------------------------------------------
; * 函数原型：void rt_hw_context_switch_to(rt_uint32 to);
; * r0 --> to
; * 该函数用于开启第一次线程切换
; *-----------------------------------------------------------------------
; */
		
rt_hw_context_switch_to    PROC
    
	; 导出rt_hw_context_switch_to，让其具有全局属性，可以在C文件调用
	EXPORT rt_hw_context_switch_to
		
    ; 设置rt_interrupt_to_thread的值
    LDR     r1, =rt_interrupt_to_thread             ;将rt_interrupt_to_thread的地址加载到r1
    STR     r0, [r1]                                ;将r0的值存储到rt_interrupt_to_thread

    ; 设置rt_interrupt_from_thread的值为0，表示启动第一次线程切换
    LDR     r1, =rt_interrupt_from_thread           ;将rt_interrupt_from_thread的地址加载到r1
    MOV     r0, #0x0                                ;配置r0等于0
    STR     r0, [r1]                                ;将r0的值存储到rt_interrupt_from_thread

    ; 设置中断标志位rt_thread_switch_interrupt_flag的值为1
    LDR     r1, =rt_thread_switch_interrupt_flag    ;将rt_thread_switch_interrupt_flag的地址加载到r1
    MOV     r0, #1                                  ;配置r0等于1
    STR     r0, [r1]                                ;将r0的值存储到rt_thread_switch_interrupt_flag

    ; 设置 PendSV 异常的优先级
    LDR     r0, =NVIC_SYSPRI2
    LDR     r1, =NVIC_PENDSV_PRI
    LDR.W   r2, [r0,#0x00]       ; 读
    ORR     r1,r1,r2             ; 改
    STR     r1, [r0]             ; 写

    ; 触发 PendSV 异常 (产生上下文切换)
    LDR     r0, =NVIC_INT_CTRL
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]

    ; 开中断
    CPSIE   F
    CPSIE   I

    ; 永远不会到达这里
    ENDP
;/*
; *-----------------------------------------------------------------------
; * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
; * r0 --> from
; * r1 --> to
; *-----------------------------------------------------------------------
; */
;rt_hw_context_switch_interrupt
    ;EXPORT rt_hw_context_switch_interrupt
		
rt_hw_context_switch    PROC
    EXPORT rt_hw_context_switch

    ; 设置中断标志位rt_thread_switch_interrupt_flag为1     
    LDR     r2, =rt_thread_switch_interrupt_flag          ; 加载rt_thread_switch_interrupt_flag的地址到r2
    LDR     r3, [r2]                                      ; 加载rt_thread_switch_interrupt_flag的值到r3
    CMP     r3, #1                                        ; r3与1比较，相等则执行BEQ指令，否则不执行
    BEQ     _reswitch
    MOV     r3, #1                                        ; 设置r3的值为1
    STR     r3, [r2]                                      ; 将r3的值存储到rt_thread_switch_interrupt_flag，即置1
    
	; 设置rt_interrupt_from_thread的值
    LDR     r2, =rt_interrupt_from_thread                 ; 加载rt_interrupt_from_thread的地址到r2
    STR     r0, [r2]                                      ; 存储r0的值到rt_interrupt_from_thread，即上一个线程栈指针sp的指针

_reswitch
    ; 设置rt_interrupt_to_thread的值
	LDR     r2, =rt_interrupt_to_thread                   ; 加载rt_interrupt_from_thread的地址到r2
    STR     r1, [r2]                                      ; 存储r1的值到rt_interrupt_from_thread，即下一个线程栈指针sp的指针

    ; 触发PendSV异常，实现上下文切换
	LDR     r0, =NVIC_INT_CTRL              
    LDR     r1, =NVIC_PENDSVSET
    STR     r1, [r0]
	
    ; 子程序返回
	BX      LR
	
	; 子程序结束
    ENDP

;/*
; *-----------------------------------------------------------------------
; * void PendSV_Handler(void);
; * r0 --> switch from thread stack
; * r1 --> switch to thread stack
; * psr, pc, lr, r12, r3, r2, r1, r0 are pushed into [from] stack
; *-----------------------------------------------------------------------
; */
;/*
;栈汇编指令说明
;ST - store
;M - Multiple
;F - FULL 栈指针指向栈顶元素（即最后一个入栈的数据元素）
;栈指针指向与栈顶元素相邻的一个可用书局单元时称为EMPTY栈
;D - Descending   

;LD - Load
;M - Multiple
;F - FULL
;D - Descending 当数据栈向内存地址减小的方向增长时，称为Descending栈
;当数据栈向内存地址增加的方向增长时，称为 Ascending栈

;用于数据存储与读取：
;IA - Increase After
;IB - Increase Before
;DA - Decrease After
;DB - Decrease Before 

;用于堆栈操作:
;FD - Full Descending
;ED - Empty Descending
;FA - Full Ascending
;EA - Empty Ascending

;压栈（存入）
;STMIB（地址先增而后完成操作）、STMFA（满递增堆栈）；
;STMIA（完成操作而后地址递增）、STMEA（空递增堆栈）；
;STMDB（地址先减而后完成操作）、STMFD（满递减堆栈）；
;STMDA（完成操作而后地址递减）、STMED（空递减堆栈）；
;出栈（读取）
;LDMIB（地址先增而后完成操作）、LDMFA（满递增堆栈）；
;LDMIA（完成操作而后地址递增）、LDMEA（空递增堆栈）；
;LDMDB（地址先减而后完成操作）、LDMFD（满递减堆栈）；
;LDMDA（完成操作而后地址递减）、LDMED（空递减堆栈）；


;*/
PendSV_Handler   PROC
    EXPORT PendSV_Handler

    ; 失能中断，为了保护上下文切换不被中断
    MRS     r2, PRIMASK
    CPSID   I

    ; 获取中断标志位，看看是否为0
    LDR     r0, =rt_thread_switch_interrupt_flag     ; 加载rt_thread_switch_interrupt_flag的地址到r0
    LDR     r1, [r0]                                 ; 加载rt_thread_switch_interrupt_flag的值到r1
    CBZ     r1, pendsv_exit                          ; 判断r1是否为0，为0则跳转到pendsv_exit

    ; r1不为0则清0
    MOV     r1, #0x00
    STR     r1, [r0]                                 ; 将r1的值存储到rt_thread_switch_interrupt_flag，即清0

    ; 判断rt_interrupt_from_thread的值是否为0
    LDR     r0, =rt_interrupt_from_thread            ; 加载rt_interrupt_from_thread的地址到r0
    LDR     r1, [r0]                                 ; 加载rt_interrupt_from_thread的值到r1
    CBZ     r1, switch_to_thread                     ; 判断r1是否为0，为0则跳转到switch_to_thread
                                                     ; 第一次线程切换时rt_interrupt_from_thread肯定为0，则跳转到switch_to_thread

; ========================== 上文保存 ==============================
    ; 当进入PendSVC Handler时，上一个线程运行的环境即：
 	; xPSR，PC（线程入口地址），R14，R12，R3，R2，R1，R0（线程的形参）
 	; 这些CPU寄存器的值会自动保存到线程的栈中，剩下的r4~r11需要手动保存
	
 	
    MRS     r1, psp                                  ; 获取线程栈指针到r1
    STMFD   r1!, {r4 - r11}                          ; 将CPU寄存器r4~r11的值存储到r1指向的地址(每操作一次地址将递减一次)
    ;r1! 写入最后返回r1(回写)  
	;STMFD满递减堆栈
	;STMDB地址先减而后完成操作
	LDR     r0, [r0]                                 ; 加载r0指向值到r0，即r0=rt_interrupt_from_thread
    STR     r1, [r0]                                 ; 将r1的值存储到r0，即更新线程栈sp
	
; ========================== 下文切换 ==============================
switch_to_thread
    LDR     r1, =rt_interrupt_to_thread               ; 加载rt_interrupt_to_thread的地址到r1
	                                                  ; rt_interrupt_to_thread是一个全局变量，里面存的是线程栈指针SP的指针
    LDR     r1, [r1]                                  ; 加载rt_interrupt_to_thread的值到r1，即sp指针的指针
    LDR     r1, [r1]                                  ; 加载rt_interrupt_to_thread的值到r1，即sp

    LDMFD   r1!, {r4 - r11}                           ;将线程栈指针r1(操作之前先递减)指向的内容加载到CPU寄存器r4~r11
    ;LDMFD 满递减堆栈
	;LDMIA 完成操作而后地址递增
	MSR     psp, r1                                   ;将线程栈指针更新到PSP

pendsv_exit
    ; 恢复中断
    MSR     PRIMASK, r2

    ORR     lr, lr, #0x04                             ; 确保异常返回使用的堆栈指针是PSP，即LR寄存器的位2要为1
    BX      lr                                        ; 异常返回，这个时候任务堆栈中的剩下内容将会自动加载到xPSR，PC（任务入口地址），R14，R12，R3，R2，R1，R0（任务的形参）
	                                                  ; 同时PSP的值也将更新，即指向任务堆栈的栈顶。在ARMC3中，堆是由高地址向低地址生长的。
    ; PendSV_Handler 子程序结束
	ENDP	
	
	
	ALIGN   4

    END
		