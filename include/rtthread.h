#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

#include <rtdef.h>
#include <rtconfig.h>
#include <rtservice.h>

/*
*************************************************************************
*                               ��������
*************************************************************************
*/

/*
-------------------------------------------------------------------------
*                               �ں˶���ӿ�
-------------------------------------------------------------------------
*/
void rt_object_init(struct rt_object         *object,
                    enum rt_object_class_type type,
                    const char               *name);
rt_bool_t rt_object_is_systemobject(rt_object_t object);	//�Ƿ������Ƕ���		
void rt_object_detach(rt_object_t object);
void rt_object_delete(rt_object_t object);	
					
/*
-------------------------------------------------------------------------
*                               �߳̽ӿ�
-------------------------------------------------------------------------
*/
rt_err_t rt_thread_init(struct rt_thread *thread,
						const char *name,
                        void (*entry)(void *parameter),
                        void             *parameter,
                        void             *stack_start,
                        rt_uint32_t       stack_size,
						rt_uint8_t 		  priority,
						rt_uint32_t 	  tick);
														
void rt_schedule_remove_thread(struct rt_thread *thread);                        
void rt_schedule_insert_thread(struct rt_thread *thread);
rt_err_t rt_thread_resume(rt_thread_t thread);
rt_thread_t rt_thread_self(void);                        
rt_err_t rt_thread_startup(rt_thread_t thread);
						
rt_err_t rt_thread_suspend(rt_thread_t thread);                        
rt_err_t rt_thread_sleep(rt_tick_t tick);												
rt_err_t rt_thread_delay(rt_tick_t tick);												

						
void rt_thread_timeout(void *parameter);  
						
rt_err_t rt_thread_yield(void);		

rt_err_t rt_thread_delete(rt_thread_t thread);
rt_err_t rt_thread_detach(rt_thread_t thread);						
//�߳�״̬��Ϣ
void rt_thread_getinfo(rt_thread_t thread,thread_info_t thread_info);

/*
-------------------------------------------------------------------------
*                             �����߳̽ӿ�
-------------------------------------------------------------------------
*/                        
void rt_thread_idle_init(void);																		

/*
-------------------------------------------------------------------------
*                             �������ӿ�
-------------------------------------------------------------------------
*/
void rt_system_scheduler_init(void);
void rt_schedule_insert_thread(struct rt_thread *thread);
void rt_system_scheduler_start(void);

void rt_schedule(void);

//�ٽ�������
rt_uint32_t rt_Thread_critical_in(void);
void rt_Thread_critical_out(rt_uint32_t status);
//������
void rt_enter_critical(void);
void rt_exit_critical(void);
/*
-------------------------------------------------------------------------
*                             �жϽӿ�
-------------------------------------------------------------------------
*/                        
void rt_interrupt_enter(void);
void rt_interrupt_leave(void);
/*
-------------------------------------------------------------------------
*                             IPC�̼߳�ͨ�Žӿ�
-------------------------------------------------------------------------
*/  

/**
 * @addtogroup IPC
 */

/**@{*/

#ifdef RT_USING_SEMAPHORE
/*
 * �ź���
 */
rt_err_t rt_sem_init(rt_sem_t    sem,
                     const char *name,
                     rt_uint32_t value,
                     rt_uint8_t  flag);
rt_err_t rt_sem_detach(rt_sem_t sem);
rt_sem_t rt_sem_create(const char *name, rt_uint32_t value, rt_uint8_t flag);
rt_err_t rt_sem_delete(rt_sem_t sem);

rt_err_t rt_sem_take(rt_sem_t sem, rt_int32_t time);
rt_err_t rt_sem_trytake(rt_sem_t sem);
rt_err_t rt_sem_release(rt_sem_t sem);
rt_err_t rt_sem_control(rt_sem_t sem, int cmd, void *arg);
#endif

#ifdef RT_USING_MUTEX
/*
 * �����ź�
 */
rt_err_t rt_mutex_init(rt_mutex_t mutex, const char *name, rt_uint8_t flag);
rt_err_t rt_mutex_detach(rt_mutex_t mutex);
rt_mutex_t rt_mutex_create(const char *name, rt_uint8_t flag);
rt_err_t rt_mutex_delete(rt_mutex_t mutex);

