#include <rtthread.h>
#include <rthw.h>
#include "ARMCM3.h"
/*全局变量*/
rt_uint32_t flag1;
rt_uint32_t flag2;
rt_uint32_t flag3;
rt_uint32_t flag4;
//用于验证临界区保护的全局变量
rt_uint32_t tickcounter=0;
//验证调度锁的变量
rt_uint32_t sharecounter=0;
//线程信息
struct _thread_info thread_info3;
//优先级列表
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
/**********************线程控制块 & STACK & 线程声明*****************************/
//定义线程控制块
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;
struct rt_thread rt_flag3_thread;
struct rt_thread rt_flag4_thread;
ALIGN(RT_ALIGN_SIZE)//四字节对齐
//定义线程栈 
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];
rt_uint8_t rt_flag4_thread_stack[512];
//线程声明
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);
void flag3_thread_entry(void *p_arg);
void flag4_thread_entry(void *p_arg);
/**********************线程控制块 & STACK & 线程声明*****************************/

void delay (uint32_t count);
int main(void)
{
	/* 关中断 */
    rt_hw_interrupt_disable();
    
    /* SysTick中断频率设置 */ 
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );//10ms的tick
	
	/* 系统定时器列表初始化 */
	rt_system_timer_init();
	
	/*调度器初始化*/
	rt_system_scheduler_init();
	
	/* 初始化空闲线程 */    
    rt_thread_idle_init();
	
	/*初始化线程1*/
	rt_thread_init( &rt_flag1_thread,                 /* 线程控制块 */
                    "rt_flag1_thread",                /* 线程名字，字符串形式 */
	                flag1_thread_entry,               /* 线程入口地址 */
	                RT_NULL,                          /* 线程形参 */
	                &rt_flag1_thread_stack[0],        /* 线程栈起始地址 */
	                sizeof(rt_flag1_thread_stack),    /* 线程栈大小，单位为字节 */
					3,								  /* 优先级*/
					10);  							  /* 时间片*/
	/* 将线程插入到就绪列表 */
	//rt_list_insert_before( &(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist) );	
	rt_thread_startup(&rt_flag1_thread);
	/*初始化线程2*/
	rt_thread_init(&rt_flag2_thread,
					"rt_flag2_thread",                /* 线程名字，字符串形式 */
					flag2_thread_entry,
					RT_NULL,
					&rt_flag2_thread_stack[0],
					sizeof(rt_flag2_thread_stack),
						3,
					10);
	/* 将线程插入到就绪列表 */
	//rt_list_insert_before( &(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist) );	
	rt_thread_startup(&rt_flag2_thread);				
	/* 初始化线程3 */
	rt_thread_init( &rt_flag3_thread,                 /* 线程控制块 */
                    "rt_flag3_thread",                /* 线程名字，字符串形式 */
	                flag3_thread_entry,               /* 线程入口地址 */
	                RT_NULL,                          /* 线程形参 */
	                &rt_flag3_thread_stack[0],        /* 线程栈起始地址 */
	                sizeof(rt_flag3_thread_stack),    /* 线程栈大小，单位为字节 */
                    3,								  /*线程3和2同一个优先级*/
					5);                               /*时间片*/			
	rt_thread_startup(&rt_flag3_thread);
		/* 初始化线程4 注定被删除 */
	rt_thread_init( &rt_flag4_thread,                 /* 线程控制块 */
                    "rt_flag4_thread",                /* 线程名字，字符串形式 */
	                flag4_thread_entry,               /* 线程入口地址 */
	                RT_NULL,                          /* 线程形参 */
	                &rt_flag4_thread_stack[0],        /* 线程栈起始地址 */
	                sizeof(rt_flag4_thread_stack),    /* 线程栈大小，单位为字节 */
                    3,								  /*线程3和2同一个优先级*/
					2);                               /*时间片*/			
	rt_thread_startup(&rt_flag4_thread);
	/* 启动系统调度器 */
	rt_system_scheduler_start(); 	
}
/***************************函数实现*****************************/
/* 软件延时 */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}
/*线程入口1*/
void flag1_thread_entry( void *p_arg )
{
	for(;;) 
	 {
		
		rt_uint32_t var;
		flag1=1;
		rt_thread_resume(&rt_flag3_thread);
		//加锁
		rt_enter_critical();
		var=sharecounter;
		var++;
		rt_thread_delay(100);//阻塞延时  
		sharecounter=var;
		//解锁	 
		rt_exit_critical();
		rt_thread_resume(&rt_flag3_thread);
		//thread_info3=rt_thread_getinfo(&rt_flag3_thread);
		flag1=0;
		rt_thread_delay(100);
		/* 线程切换，这里是手动切换 */		
		//rt_schedule();
	 }
}
/*线程入口2*/ 
void flag2_thread_entry( void *p_arg )
{
	for(;;)
	 {
		flag2=1;
		rt_enter_critical();
		sharecounter++;
		rt_exit_critical();
		if(sharecounter==0xf)
		{
			rt_thread_detach(&rt_flag4_thread);
		}
		//临界区
		//rt_uint32_t status=rt_Thread_critical_in();
		//register rt_base_t temp;
		///* 关中断 */
		//temp = rt_hw_interrupt_disable();
		//rt_uint32_t i=tickcounter; 
		rt_thread_delay(10);
		flag2=0;
		
		rt_thread_delay(10);
		//tickcounter=i+1;
		//rt_hw_interrupt_enable(temp);
		//rt_Thread_critical_out(status); 		 
		 //临界区
		 /* 线程切换，这里是手动切换 */		
		//rt_schedule();
	 }
}
/* 线程入口3 */
void flag3_thread_entry( void *p_arg )
{
	for( ;; )
	{
        flag3 = 1;
        rt_thread_suspend(&rt_flag3_thread);		
		flag3 = 0;
        rt_thread_suspend(&rt_flag3_thread); 
		rt_thread_getinfo(&rt_flag3_thread,&thread_info3);
	}
}
void flag4_thread_entry( void *p_arg )
{
	for( ;; )
	{
        flag4 = 1;
        delay(100);		
		flag4= 0;
        delay (100);
	}
}

//定时器中断处理函数
void SysTick_Handler(void)
{
    /* 进入中断 */
    rt_interrupt_enter();

    rt_tick_increase();
	//tickcounter++;
    /* 离开中断 */
    rt_interrupt_leave();
}
