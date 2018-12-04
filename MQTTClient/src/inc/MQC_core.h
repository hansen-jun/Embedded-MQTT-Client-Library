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
 * @file        MQC_core.h
 * @brief       MQTT Client Libary Core Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/06/14 : zhaozhenge@outlook.com 
 *                  -# New
 */

#ifndef _MQC_CORE_H_
#define _MQC_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Include
**************************************************************/

#include "../../interface/MQC_api.h"

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               Start a MQTT Session
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           SystimeCount            System timer count with milisecond
 * @retval              D_MQC_RET_OK
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/15
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreStart(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount);

/** 
 * @brief               Stop the MQTT Session
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/15
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreStop(S_MQC_SESSION_HANDLE* MQCHandler);

/** 
 * @brief               Open a MQTT connection with CONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Timeout                 Timeout with millisecond for cannot receive server response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_NOTIFY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/16
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreOpen(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout);

/** 
 * @brief               Close a MQTT connection with DISCONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreClose(S_MQC_SESSION_HANDLE* MQCHandler);

/** 
 * @brief               Reset a MQTT connection with CONNECT Message (CleanSession Flag On)
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Timeout                 Timeout with millisecond for cannot receive server response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreReset(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout);

/** 
 * @brief               Subscribe topic via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           QoSList                 Topic Filter's QoS List
 * @param[in]           ListNum                 Count of the topic filter list
 * @param[in]           ResultFuncCB            Callback function will be called to notify if received Server Response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/19
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreSubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_QOS_LEVEL* QoSList, uint32_t ListNum, F_SUBSCRIBE_RES_CBFUNC ResultFuncCB);

/** 
 * @brief               Cancel a subscribed topic via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           TopicFilterList         Topic Filter List
 * @param[in]           ListNum                 Count of the topic filter list
 * @param[in]           ResultFuncCB             Callback function will be called to notify if received Server Response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreUnsubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, F_UNSUBSCRIBE_RES_CBFUNC ResultFuncCB);

/** 
 * @brief               Publish message via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Message                 MQTT Message
 * @param[in]           QoS                     Message's QoS
 * @param[in]           Retain                  Message's Retain Flag
 * @param[in]           ResultFuncCB            Callback function will be called to notify if received Server Response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/19
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CorePublish(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, E_MQC_QOS_LEVEL QoS, bool Retain, F_PUBLISH_RES_CBFUNC ResultFuncCB);

/** 
 * @brief               Send a PINFREQ Message via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CorePing(S_MQC_SESSION_HANDLE* MQCHandler);

/** 
 * @brief               Set the data read from network to the MQTT Sesstion.
 *                      The data will be parsed as MQTT protocol.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Data                    Data read from the network
 * @param[in]           Size                    Size of the Data
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_FORMAT
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern int32_t MQC_CoreRead(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t* Data, size_t Size);

/** 
 * @brief               This function will be called periodically to keep the MQTT session alive
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           SystimeCount            System timer count with millisecond
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @callergraph
 */
extern void MQC_CoreContinue(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount);

#ifdef __cplusplus
}
#endif

#endif /* _MQC_CORE_H_ */
