/*
 *  Copyright (C) 2018, ZhaoZhenge, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**************************************************************
**  Mbed MQTT Client Library with C
**************************************************************/
/** 
 * @file        MQC_queue.c
 * @brief       MQTT Client Library Message Queue Management
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/11/30 : zhaozhenge@outlook.com 
 *                  -# New
 * @version     00.00.02 
 *              - 2018/12/12 : zhaozhenge@outlook.com 
 *                  -# Modify some comment
 */

/**************************************************************
**  Include
**************************************************************/

#include "../inc/MQC_queue.h"

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               calculate the time passed
 * @param[in]           SystimeCountOrig        Base System timer Count
 * @param[in]           SystimeCountNow         Now System timer Count
 * @return              Passed time
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
static uint32_t prvCheckPassTime(uint32_t SystimeCountOrig, uint32_t SystimeCountNow)
{
    uint32_t Ret = 0;
    if(SystimeCountOrig <= SystimeCountNow)
    {
        Ret = SystimeCountNow - SystimeCountOrig;
    }
    else
    {
        Ret = 0xFFFFFFFF - SystimeCountOrig + SystimeCountNow;
    }
    return Ret;
}

/** 
 * @brief               Create a new Message Queue Management Handler
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           SysTimeCount            System timer count when start the timer handler
 * @retval              D_MQC_RET_OK
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_MsgQueue_create(S_MQC_MSG_QUEUE* MsgQueue, uint32_t SysTimeCount)
{
    /* Internal module , do not need to check the input data */
    memset(MsgQueue, 0, sizeof(S_MQC_MSG_QUEUE));
    MsgQueue->ListCount         =   0;
    MsgQueue->MnotonicTime      =   SysTimeCount;
    MsgQueue->PacketIdentifier  =   0;
    list_init(&(MsgQueue->MsgList));
    list_init(&(MsgQueue->ExecMsgList));
    return D_MQC_RET_OK;
}

/** 
 * @brief               Delete a Message Queue Managment handler
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @retval              D_MQC_RET_OK
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_MsgQueue_delete(S_MQC_MSG_QUEUE* MsgQueue)
{
    /* Internal module , do not need to check the input data */
    memset(MsgQueue, 0, sizeof(S_MQC_MSG_QUEUE));
    return D_MQC_RET_OK;
}

/** 
 * @brief               Search a Message by Packet Identifier and Message Type from the Message Queue
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           PacketIdentifier        Packet Identifier
 * @param[in]           MsgType                 Message Type
 * @return              The pointer of found Request Data 
 * @note                NULL maybe returned if no Message has been found
 * @note                The found Message do not pop from the queue
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern S_MQC_MSG_CTX* MQC_MsgQueue_search(S_MQC_MSG_QUEUE* MsgQueue, uint16_t PacketIdentifier, E_MQC_MSG_TYPE MsgType)
{
    T_LIST_NODE*    Node        =   NULL;
    T_LIST_NODE*    TmpNode     =   NULL;
    S_MQC_MSG_CTX*  Message     =   NULL;
    
    /* Search the Message in queue */
    list_for_each((&(MsgQueue->MsgList)), Node, TmpNode)
    {
        Message = (S_MQC_MSG_CTX*)Node;
        if( (Message->PacketIdentifier == PacketIdentifier) && ((Message->MsgData[0] >> 4) == MsgType) )
        {
            return Message;
        }
    }
    return NULL;
}

/** 
 * @brief               Push the Message into Message Queue
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           Message                 Message that want to push in the queue
 * @return              NULL or The pointer of the popped Message 
 * @note                If the Queue is full, will pop the head data of the Queue to create space
 * @note                NULL will be returned if No Data popped
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
extern S_MQC_MSG_CTX* MQC_MsgQueue_push( S_MQC_MSG_QUEUE* MsgQueue, S_MQC_MSG_CTX* Message )
{
    T_LIST_NODE*    Node    =   NULL;
    
    /* Insert */
    Node = (T_LIST_NODE*)Message;
    list_insert_tail(Node, (&(MsgQueue->MsgList)));
    return NULL;
}

