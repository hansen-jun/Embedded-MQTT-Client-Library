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
 * @file        MQC_def.h
 * @brief       MQTT Client Library Define Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/06/13 : zhaozhenge@outlook.com 
 *                  -# New
 */

#ifndef _MQC_DEFINE_H_
#define _MQC_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Include
**************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "MQC_config.h"
#include "../../../CommonLib/CLIB_api.h"
#if defined (MQC_NET_API)
#include "../inc/MQC_net.h"
#endif /* MQC_NET_API */

/**************************************************************
**  Symbol
**************************************************************/

/**
 * @brief      MQTT session status
 * @author     zhaozhenge@outlook.com
 * @date       2018/11/22
 */
typedef enum _E_MQC_STATUS
{
    E_MQC_STATUS_OPEN = 1,                      /*!< MQTT Session Created */
    E_MQC_STATUS_CONNECT,                       /*!< Connecting to server */
    E_MQC_STATUS_WORK,                          /*!< Normal (connected with server) */
    E_MQC_STATUS_RESET,                         /*!< Session Reset */
    E_MQC_STATUS_INVALID = 0xFF                 /*!< MQTT Session Deleted */
}E_MQC_STATUS;

/**************************************************************
**  Struct
**************************************************************/

/**
 * @brief      MQTT Message Queue
 * @author     zhaozhenge@outlook.com
 * @date       2018/06/19
 */
typedef struct _S_MQC_MSG_QUEUE
{
    T_LIST_NODE             MsgList;            /*!< Message entry list */
    T_LIST_NODE             ExecMsgList;        /*!< Execute entry list */
    uint32_t                ListCount;          /*!< Message number of the Queue */
    uint32_t                MnotonicTime;       /*!< System timer count */
    uint16_t                PacketIdentifier;   /*!< Packet Identifier */
}S_MQC_MSG_QUEUE;

/**
 * @brief      MQTT session manage context
 * @author     zhaozhenge@outlook.com
 * @date       2018/06/19
 */
typedef struct _S_MQC_SESSION_CTX
{
    E_MQC_STATUS            Status;             /*!< Session status */
    S_MQC_MSG_QUEUE         MessageQueue;       /*!< Message Queue Management Context */
    uint32_t                TimeoutCount;       /*!< Count the timeout */
    uint32_t                SystimeCount;       /*!< System timer count with millisecond */
    uint8_t*                RecvData;           /*!< The Data recieved already */
    uint32_t                RecvDataSize;       /*!< The size of Data recieved already */
    uint32_t                TotalRecvDataSize;  /*!< The total size of Data want to recieve */
    uint32_t                HeaderDataSize;     /*!< The data size of Message header (Fixed header + Remaining length)  */
}S_MQC_SESSION_CTX;

#ifdef __cplusplus
}
#endif

#endif /* _MQC_DEFINE_H_ */
