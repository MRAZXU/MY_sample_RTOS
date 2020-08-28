#ifndef __RT_DEF_H__
#define __RT_DEF_H__
#include <rtconfig.h>
/*
*************************************************************************
*                               ��������
*************************************************************************
*/
/* RT-Thread basic data type definitions */
typedef signed   char                   rt_int8_t;      /**<  8bit integer type */
typedef signed   short                  rt_int16_t;     /**< 16bit integer type */
typedef signed   long                   rt_int32_t;     /**< 32bit integer type */
typedef unsigned char                   rt_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  rt_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned long                   rt_uint32_t;    /**< 32bit unsigned integer type */
typedef int                             rt_bool_t;      /**< boolean type */

/* 32bit CPU */
typedef long                            rt_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   rt_ubase_t;     /**< Nbit unsigned CPU related data type */

typedef rt_base_t                       rt_err_t;       /**< Type for error number */
typedef rt_uint32_t                     rt_time_t;      /**< Type for time stamp */
typedef rt_uint32_t                     rt_tick_t;      /**< Type for tick count */
typedef rt_base_t                       rt_flag_t;      /**< Type for flags */
typedef rt_ubase_t                      rt_size_t;      /**< Type for size number */
typedef rt_ubase_t                      rt_dev_t;       /**< Type for device */
typedef rt_base_t                       rt_off_t;       /**< Type for offset */

/* boolean type definitions */
#define RT_TRUE                         1               /**< boolean true  */
#define RT_FALSE                        0               /**< boolean fails */

/* �������͵����ֵ */
#define RT_UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define RT_UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define RT_UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#define RT_TICK_MAX                     RT_UINT32_MAX   /**< Maxium number of tick */


#ifdef __CC_ARM
		#define rt_inline                   static __inline
		#define ALIGN(n)                    __attribute__((aligned(n)))

#elif defined (__IAR_SYSTEMS_ICC__)
    #define rt_inline                   static inline
		#define ALIGN(n)                    PRAGMA(data_alignment=n)
		
#elif defined (__GNUC__)
    #define rt_inline                   static __inline
		#define ALIGN(n)                    __attribute__((aligned(n)))
#else
    #error not supported tool chain			
#endif

		
#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

		
#define RT_NULL                         (0)

/*
*************************************************************************
*                               �����붨��
*************************************************************************
*/
/* RT-Thread �������ض��� */
#define RT_EOK                          0               /* û�д��� */
#define RT_ERROR                        1               /* һ��������� */
#define RT_ETIMEOUT                     2               /* ��ʱ */
#define RT_EFULL                        3               /* ��Դ���� */
#define RT_EEMPTY                       4               /* ��ԴΪ�� */
#define RT_ENOMEM                       5               /* û���ڴ� */
#define RT_ENOSYS                       6               /* No system */
#define RT_EBUSY                        7               /* æ*/
#define RT_EIO                          8               /* IO���� */
#define RT_EINTR                        9               /* �ж�ϵͳ���� */
#define RT_EINVAL                       10              /* ��Ч�β� */

/*
*************************************************************************
*                               ˫������ṹ��
*************************************************************************
*/
struct rt_list_node
{
    struct rt_list_node *next;              /* ָ���һ���ڵ� */
    struct rt_list_node *prev;              /* ָ��ǰһ���ڵ� */
};
typedef struct rt_list_node rt_list_t;                  

/*
*************************************************************************
*                               �ں˶���ṹ��
*************************************************************************
*/
/**
 * �ں˶���������ݽṹ
 */
struct rt_object
{
    char       name[RT_NAME_MAX];                       /* �ں˶�������� */
    rt_uint8_t type;                                    /* �ں˶�������� */
    rt_uint8_t flag;                                    /* �ں˶����״̬ */


    rt_list_t  list;                                    /* �ں˶�����б�ڵ� */
};
typedef struct rt_object *rt_object_t;                  /*�ں˶������������ض��� */

/**
 * ��������������ĺ���ʹ�ܣ���Щ��ͨ����rtconfig.h�ж���
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Event
 *  - MailBox
 *  - MessageQueue
 *  - MemHeap
 *  - MemPool
 *  - Device
 *  - Timer
 *  - Module
 *  - Unknown
 *  - Static
 */
enum rt_object_class_type
{
     RT_Object_Class_Thread = 0,       /* �������߳� */
     RT_Object_Class_Semaphore,        /* �������ź��� */
     RT_Object_Class_Mutex,            /* �����ǻ����� */
     RT_Object_Class_Event,            /* �������¼� */
     RT_Object_Class_MailBox,          /* ���������� */
     RT_Object_Class_MessageQueue,     /* ��������Ϣ���� */
     RT_Object_Class_MemHeap,          /* �������ڴ�� */
     RT_Object_Class_MemPool,          /* �������ڴ�� */
     RT_Object_Class_Device,           /* �������豸 */
     RT_Object_Class_Timer,            /* �����Ƕ�ʱ�� */
     RT_Object_Class_Module,           /* ������ģ�� */
     RT_Object_Class_Unknown,          /* ����δ֪ */
     RT_Object_Class_Static = 0x80     /* �����Ǿ�̬���� */
};



