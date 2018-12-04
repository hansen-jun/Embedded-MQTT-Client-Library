/**************************************************************
**  link list Library with C
**************************************************************/
/** 
 * @file        CLIB_list.h
 * @brief       Doubly Link List API with C Header
 * @author      Yan.Feng
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2016/05/23 : zhaozhenge@outlook.com 
 *                  -# New
 * @version     00.00.02 
 *              - 2018/07/20 : zhaozhenge@outlook.com 
 *                  -# Add into mbed_c_library module
 */
 
#ifndef LIST_H_
#define LIST_H_

/**************************************************************
**  Structure
**************************************************************/

/** 
 *  @brief      List Node Structure
 *  @author     Yan.Feng
 *  @date       2016/05/23
 */
typedef struct S_LIST_NODE 
{
    struct S_LIST_NODE *prev;           /*!< The pointer of the previous node */
    struct S_LIST_NODE *next;           /*!< The pointer of the next node */
} T_LIST_NODE;

/**************************************************************
**  Interface
**************************************************************/

/**
 * @addtogroup  doubly_link_list_module
 * @ingroup     mbed_c_library
 * @brief       Doubly Link List API
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/07/20 : zhaozhenge@outlook.com
 *                  -# New
 * @{ 
 */

/** 
 * @brief               initialize a Doubly Link list 
 * @param[in,out]       head            The pointer of the List Head Node
 * @return              None
 * @note                The \a head parameter identify a link list
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline void list_init(T_LIST_NODE* head) 
{
    head->next = head;
    head->prev = head;
}

/** 
 * @brief               Judge if the list is empty
 * @param[in,out]       head            The pointer of the List Head Node
 * @retval              true            List is empty
 * @retval              false           List is not empty
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline int list_empty(T_LIST_NODE* head)
{
    return (head->next == head);
}

/** 
 * @brief               Add a Node to anywhere of the List
 * @param[in,out]       node            The pointer of node that want to insert
 * @param[in,out]       p               The pointer of the previous node
 * @param[in,out]       n               The pointer of the next node
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline void list_insert(T_LIST_NODE* node, T_LIST_NODE* p, T_LIST_NODE* n) 
{
    node->next = n;
    node->prev = p;
    p->next = node;
    n->prev = node;
}

/** 
 * @brief               Delete a Node from the List
 * @param[in,out]       node            The pointer of node that want to delete
 * @param[in,out]       p               The pointer of the previous node
 * @param[in,out]       n               The pointer of the next node
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline T_LIST_NODE* list_delete(T_LIST_NODE* node, T_LIST_NODE* p, T_LIST_NODE* n) 
{
    p->next = node->next;
    n->prev = node->prev;
    return node;
}

/** 
 * @brief               Add a Node to the tail of the List
 * @param[in,out]       node            The pointer of node that want to add
 * @param[in,out]       head            The pointer of the List Head Node
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline void list_insert_tail(T_LIST_NODE* node, T_LIST_NODE* head) 
{
    list_insert(node, head->prev, head);
}

/** 
 * @brief               Add a Node to the Head of the List
 * @param[in,out]       node                The pointer of node that want to add
 * @param[in,out]       head                The pointer of the List Head Node
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline void list_insert_head(T_LIST_NODE* node, T_LIST_NODE* head) 
{
    list_insert(node, head, head->next);
}

/** 
 * @brief               Delete the last Node of the List
 * @param[in,out]       head                The pointer of the List Head Node
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline T_LIST_NODE* list_delete_tail(T_LIST_NODE* head) 
{   
    return list_delete(head->prev, head->prev->prev, head);
}

/** 
 * @brief               Delete the first Node of the List
 * @param[in,out]       head                The pointer of the List Head Node
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 * @callgraph
 * @hidecallergraph
 */
static inline T_LIST_NODE* list_delete_head(T_LIST_NODE* head)
{
    return list_delete(head->next, head, head->next->next);
}

/** 
 * @brief               List Iterator
 * @param[in,out]       head                The pointer of the List Head Node (T_LIST_NODE type)
 * @param[in,out]       pNode               The pointer of the current Node (T_LIST_NODE type)
 * @param[in,out]       pTmp                The pointer of Node (T_LIST_NODE type, temp data)
 * @return              None
 * @author              Yan.Feng
 * @date                2016/05/23
 */
#define list_for_each(head, pNode, pTmp) \
    for(pNode=head->next, pTmp=pNode->next; pNode!=head; pNode=pTmp, pTmp=pTmp->next)

/**
 * @}
 */

#endif /* LIST_H_ */
