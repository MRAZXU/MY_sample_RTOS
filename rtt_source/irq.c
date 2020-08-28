#include <rtthread.h>
#include <rthw.h>
#include "ARMCM3.h"
/* 中断计数器 */
volatile rt_uint8_t rt_interrupt_nest;

/**
 * 当BSP文件的中断服务函数进入时会调用该函数
 * 
 * @note 请不要在应用程序中调用该函数
 *
 * @see rt_interrupt_leave
 */
void rt_interrupt_enter(void)
{
    rt_base_t level;
    
    
    /* 关中断 */
    level = rt_hw_interrupt_disable();
    
    /* 中断计数器++ */
    rt_interrupt_nest ++;
    
    /* 开中断 */
    rt_hw_interrupt_enable(level);
}


/**
 * 当BSP文件的中断服务函数离开时会调用该函数
 *
 * @note 请不要在应用程序中调用该函数
 *
 * @see rt_interrupt_enter
 */
void rt_interrupt_leave(void)
{
    rt_base_t level;
    
    
    /* 关中断 */
    level = rt_hw_interrupt_disable();
    
    /* 中断计数器-- */
    rt_interrupt_nest --;

    /* 开中断 */
    rt_hw_interrupt_enable(level);
}

/*临界区代码保护*/
//这一部分的代码在rt_hw_interrupt_disable的.s文件中已经有了体现
rt_uint32_t rt_Thread_critical_in(void)
{
	rt_uint32_t primask=__get_PRIMASK();//获取primask
	__disable_irq();
	return primask;
}

void rt_Thread_critical_out(rt_uint32_t status)
{
	__set_PRIMASK(status);
}
