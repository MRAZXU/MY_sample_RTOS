#include <rtthread.h>
#include <rthw.h>
extern struct rt_thread idle;
extern struct rt_thread rt_flag1_thread;
extern struct rt_thread rt_flag2_thread;
/*
*************************************************************************
*                                 ȫ�ֱ���
*************************************************************************
*/
/* �߳̿��ƿ�ָ�룬����ָ��ǰ�߳� */
struct rt_thread *rt_current_thread;

/* �߳����ȼ��� */
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/* �߳̾������ȼ��� */
rt_uint32_t rt_thread_ready_priority_group;

/* ��ǰ���ȼ� */
rt_uint8_t rt_current_priority;

/* �߳������б� */
rt_list_t rt_thread_defunct;

/* ������������ */
static rt_int16_t rt_scheduler_lock_nest;
/*
*************************************************************************
*                               ����������
*************************************************************************
*/

/* ��ʼ��ϵͳ������ ******************************************************************/
void rt_system_scheduler_init(void)
{	
	register rt_base_t offset;	//register��ֹ���Ż�

	
	/* �߳̾����б��ʼ�� */
	for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset ++)
	{
			rt_list_init(&rt_thread_priority_table[offset]);
	}
	/* ��ʼ����ǰ���ȼ�Ϊ�����̵߳����ȼ� */
    rt_current_priority = RT_THREAD_PRIORITY_MAX - 1;
    
	/* ��ʼ����ǰ�߳̿��ƿ�ָ�� */
	rt_current_thread = RT_NULL;
    
    /* ��ʼ���߳̾������ȼ��� */
    rt_thread_ready_priority_group = 0;
}

/* ����ϵͳ������ ****************************************************************************/
void rt_system_scheduler_start(void)
{
	register struct rt_thread *to_thread;
    register rt_ubase_t highest_ready_priority;
                              
    /* ��ȡ������������ȼ� */
	//λͼ������
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
    //�����滹�� rt_container_of ͨ���ṹ���Ա�����ƶϽṹ�������ʼ��ַ�ķ���                        
    /* ��ȡ��Ҫ�����̵߳��߳̿��ƿ� */
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                              struct rt_thread,
                              tlist);

    rt_current_thread = to_thread;

    /* �л����µ��߳� */
    rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);

    /* ��Զ���᷵�� */ 
}

/*�����������߳�***********************************************************************************/
//��������߳����ȼ��б�Ĳ���
void rt_schedule_insert_thread(struct rt_thread *thread)
{
    register rt_base_t temp;

    /* ���ж� */
    temp = rt_hw_interrupt_disable();

    /* �ı��߳�״̬ */
    thread->stat = RT_THREAD_READY;

    /* ���̲߳�������б� */
    rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                          &(thread->tlist));

    /* �����߳̾������ȼ����ж�Ӧ��λ */
    rt_thread_ready_priority_group |= thread->number_mask;

    /* ���ж� */
    rt_hw_interrupt_enable(temp);
}

/*������ɾ���߳�**********************************************************************************/
void rt_schedule_remove_thread(struct rt_thread *thread)
{
    register rt_base_t temp;


    /* ���ж� */
    temp = rt_hw_interrupt_disable();
    
    /* ���̴߳Ӿ����б�ɾ�� */
    rt_list_remove(&(thread->tlist));
    
    if (rt_list_isempty(&(rt_thread_priority_table[thread->current_priority])))
    {
        rt_thread_ready_priority_group &= ~thread->number_mask;
    }

    /* ���ж� */
    rt_hw_interrupt_enable(temp);
}

/* ϵͳ���� ******************************************************************************************/
void rt_schedule(void)
{
	rt_base_t level;
    register rt_ubase_t highest_ready_priority;
    struct rt_thread *to_thread;
    struct rt_thread *from_thread;
    /* ���ж� */
    level = rt_hw_interrupt_disable();
    /* ��ȡ������������ȼ� */
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
    /* ��ȡ������������ȼ���Ӧ���߳̿��ƿ� */
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                              struct rt_thread,
                              tlist);
	/* ����������Ѿ��������˳� */
	if (rt_scheduler_lock_nest == 0)
	{
		/* ���Ŀ���̲߳��ǵ�ǰ�̣߳���Ҫ�����߳��л� */
		if (to_thread != rt_current_thread)
		{
			rt_current_priority = (rt_uint8_t)highest_ready_priority;
			from_thread         = rt_current_thread;
			rt_current_thread   = to_thread;
			rt_hw_context_switch((rt_uint32_t)&from_thread->sp,(rt_uint32_t)&to_thread->sp);
		}
	}
	/* ���ж� */
	rt_hw_interrupt_enable(level);  
}
/*
*************************************************************************
*                               �����������˴�������
*************************************************************************
*/
void rt_enter_critical(void)
{
    register rt_base_t level;
	//������Ҳ�������ٽ�������
    /* disable interrupt */
    level = rt_hw_interrupt_disable();

	if(rt_scheduler_lock_nest<255)
	{
    rt_scheduler_lock_nest ++;	
	}
    rt_hw_interrupt_enable(level);
}

void rt_exit_critical(void)
{
    register rt_base_t level;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    rt_scheduler_lock_nest --;
    if (rt_scheduler_lock_nest <= 0)
    {
        rt_scheduler_lock_nest = 0;
        /* enable interrupt */
        rt_hw_interrupt_enable(level);

        if (rt_current_thread)
        {
            /* if scheduler is started, do a schedule */
            rt_schedule();
        }
    }
    else
    {
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
    }
}
