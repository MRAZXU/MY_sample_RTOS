#include <rtthread.h>
#include <rthw.h>
#include "ARMCM3.h"
/*ȫ�ֱ���*/
rt_uint32_t flag1;
rt_uint32_t flag2;
rt_uint32_t flag3;
rt_uint32_t flag4;
//������֤�ٽ���������ȫ�ֱ���
rt_uint32_t tickcounter=0;
//��֤�������ı���
rt_uint32_t sharecounter=0;
//�߳���Ϣ
struct _thread_info thread_info3;
//���ȼ��б�
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
/**********************�߳̿��ƿ� & STACK & �߳�����*****************************/
//�����߳̿��ƿ�
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;
struct rt_thread rt_flag3_thread;
struct rt_thread rt_flag4_thread;
ALIGN(RT_ALIGN_SIZE)//���ֽڶ���
//�����߳�ջ 
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];
rt_uint8_t rt_flag4_thread_stack[512];
//�߳�����
void flag1_thread_entry(void *p_arg);
void flag2_thread_entry(void *p_arg);
void flag3_thread_entry(void *p_arg);
void flag4_thread_entry(void *p_arg);
/**********************�߳̿��ƿ� & STACK & �߳�����*****************************/

void delay (uint32_t count);
int main(void)
{
	/* ���ж� */
    rt_hw_interrupt_disable();
    
    /* SysTick�ж�Ƶ������ */ 
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );//10ms��tick
	
	/* ϵͳ��ʱ���б��ʼ�� */
	rt_system_timer_init();
	
	/*��������ʼ��*/
	rt_system_scheduler_init();
	
	/* ��ʼ�������߳� */    
    rt_thread_idle_init();
	
	/*��ʼ���߳�1*/
	rt_thread_init( &rt_flag1_thread,                 /* �߳̿��ƿ� */
                    "rt_flag1_thread",                /* �߳����֣��ַ�����ʽ */
	                flag1_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag1_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag1_thread_stack),    /* �߳�ջ��С����λΪ�ֽ� */
					3,								  /* ���ȼ�*/
					10);  							  /* ʱ��Ƭ*/
	/* ���̲߳��뵽�����б� */
	//rt_list_insert_before( &(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist) );	
	rt_thread_startup(&rt_flag1_thread);
	/*��ʼ���߳�2*/
	rt_thread_init(&rt_flag2_thread,
					"rt_flag2_thread",                /* �߳����֣��ַ�����ʽ */
					flag2_thread_entry,
					RT_NULL,
					&rt_flag2_thread_stack[0],
					sizeof(rt_flag2_thread_stack),
						3,
					10);
	/* ���̲߳��뵽�����б� */
	//rt_list_insert_before( &(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist) );	
	rt_thread_startup(&rt_flag2_thread);				
	/* ��ʼ���߳�3 */
	rt_thread_init( &rt_flag3_thread,                 /* �߳̿��ƿ� */
                    "rt_flag3_thread",                /* �߳����֣��ַ�����ʽ */
	                flag3_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag3_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag3_thread_stack),    /* �߳�ջ��С����λΪ�ֽ� */
                    3,								  /*�߳�3��2ͬһ�����ȼ�*/
					5);                               /*ʱ��Ƭ*/			
	rt_thread_startup(&rt_flag3_thread);
		/* ��ʼ���߳�4 ע����ɾ�� */
	rt_thread_init( &rt_flag4_thread,                 /* �߳̿��ƿ� */
                    "rt_flag4_thread",                /* �߳����֣��ַ�����ʽ */
	                flag4_thread_entry,               /* �߳���ڵ�ַ */
	                RT_NULL,                          /* �߳��β� */
	                &rt_flag4_thread_stack[0],        /* �߳�ջ��ʼ��ַ */
	                sizeof(rt_flag4_thread_stack),    /* �߳�ջ��С����λΪ�ֽ� */
                    3,								  /*�߳�3��2ͬһ�����ȼ�*/
					2);                               /*ʱ��Ƭ*/			
	rt_thread_startup(&rt_flag4_thread);
	/* ����ϵͳ������ */
	rt_system_scheduler_start(); 	
}
/***************************����ʵ��*****************************/
/* �����ʱ */
void delay (uint32_t count)
{
	for(; count!=0; count--);
}
/*�߳����1*/
void flag1_thread_entry( void *p_arg )
{
	for(;;) 
	 {
		
		rt_uint32_t var;
		flag1=1;
		rt_thread_resume(&rt_flag3_thread);
		//����
		rt_enter_critical();
		var=sharecounter;
		var++;
		rt_thread_delay(100);//������ʱ  
		sharecounter=var;
		//����	 
		rt_exit_critical();
		rt_thread_resume(&rt_flag3_thread);
		//thread_info3=rt_thread_getinfo(&rt_flag3_thread);
		flag1=0;
		rt_thread_delay(100);
		/* �߳��л����������ֶ��л� */		
		//rt_schedule();
	 }
}
/*�߳����2*/ 
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
		//�ٽ���
		//rt_uint32_t status=rt_Thread_critical_in();
		//register rt_base_t temp;
		///* ���ж� */
		//temp = rt_hw_interrupt_disable();
		//rt_uint32_t i=tickcounter; 
		rt_thread_delay(10);
		flag2=0;
		
		rt_thread_delay(10);
		//tickcounter=i+1;
		//rt_hw_interrupt_enable(temp);
		//rt_Thread_critical_out(status); 		 
		 //�ٽ���
		 /* �߳��л����������ֶ��л� */		
		//rt_schedule();
	 }
}
/* �߳����3 */
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

//��ʱ���жϴ�����
void SysTick_Handler(void)
{
    /* �����ж� */
    rt_interrupt_enter();

    rt_tick_increase();
	//tickcounter++;
    /* �뿪�ж� */
    rt_interrupt_leave();
}