/**
 * �ں˶�����Ϣ�ṹ��
 */
struct rt_object_information
{
    enum rt_object_class_type type;                     /* �������� */
    rt_list_t                 object_list;              /* �����б�ڵ� */
    rt_size_t                 object_size;              /* �����С */
};
/*
*************************************************************************
*                               IPC�ṹ��
*************************************************************************
*/
/**
 * IPC flags and control command definitions
 */
#define RT_IPC_FLAG_FIFO                0x00            /**< FIFOed IPC. @ref IPC. */
#define RT_IPC_FLAG_PRIO                0x01            /**< PRIOed IPC. @ref IPC. */

#define RT_IPC_CMD_UNKNOWN              0x00            /**< unknown IPC command */
#define RT_IPC_CMD_RESET                0x01            /**< reset IPC object */

#define RT_WAITING_FOREVER              -1              /**< Block forever until get resource. */
#define RT_WAITING_NO                   0               /**< Non-block. */

/**
 * Base structure of IPC object
 */
struct rt_ipc_object
{
    struct rt_object parent;                            /**< inherit from rt_object */

    rt_list_t        suspend_thread;                    /**< threads pended on this resource */
};

#ifdef RT_USING_SEMAPHORE
/**
 * Semaphore structure
 */
struct rt_semaphore
{
    struct rt_ipc_object parent;                        /**< inherit from ipc_object */

    rt_uint16_t          value;                         /**< value of semaphore. */
    rt_uint16_t          reserved;                      /**< reserved field */
};
typedef struct rt_semaphore *rt_sem_t;
#endif

#ifdef RT_USING_MUTEX
/**
 * Mutual exclusion (mutex) structure
 */
struct rt_mutex
{
    struct rt_ipc_object parent;                        /**< inherit from ipc_object */

    rt_uint16_t          value;                         /**< value of mutex */

    rt_uint8_t           original_priority;             /**< priority of last thread hold the mutex */
    rt_uint8_t           hold;                          /**< numbers of thread hold the mutex */

    struct rt_thread    *owner;                         /**< current owner of mutex */
};
typedef struct rt_mutex *rt_mutex_t;
#endif

#ifdef RT_USING_EVENT
/**
 * flag defintions in event
 */
#define RT_EVENT_FLAG_AND               0x01            /**< logic and */
#define RT_EVENT_FLAG_OR                0x02            /**< logic or */
#define RT_EVENT_FLAG_CLEAR             0x04            /**< clear flag */

/*
 * event structure
 */
struct rt_event
{
    struct rt_ipc_object parent;                        /**< inherit from ipc_object */

    rt_uint32_t          set;                           /**< event set */
};
typedef struct rt_event *rt_event_t;
#endif

#ifdef RT_USING_MAILBOX
/**
 * mailbox structure
 */
struct rt_mailbox
{
    struct rt_ipc_object parent;                        /**< inherit from ipc_object */

    rt_ubase_t          *msg_pool;                      /**< start address of message buffer */

    rt_uint16_t          size;                          /**< size of message pool */

    rt_uint16_t          entry;                         /**< index of messages in msg_pool */
    rt_uint16_t          in_offset;                     /**< input offset of the message buffer */
    rt_uint16_t          out_offset;                    /**< output offset of the message buffer */

    rt_list_t            suspend_sender_thread;         /**< sender thread suspended on this mailbox */
};
typedef struct rt_mailbox *rt_mailbox_t;
#endif

#ifdef RT_USING_MESSAGEQUEUE
/**
 * message queue structure
 */
struct rt_messagequeue
{
    struct rt_ipc_object parent;                        /**< inherit from ipc_object */

    void                *msg_pool;                      /**< start address of message queue */

    rt_uint16_t          msg_size;                      /**< message size of each message */
    rt_uint16_t          max_msgs;                      /**< max number of messages */

    rt_uint16_t          entry;                         /**< index of messages in the queue */

    void                *msg_queue_head;                /**< list head */
    void                *msg_queue_tail;                /**< list tail */
    void                *msg_queue_free;                /**< pointer indicated the free node of queue */
};
typedef struct rt_messagequeue *rt_mq_t;
#endif
/*
*************************************************************************
*                               ʱ��&��ʱ��
*************************************************************************
*/
/**
 * ʱ�� & ��ʱ�� �궨��
 */
#define RT_TIMER_FLAG_DEACTIVATED       0x0     /* ��ʱ��û�м��� */
#define RT_TIMER_FLAG_ACTIVATED         0x1     /* ��ʱ���Ѿ����� */
#define RT_TIMER_FLAG_ONE_SHOT          0x0     /* ���ζ�ʱ */
#define RT_TIMER_FLAG_PERIODIC          0x2     /* ���ڶ�ʱ */