/** 
 * @brief               Pop the head Message of the Queue
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @return              The pointer of the popped Message
 * @note                NULL maybe returned if Queue empty (No Message popped)
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern S_MQC_MSG_CTX* MQC_MsgQueue_pop(S_MQC_MSG_QUEUE* MsgQueue)
{
    T_LIST_NODE*    Node        =   NULL;
    S_MQC_MSG_CTX*  Message     =   NULL;
    
    if( !list_empty((&(MsgQueue->MsgList))) )
    {
        Node = list_delete_head((&(MsgQueue->MsgList)));
        Message = (S_MQC_MSG_CTX*)Node;
        MsgQueue->ListCount--;
    }
    return Message;
}

/** 
 * @brief               Slice the indicated Message from the Queue
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           Message                 The Message which want to be sliced from the Queue
 * @return              None
 * @note                The indicated Message should be actually in the Queue
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/12
 * @callgraph
 * @callergraph
 */
extern void MQC_MsgQueue_slice(S_MQC_MSG_QUEUE* MsgQueue, S_MQC_MSG_CTX* Message)
{
    T_LIST_NODE* Node = (T_LIST_NODE*)Message;
    list_delete(Node, Node->prev, Node->next);
    MsgQueue->ListCount--;
    return;
}

/** 
 * @brief               Judge if the Message Queue is empty
 * @param[in,out]       MsgQueue        Message Queue Management handler
 * @retval              true            Message Queue is empty
 * @retval              false           Message Queue is not empty
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @hidecallergraph
 */
extern bool MQC_MsgQueue_empty( S_MQC_MSG_QUEUE* MsgQueue )
{
    return list_empty((&(MsgQueue->MsgList)));
}

/** 
 * @brief               Message Queue process iterator
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           SysTimeCount            System timer count with millisecond
 * @param[in]           WriteFuncCB             Message Send callback function for Message Queue process iterator
 * @param[in]           TimeoutFuncCB           Message Response timeout callback function for Message Queue process iterator
 * @param[in]           UsrData                 User Data used for callback function
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
extern void  MQC_MsgQueue_process(S_MQC_MSG_QUEUE* MsgQueue, uint32_t SysTimeCount, 
                                    void (*WriteFuncCB)(S_MQC_MSG_CTX*  Message, void* UserCtx), 
                                    void (*TimeoutFuncCB)(S_MQC_MSG_CTX*  Message, void* UserCtx), 
                                    void* UsrData)
{
    uint32_t            PassTime    =   0;
    T_LIST_NODE*        Node        =   NULL;
    T_LIST_NODE*        TmpNode     =   NULL;
    S_MQC_MSG_CTX*      Message     =   NULL;
    
    /* check the expire time */
    PassTime = prvCheckPassTime(MsgQueue->MnotonicTime, SysTimeCount);
    MsgQueue->MnotonicTime = SysTimeCount;
    
    /* Search the iterat the message in queue */
    list_for_each((&(MsgQueue->MsgList)), Node, TmpNode)
    {
        Message = (S_MQC_MSG_CTX*)Node;
        /* Check timeout */
        if(PassTime >= Message->ExpireTime)
        {
            Message->SendCount = (0 == Message->SendCount)?0:Message->SendCount-1;
            if(Message->SendCount)
            {
                /* timeout recount */
                Message->ExpireTime = Message->Timeout;
                /* ReSend the message */
                WriteFuncCB(Message, UsrData);
            }
            else
            {
                /* Timeout and should call the callback function */
                list_delete(Node, Node->prev, Node->next);
                MsgQueue->ListCount--;
                list_insert_tail( Node,  &(MsgQueue->ExecMsgList));
            }
        }
        else
        {
            /* Update expire time count */
            Message->ExpireTime = Message->ExpireTime - PassTime;
        }
    }
    
    /* Do the callback function */
    list_for_each((&(MsgQueue->ExecMsgList)), Node, TmpNode)
    {
        list_delete(Node, Node->prev, Node->next);
        Message = (S_MQC_MSG_CTX*)Node;
        TimeoutFuncCB(Message, UsrData);
    }
    return;
}

/** 
 * @brief               Message Queue iterator
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           ForeachFuncCB           Iterator callback function
 * @param[in]           UsrData                 User Data used for callback function
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
extern void MQC_MsgQueue_foreach(S_MQC_MSG_QUEUE* MsgQueue, void (*ForeachFuncCB)(S_MQC_MSG_CTX*  Message, void* UserCtx) , void* UsrData)
{
    T_LIST_NODE*        Node        =   NULL;
    T_LIST_NODE*        TmpNode     =   NULL;
    S_MQC_MSG_CTX*      Message     =   NULL;
    
    /* Do the callback function */
    list_for_each((&(MsgQueue->MsgList)), Node, TmpNode)
    {
        Message = (S_MQC_MSG_CTX*)Node;
        ForeachFuncCB(Message, UsrData);
    }
    return;
}
