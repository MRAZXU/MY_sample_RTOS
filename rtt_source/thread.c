#include <rtthread.h>
#include <rthw.h>
extern struct rt_thread *rt_current_thread;
extern rt_uint32_t rt_thread_ready_priority_group;
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_list_t rt_thread_defunct;
rt_err_t rt_thread_init(struct rt_thread *thread,
						const char *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
						rt_uint8_t 		  priority,
						rt_uint32_t tick)
{
    /*�̶߳����ʼ��*/
	rt_object_init((rt_object_t)thread, RT_Object_Class_Thread, name);
	/* ��ʼ���߳����� */
    rt_list_init(&(thread->tlist));

    thread->entry = (void *)entry;
    thread->parameter = parameter;

    /* �߳�ջ ��ʼ�� �������߳�ջָ��*/
    thread->stack_addr = stack_start;
    thread->stack_size = stack_size;


    thread->sp = 
	(void *)rt_hw_stack_init(thread->entry, 
						     thread->parameter,
    (void *)((char *)thread->stack_addr + thread->stack_size -4));
	
	thread->init_priority    = priority;
    thread->current_priority = priority;
    thread->number_mask = 0;
    
	
    /* �������״̬ */
    thread->error = RT_EOK;
    thread->stat  = RT_THREAD_INIT;
	
	/* ʱ��Ƭ��� */
	thread->init_tick = tick;
	thread->remaining_tick = tick;
	
	/* ��ʼ���̶߳�ʱ�� */
    rt_timer_init(&(thread->thread_timer),     /* ��̬��ʱ������ */
                  thread->name,                /* ��ʱ�������֣�ֱ��ʹ�õ����̵߳����� */
                  rt_thread_timeout,           /* ��ʱ���� */
                  thread,                      /* ��ʱ�����β� */
                  0,                           /* ��ʱʱ�� */
                  RT_TIMER_FLAG_ONE_SHOT);     /* ��ʱ���ı�־ */
	
    return RT_EOK;
}
/*�����߳�****************************************************************/
rt_err_t rt_thread_startup(rt_thread_t thread)
{
    /* ���õ�ǰ���ȼ�Ϊ��ʼ���ȼ� */
    thread->current_priority = thread->init_priority;
    thread->number_mask = 1L << thread->current_priority;    
    /* �ı��̵߳�״̬Ϊ����״̬ */
    thread->stat = RT_THREAD_SUSPEND;    
    /* Ȼ��ָ��߳� */
    rt_thread_resume(thread);
    if (rt_thread_self() != RT_NULL)
    {
        /* ϵͳ���� */
        rt_schedule();
    }
    return RT_EOK;
}
/*�ָ��߳�*******************************************************/
rt_err_t rt_thread_resume(rt_thread_t thread)
{
    register rt_base_t temp;
    /* �����ָ����̱߳����ڹ���̬�����򷵻ش����� */
    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_SUSPEND)
    {
        return -RT_ERROR;
    }
    /* ���ж� */
    temp = rt_hw_interrupt_disable();
    /* �ӹ�������Ƴ� */
    rt_list_remove(&(thread->tlist));
    /* ���ж� */
    rt_hw_interrupt_enable(temp);
    /* ��������б� */
    rt_schedule_insert_thread(thread);
    return RT_EOK;
}
/*��⵱ǰ���߳�*/
rt_thread_t rt_thread_self(void)
{
    return rt_current_thread;
}
/*�̹߳���*************************************************/
rt_err_t rt_thread_suspend(rt_thread_t thread)
{
    register rt_base_t temp;
    /* ֻ�о������̲߳��ܱ����𣬷����˳����ش����� */
    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_READY)
    {
        return -RT_ERROR;
    }
    /* ���ж� */
    temp = rt_hw_interrupt_disable();
    /* �ı��߳�״̬ */
    thread->stat = RT_THREAD_SUSPEND;
    /* ���̴߳Ӿ����б�ɾ�� */
    rt_schedule_remove_thread(thread);
    /* ֹͣ�̶߳�ʱ�� */
    rt_timer_stop(&(thread->thread_timer));
    /* ���ж� */
    rt_hw_interrupt_enable(temp);
    return RT_EOK;
}
/*�߳�˯��(����������)**********************************************/
rt_err_t rt_thread_sleep(rt_tick_t tick)
{
    register rt_base_t temp;
    struct rt_thread *thread;
    /* ���ж� */
    temp = rt_hw_interrupt_disable();
    /* ��ȡ��ǰ�̵߳��߳̿��ƿ� */
    thread = rt_current_thread;
    /* �����߳� */
    rt_thread_suspend(thread);
    /* �����̶߳�ʱ���ĳ�ʱʱ�� */
    rt_timer_control(&(thread->thread_timer), RT_TIMER_CTRL_SET_TIME, &tick);
    /* ������ʱ�� */
    rt_timer_start(&(thread->thread_timer));
    /* ���ж� */
    rt_hw_interrupt_enable(temp);
    /* ִ��ϵͳ���� */
    rt_schedule();
    return RT_EOK;
}
/*�߳���ʱ***********************************************************/
rt_err_t rt_thread_delay(rt_tick_t tick)
{
	return rt_thread_sleep(tick);
}