#define RT_TIMER_FLAG_HARD_TIMER        0x0     /* Ӳ����ʱ������ʱ���ص������� tick isr�е��� */

#define RT_TIMER_FLAG_SOFT_TIMER        0x4     /* �����ʱ������ʱ���ص������ڶ�ʱ���߳��е��� */


#define RT_TIMER_CTRL_SET_TIME          0x0     /* ���ö�ʱ����ʱʱ�� */
#define RT_TIMER_CTRL_GET_TIME          0x1     /* ��ȡ��ʱ����ʱʱ�� */
#define RT_TIMER_CTRL_SET_ONESHOT       0x2     /* �޸Ķ�ʱ��Ϊһ�ζ�ʱ */
#define RT_TIMER_CTRL_SET_PERIODIC      0x3     /* �޸Ķ�ʱ��Ϊ���ڶ�ʱ */

#ifndef RT_TIMER_SKIP_LIST_LEVEL
#define RT_TIMER_SKIP_LIST_LEVEL          1
#endif

/* 1 or 3 */
#ifndef RT_TIMER_SKIP_LIST_MASK
#define RT_TIMER_SKIP_LIST_MASK         0x3
#endif

/**
 * ��ʱ���ṹ��
 */
struct rt_timer
{
    struct rt_object parent;                         /* �� rt_object �̳� */

    rt_list_t row[RT_TIMER_SKIP_LIST_LEVEL];

    void (*timeout_func)(void *parameter);           /* ��ʱ���� */
    void            *parameter;                      /* ��ʱ�����β� */

    rt_tick_t        init_tick;                      /* ��ʱ��ʵ����Ҫ��ʱ��ʱ�� */
    rt_tick_t        timeout_tick;                   /* ��ʱ��ʵ�ʳ�ʱʱ��ϵͳ������ */
};
typedef struct rt_timer *rt_timer_t;
/*
*************************************************************************
*                               �߳̽ṹ��
*************************************************************************
*/

struct rt_thread
{
	/* rt ���� */
    char        name[RT_NAME_MAX];    /* ��������� */
    rt_uint8_t  type;                 /* �������� */
    rt_uint8_t  flags;                /* �����״̬ */
    rt_list_t   list;                 /* ������б�ڵ� */
    
	rt_list_t   tlist;                /* �߳�����ڵ� */
	
	void        *sp;	          /* �߳�ջָ�� */
	void        *entry;	          /* �߳���ڵ�ַ */
	void        *parameter;	      /* �߳��β� */	
	void        *stack_addr;      /* �߳���ʼ��ַ */
	rt_uint32_t stack_size;       /* �߳�ջ��С����λΪ�ֽ� */
	
	rt_ubase_t  init_tick;            /* ��ʼʱ��Ƭ */
    rt_ubase_t  remaining_tick;       /* ʣ��ʱ��Ƭ */ 
    
    void (*cleanup)(struct rt_thread *tid); /*�߳��������*/
	
	rt_uint8_t  current_priority;     /* ��ǰ���ȼ� */
    rt_uint8_t  init_priority;        /* ��ʼ���ȼ� */
    rt_uint32_t number_mask;          /* ��ǰ���ȼ����� */
	
	rt_err_t    error;                /* ������ */
    rt_uint8_t  stat;                 /* �̵߳�״̬ */
	
	struct rt_timer thread_timer; /* ���õ��̶߳�ʱ�� */
    
};

typedef struct rt_thread *rt_thread_t;

struct _thread_info
{
	rt_uint32_t delayticks;
	rt_uint32_t prio;
	rt_uint32_t state;
	rt_uint32_t left_ticks;
	
};
typedef struct _thread_info *thread_info_t;
/*
 * �߳�״̬����
 */
#define RT_THREAD_INIT                  0x00                /* ��ʼ̬ */
#define RT_THREAD_READY                 0x01                /* ����̬ */
#define RT_THREAD_SUSPEND               0x02                /* ����̬ */
#define RT_THREAD_RUNNING               0x03                /* ����̬ */
#define RT_THREAD_BLOCK                 RT_THREAD_SUSPEND   /* ����̬ */
#define RT_THREAD_CLOSE                 0x04                /* �ر�̬ */
#define RT_THREAD_STAT_MASK             0x0f

#define RT_THREAD_STAT_SIGNAL           0x10
#define RT_THREAD_STAT_SIGNAL_READY     (RT_THREAD_STAT_SIGNAL | RT_THREAD_READY)
#define RT_THREAD_STAT_SIGNAL_SUSPEND   0x20
#define RT_THREAD_STAT_SIGNAL_MASK      0xf0

#endif  /* __RT_DEF_H__ */
