#ifndef __RT_SERVICE_H__
#define __RT_SERVICE_H__

/* 通过链表指针找到结构体任何成员变量的地址 */
//linux中也有 
//结构体变量ptr，成员类型type，成员member
//&为取地址符
//结构体的首地址=成员地址-偏移量
//成员地址：(char *)(ptr)
//偏移地址：&((type *)0)->member)
#define rt_container_of(ptr, type, member) \
    ((type *)  ((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
/* 已知一个结构体里面的成员的地址，反推出该结构体的首地址 */			
#define rt_list_entry(node, type, member) \
    rt_container_of(node, type, member)
		
/*
*************************************************************************
*                           双向链表操作相关函数
*************************************************************************
*/
		
/* 初始化链表节点 */
rt_inline void rt_list_init(rt_list_t *l)
{
    l->next = l->prev = l;
}

/* 在双向链头尾部插入一个节点 */
rt_inline void rt_list_insert_before(rt_list_t *l, rt_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/* 在双向链表尾部插入一个节点 */
rt_inline void rt_list_insert_after(rt_list_t *l, rt_list_t *n)
{
//先对付prev
    l->next->prev = n;
    n->next = l->next;
//后对付next
    l->next = n;
    n->prev = l;
}



/* 从双向链表删除一个节点 */
rt_inline void rt_list_remove(rt_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}
/**
 * @brief 判断列表是否为空
 * @param l 需要判断的list
 */
rt_inline int rt_list_isempty(const rt_list_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
rt_inline unsigned int rt_list_len(const rt_list_t *l)
{
    unsigned int len = 0;
    const rt_list_t *p = l;
    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

/**
 * rt_list_for_each - iterate over a list
 * @pos:	the rt_list_t * to use as a loop cursor.
 * @head:	the head for your list.
 */
#define rt_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * rt_list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the rt_list_t * to use as a loop cursor.
 * @n:		another rt_list_t * to use as temporary storage
 * @head:	the head for your list.
 */
#define rt_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * rt_list_for_each_entry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define rt_list_for_each_entry(pos, head, member) \
    for (pos = rt_list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = rt_list_entry(pos->member.next, typeof(*pos), member))
/**
 * rt_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#define rt_list_for_each_entry_safe(pos, n, head, member) \
    for (pos = rt_list_entry((head)->next, typeof(*pos), member), \
         n = rt_list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = rt_list_entry(n->member.next, typeof(*n), member))	
/**
 * rt_list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define rt_list_first_entry(ptr, type, member) \
    rt_list_entry((ptr)->next, type, member)
	
/*剩下的就是单链表的实现*/	
	
	
	
#endif /* __RT_SERVICE_H__ */
