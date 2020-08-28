#include <rtthread.h>
#include <rthw.h>
#include "ARMCM3.h"
/* �жϼ����� */
volatile rt_uint8_t rt_interrupt_nest;

/**
 * ��BSP�ļ����жϷ���������ʱ����øú���
 * 
 * @note �벻Ҫ��Ӧ�ó����е��øú���
 *
 * @see rt_interrupt_leave
 */
void rt_interrupt_enter(void)
{
    rt_base_t level;
    
    
    /* ���ж� */
    level = rt_hw_interrupt_disable();
    
    /* �жϼ�����++ */
    rt_interrupt_nest ++;
    
    /* ���ж� */
    rt_hw_interrupt_enable(level);
}


/**
 * ��BSP�ļ����жϷ������뿪ʱ����øú���
 *
 * @note �벻Ҫ��Ӧ�ó����е��øú���
 *
 * @see rt_interrupt_enter
 */
void rt_interrupt_leave(void)
{
    rt_base_t level;
    
    
    /* ���ж� */
    level = rt_hw_interrupt_disable();
    
    /* �жϼ�����-- */
    rt_interrupt_nest --;

    /* ���ж� */
    rt_hw_interrupt_enable(level);
}

/*�ٽ������뱣��*/
//��һ���ֵĴ�����rt_hw_interrupt_disable��.s�ļ����Ѿ���������
rt_uint32_t rt_Thread_critical_in(void)
{
	rt_uint32_t primask=__get_PRIMASK();//��ȡprimask
	__disable_irq();
	return primask;
}

void rt_Thread_critical_out(rt_uint32_t status)
{
	__set_PRIMASK(status);
}
