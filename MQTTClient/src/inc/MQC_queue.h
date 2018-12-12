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
 * @file        MQC_queue.h
 * @brief       MQTT Client Libary Message Queue Management Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/11/23 : zhaozhenge@outlook.com 
 *                  -# New
 * @version     00.00.02 
 *              - 2018/12/12 : zhaozhenge@outlook.com 
 *                  -# Modify some comment
 */

#ifndef _MQC_QUEUE_H_
#define _MQC_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Include
**************************************************************/

#include "MQC_api.h"

/**************************************************************
**  Structure
**************************************************************/

/**
 * @brief      MQTT SUBSCRIBE Message Extra information  
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/23
 */
typedef struct _S_MQC_MSG_SUB_DATA
{
    F_SUBSCRIBE_RES_CBFUNC      ResultFuncCB;           /*!< Result callback function */
    uint32_t                    ListNum;                /*!< List Count of the Topic Filter want to subscribe */
    S_MQC_UTF8_DATA             TopicFilterList[];      /*!< List of the Topic Filter want to subscribe */
}S_MQC_MSG_SUB_DATA;

/**
 * @brief      MQTT UNSUBSCRIBE Message Extra information  
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/23
 */
typedef struct _S_MQC_MSG_UNSUB_DATA
{
    F_UNSUBSCRIBE_RES_CBFUNC    ResultFuncCB;           /*!< Result callback function */
    uint32_t                    ListNum;                /*!< List Count of the Topic Filter want to unsubscribe */
    S_MQC_UTF8_DATA             TopicFilterList[];      /*!< List of the Topic Filter want to unsubscribe */
}S_MQC_MSG_UNSUB_DATA;

/**
 * @brief      MQTT PUBLISH Message Extra information  
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/23
 */
typedef struct _S_MQC_MSG_PUB_DATA
{
    F_PUBLISH_RES_CBFUNC        ResultFuncCB;           /*!< Result callback function */
    S_MQC_MESSAGE_INFO          Message;                /*!< Message want to publish */
}S_MQC_MSG_PUB_DATA;


/**
 * @brief      MQTT Message Extra information  
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/23
 */
typedef union _U_MQC_MSG_EXT_DATA
{
    S_MQC_MSG_SUB_DATA          Subscribe;              /*!< SUBSCRIBE Message */
    S_MQC_MSG_UNSUB_DATA        UnSubscribe;            /*!< UNSUBSCRIBE Message */
    S_MQC_MSG_PUB_DATA          Publish;                /*!< PUBLISH Message */
}U_MQC_MSG_EXT_DATA;

/** 
 * @brief      Message stored in the send Queue 
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/23
 */
typedef struct _S_MQC_MSG_CTX
{
    T_LIST_NODE                 Node;                   /*!< List Node information */
    uint32_t                    SendCount;              /*!< Send Count */
    uint32_t                    ExpireTime;             /*!< Timeout Expire time Count */
    uint32_t                    Timeout;                /*!< Timeout */ 
    uint32_t                    MsgLength;              /*!< Message Length */
    uint8_t*                    MsgData;                /*!< Message Data */
    uint16_t                    PacketIdentifier;       /*!< Packet Identifier */
    U_MQC_MSG_EXT_DATA          ExtData;                /*!< Message Extra Infomation */
}S_MQC_MSG_CTX;

/**************************************************************
**  Interface
**************************************************************/

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
extern int32_t MQC_MsgQueue_create(S_MQC_MSG_QUEUE* MsgQueue, uint32_t SysTimeCount);
   
/** 
 * @brief               Delete a Message Queue Managment handler
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @retval              D_MQC_RET_OK
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/30
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_MsgQueue_delete(S_MQC_MSG_QUEUE* MsgQueue);

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
extern S_MQC_MSG_CTX* MQC_MsgQueue_search(S_MQC_MSG_QUEUE* MsgQueue, uint16_t PacketIdentifier, E_MQC_MSG_TYPE MsgType);

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
extern S_MQC_MSG_CTX* MQC_MsgQueue_pop(S_MQC_MSG_QUEUE* MsgQueue);

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
extern S_MQC_MSG_CTX* MQC_MsgQueue_push( S_MQC_MSG_QUEUE* MsgQueue, S_MQC_MSG_CTX* Message );

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
extern void MQC_MsgQueue_slice(S_MQC_MSG_QUEUE* MsgQueue, S_MQC_MSG_CTX* Message);

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
extern bool MQC_MsgQueue_empty( S_MQC_MSG_QUEUE* MsgQueue );

/** 
 * @brief               Message Queue process iterator
 * @param[in,out]       MsgQueue                Message Queue Management handler
 * @param[in]           SysTimeCount            System timer count with millisecond
 * @param[in]           WriteFuncCB             Message Send callback function for Message Queue process iterator
 * @param[in]           TimeoutFuncCB           Message Response timeout callback function for Message Queue process iterator
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 * @callgraph
 * @callergraph
 */
extern void  MQC_MsgQueue_process(S_MQC_MSG_QUEUE* MsgQueue, uint32_t SysTimeCount, 
                                    void (*WriteFuncCB)(S_MQC_MSG_CTX*  Message, void* UserCtx), 
                                    void (*TimeoutFuncCB)(S_MQC_MSG_CTX*  Message, void* UserCtx), 
                                    void* UsrData);

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
extern void MQC_MsgQueue_foreach(S_MQC_MSG_QUEUE* MsgQueue, void (*ForeachFuncCB)(S_MQC_MSG_CTX*  Message, void* UserCtx) , void* UsrData);

#ifdef __cplusplus
}
#endif

#endif /* _MQC_QUEUE_H_ */
