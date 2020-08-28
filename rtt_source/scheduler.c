#include <rtthread.h>
#include <rthw.h>
extern struct rt_thread idle;
extern struct rt_thread rt_flag1_thread;
extern struct rt_thread rt_flag2_thread;
/*
*************************************************************************
*                                 全局变量
*************************************************************************
*/
/* 线程控制块指针，用于指向当前线程 */
struct rt_thread *rt_current_thread;

/* 线程优先级表 */
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/* 线程就绪优先级组 */
rt_uint32_t rt_thread_ready_priority_group;

/* 当前优先级 */
rt_uint8_t rt_current_priority;

/* 线程休眠列表 */
rt_list_t rt_thread_defunct;

/* 调度锁计数器 */
static rt_int16_t rt_scheduler_lock_nest;
/*
*************************************************************************
*                               调度器工作
*************************************************************************
*/

/* 初始化系统调度器 ******************************************************************/
void rt_system_scheduler_init(void)
{	
	register rt_base_t offset;	//register防止被优化

	
	/* 线程就绪列表初始化 */
	for (offset = 0; offset < RT_THREAD_PRIORITY_MAX; offset ++)
	{
			rt_list_init(&rt_thread_priority_table[offset]);
	}
	/* 初始化当前优先级为空闲线程的优先级 */
    rt_current_priority = RT_THREAD_PRIORITY_MAX - 1;
    
	/* 初始化当前线程控制块指针 */
	rt_current_thread = RT_NULL;
    
    /* 初始化线程就绪优先级组 */
    rt_thread_ready_priority_group = 0;
}

/* 启动系统调度器 ****************************************************************************/
void rt_system_scheduler_start(void)
{
	register struct rt_thread *to_thread;
    register rt_ubase_t highest_ready_priority;
                              
    /* 获取就绪的最高优先级 */
	//位图法查找
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
    //这里面还有 rt_container_of 通过结构体成员变量推断结构体变量起始地址的方法                        
    /* 获取将要运行线程的线程控制块 */
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                              struct rt_thread,
                              tlist);

    rt_current_thread = to_thread;

    /* 切换到新的线程 */
    rt_hw_context_switch_to((rt_uint32_t)&to_thread->sp);

    /* 永远不会返回 */ 
}

/*调度器插入线程***********************************************************************************/
//这里就有线程优先级列表的插入
void rt_schedule_insert_thread(struct rt_thread *thread)
{
    register rt_base_t temp;

    /* 关中断 */
    temp = rt_hw_interrupt_disable();

    /* 改变线程状态 */
    thread->stat = RT_THREAD_READY;

    /* 将线程插入就绪列表 */
    rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                          &(thread->tlist));

    /* 设置线程就绪优先级组中对应的位 */
    rt_thread_ready_priority_group |= thread->number_mask;

    /* 开中断 */
    rt_hw_interrupt_enable(temp);
}

/*调度器删除线程**********************************************************************************/
void rt_schedule_remove_thread(struct rt_thread *thread)
{
    register rt_base_t temp;


    /* 关中断 */
    temp = rt_hw_interrupt_disable();
    
    /* 将线程从就绪列表删除 */
    rt_list_remove(&(thread->tlist));
    
    if (rt_list_isempty(&(rt_thread_priority_table[thread->current_priority])))
    {
        rt_thread_ready_priority_group &= ~thread->number_mask;
    }

    /* 开中断 */
    rt_hw_interrupt_enable(temp);
}

/* 系统调度 ******************************************************************************************/
void rt_schedule(void)
{
	rt_base_t level;
    register rt_ubase_t highest_ready_priority;
    struct rt_thread *to_thread;
    struct rt_thread *from_thread;
    /* 关中断 */
    level = rt_hw_interrupt_disable();
    /* 获取就绪的最高优先级 */
    highest_ready_priority = __rt_ffs(rt_thread_ready_priority_group) - 1;
    /* 获取就绪的最高优先级对应的线程控制块 */
    to_thread = rt_list_entry(rt_thread_priority_table[highest_ready_priority].next,
                              struct rt_thread,
                              tlist);
	/* 如果调度器已经上锁，退出 */
	if (rt_scheduler_lock_nest == 0)
	{
		/* 如果目标线程不是当前线程，则要进行线程切换 */
		if (to_thread != rt_current_thread)
		{
			rt_current_priority = (rt_uint8_t)highest_ready_priority;
			from_thread         = rt_current_thread;
			rt_current_thread   = to_thread;
			rt_hw_context_switch((rt_uint32_t)&from_thread->sp,(rt_uint32_t)&to_thread->sp);
		}
	}
	/* 开中断 */
	rt_hw_interrupt_enable(level);  
}
/*
*************************************************************************
*                               调度锁（单核处理器）
*************************************************************************
*/
void rt_enter_critical(void)
{
    register rt_base_t level;
	//这里面也加入了临界区保护
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