rt_err_t rt_mutex_take(rt_mutex_t mutex, rt_int32_t time);
rt_err_t rt_mutex_release(rt_mutex_t mutex);
rt_err_t rt_mutex_control(rt_mutex_t mutex, int cmd, void *arg);
#endif

#ifdef RT_USING_EVENT
/*
 * �¼�
 */
rt_err_t rt_event_init(rt_event_t event, const char *name, rt_uint8_t flag);
rt_err_t rt_event_detach(rt_event_t event);
rt_event_t rt_event_create(const char *name, rt_uint8_t flag);
rt_err_t rt_event_delete(rt_event_t event);

rt_err_t rt_event_send(rt_event_t event, rt_uint32_t set);
rt_err_t rt_event_recv(rt_event_t   event,
                       rt_uint32_t  set,
                       rt_uint8_t   opt,
                       rt_int32_t   timeout,
                       rt_uint32_t *recved);
rt_err_t rt_event_control(rt_event_t event, int cmd, void *arg);
#endif

#ifdef RT_USING_MAILBOX
/*
* ����
 */
rt_err_t rt_mb_init(rt_mailbox_t mb,
                    const char  *name,
                    void        *msgpool,
                    rt_size_t    size,
                    rt_uint8_t   flag);
rt_err_t rt_mb_detach(rt_mailbox_t mb);
rt_mailbox_t rt_mb_create(const char *name, rt_size_t size, rt_uint8_t flag);
rt_err_t rt_mb_delete(rt_mailbox_t mb);

rt_err_t rt_mb_send(rt_mailbox_t mb, rt_ubase_t value);
rt_err_t rt_mb_send_wait(rt_mailbox_t mb,
                         rt_ubase_t  value,
                         rt_int32_t   timeout);
rt_err_t rt_mb_recv(rt_mailbox_t mb, rt_ubase_t *value, rt_int32_t timeout);
rt_err_t rt_mb_control(rt_mailbox_t mb, int cmd, void *arg);
#endif

#ifdef RT_USING_MESSAGEQUEUE
/*
 * ��Ϣ����
 */
rt_err_t rt_mq_init(rt_mq_t     mq,
                    const char *name,
                    void       *msgpool,
                    rt_size_t   msg_size,
                    rt_size_t   pool_size,
                    rt_uint8_t  flag);
rt_err_t rt_mq_detach(rt_mq_t mq);
rt_mq_t rt_mq_create(const char *name,
                     rt_size_t   msg_size,
                     rt_size_t   max_msgs,
                     rt_uint8_t  flag);
rt_err_t rt_mq_delete(rt_mq_t mq);

rt_err_t rt_mq_send(rt_mq_t mq, void *buffer, rt_size_t size);
rt_err_t rt_mq_urgent(rt_mq_t mq, void *buffer, rt_size_t size);
rt_err_t rt_mq_recv(rt_mq_t    mq,
                    void      *buffer,
                    rt_size_t  size,
                    rt_int32_t timeout);
rt_err_t rt_mq_control(rt_mq_t mq, int cmd, void *arg);
#endif

/**@}*/
/*
-------------------------------------------------------------------------
*                             ʱ����ؽӿ�
-------------------------------------------------------------------------
*/ 
void rt_tick_increase(void);
rt_tick_t rt_tick_get(void);
/*
-------------------------------------------------------------------------
*                             ��ʱ���ӿ�
-------------------------------------------------------------------------
*/
void rt_system_timer_init(void);

void rt_timer_init(rt_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   rt_tick_t   time,
                   rt_uint8_t  flag);

rt_err_t rt_timer_stop(rt_timer_t timer);

rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg);
rt_err_t rt_timer_start(rt_timer_t timer);
void rt_timer_check(void);
//ʱ�Ӱ���
rt_err_t rt_timer_detach(rt_timer_t timer);
/*
-------------------------------------------------------------------------
*                             �ں˷�����غ����ӿ�
-------------------------------------------------------------------------
*/                        
//�ú�����ָ���������ַ�����һ���ط�����������һ���ط�                       
char *rt_strncpy(char *dst, const char *src, rt_ubase_t n);  
//λͼ����				   
int __rt_ffs(int value);

#endif						