/*�̳߳�ʱ����*******************************************************/
void rt_thread_timeout(void *parameter)
{
    struct rt_thread *thread;
    thread = (struct rt_thread *)parameter;
    /* ���ô�����Ϊ��ʱ */
    thread->error = -RT_ETIMEOUT;
    /* ���̴߳ӹ����б���ɾ�� */
    rt_list_remove(&(thread->tlist));
    /* ���̲߳��뵽�����б� */
    rt_schedule_insert_thread(thread);
    /* ϵͳ���� */
    rt_schedule();
}
/*�ú������õ�ǰ�߳��ó�������*****************************************/
rt_err_t rt_thread_yield(void)
{
    register rt_base_t level;
    struct rt_thread *thread;

    /* ���ж� */
    level = rt_hw_interrupt_disable();

    /* ��ȡ��ǰ�̵߳��߳̿��ƿ� */
    thread = rt_current_thread;
    
    /* ����߳��ھ���̬����ͬһ�����ȼ��²�ֹһ���߳� */
    if ((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY &&
        thread->tlist.next != thread->tlist.prev)
    {
        /* ��ʱ��Ƭ������̴߳Ӿ����б��Ƴ� */
        rt_list_remove(&(thread->tlist));
        
        /* ���̲߳��뵽�����ȼ��µ������β�� */
        rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                              &(thread->tlist));

        /* ���ж� */
        rt_hw_interrupt_enable(level);
        /* ִ�е��� */
        rt_schedule();
        return RT_EOK;
    }
    /* ���ж� */
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}
/******************************************************/
/*�߳�ɾ������Դ�ͷţ�***********************************/
//1.rt_thread_delete
//2.rt_thread_detach
/**
 *�˹��ܽ�ɾ��һ���̡߳��̶߳��󽫴���ɾ��
 *�̶߳��У����ӿ����߳��е�ϵͳ���������ɾ��
 * @param thread the thread to be deleted
 * @return the operation status, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_thread_delete(rt_thread_t thread)
{
    rt_base_t lock;

    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_INIT)
    {
        /* remove from schedule */
        rt_schedule_remove_thread(thread);
    }

    /* release thread timer */
    rt_timer_detach(&(thread->thread_timer));

    /* change stat */
    thread->stat = RT_THREAD_CLOSE;

    /* disable interrupt */
    lock = rt_hw_interrupt_disable();

    /* insert to defunct thread list */
    rt_list_insert_after(&rt_thread_defunct, &(thread->tlist));

    /* enable interrupt */
    rt_hw_interrupt_enable(lock);

    return RT_EOK;
}


/**
 *�˹��ܽ������̡߳��̶߳��󽫴���ɾ����Ƚ�delete ���˶�����벿��
 *�̶߳��У�����ϵͳ��������з���/ɾ����
 * @param thread the thread to be deleted
 * @return the operation status, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_thread_detach(rt_thread_t thread)
{
    rt_base_t lock;
    if ((thread->stat & RT_THREAD_STAT_MASK) != RT_THREAD_INIT)
    {
        /* remove from schedule */
        rt_schedule_remove_thread(thread);
    }
    /* release thread timer */
    rt_timer_detach(&(thread->thread_timer));
    /* change stat */
    thread->stat = RT_THREAD_CLOSE;
	//��ͬ��delete�ĵط�
    if ((rt_object_is_systemobject((rt_object_t)thread) == RT_TRUE) &&
        thread->cleanup == RT_NULL)
    {
        rt_object_detach((rt_object_t)thread);
    }
    else
    {
        /* disable interrupt */
        lock = rt_hw_interrupt_disable();
        /* insert to defunct thread list */
        rt_list_insert_after(&rt_thread_defunct, &(thread->tlist));
        /* enable interrupt */
        rt_hw_interrupt_enable(lock);
    }
    return RT_EOK;
}
/********�����߳���Ҫ��Ϣ��ȡ����*******/
void rt_thread_getinfo(struct rt_thread *thread,struct _thread_info *thread_info)
{
	rt_base_t lock;
	
	/* disable interrupt */
    lock = rt_hw_interrupt_disable();
	thread_info->delayticks=thread->init_tick;
	thread_info->left_ticks=thread->remaining_tick;
	thread_info->prio=thread->current_priority;
	thread_info->state=thread->stat;
	
	/* enable interrupt */
    rt_hw_interrupt_enable(lock);
}

