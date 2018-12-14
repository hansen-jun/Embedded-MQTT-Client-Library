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
 * @file        MQC_api.c
 * @brief       MQTT Client Library API
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/06/14 : zhaozhenge@outlook.com 
 *                  -# New
 */

/**************************************************************
**  Include
**************************************************************/

#include "../inc/MQC_core.h"

/**************************************************************
**  Interface
**************************************************************/

/** 
 * @brief               Start a MQTT Session
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           SystimeCount            System timer count with milisecond
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @note                MQC_Start must be called before all of the MQC API called
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/14
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Start(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if(!MQCHandler->MallocFunc || !MQCHandler->FreeFunc)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if( (!MQCHandler->LockFunc && MQCHandler->UnlockFunc) 
       || (MQCHandler->LockFunc && !MQCHandler->UnlockFunc) )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if(!MQCHandler->ReadFuncCB || !MQCHandler->WriteFuncCB)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if(MQCHandler->WillMessage.Enable)
    {
        if( !MQCHandler->WillMessage.Message.Topic.Data || !MQCHandler->WillMessage.Message.Topic.Length )
        {
            return D_MQC_RET_BAD_INPUT_DATA;
        }
        /* Will Message has only two byte message length */
        if(MQCHandler->WillMessage.Message.Content && (65535 < MQCHandler->WillMessage.Message.Length) )
        {
            return D_MQC_RET_BAD_INPUT_DATA;
        }
    }
    else
    {
        MQCHandler->WillMessage.Retain = false;
        MQCHandler->WillMessage.QoS = E_MQC_QOS_0;
    }
    /* MQTT V3.1.1 allow client to use a 0 byte id  */
    if( (!MQCHandler->ClientId.Data && MQCHandler->ClientId.Length) 
       || (MQCHandler->ClientId.Data && !MQCHandler->ClientId.Length) )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* If client use a 0 byte id, must set the CleanSession with true */
    if( (!MQCHandler->ClientId.Length) && (!MQCHandler->CleanSession) )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Start */
    return MQC_CoreStart(MQCHandler, SystimeCount);
}

/** 
 * @brief               Stop the MQTT Session
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @note                This API can only be called after a MQTT client handler is started
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/14
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Stop(S_MQC_SESSION_HANDLE* MQCHandler)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Stop */
    return MQC_CoreStop(MQCHandler);
}

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
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/16
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Open(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Open */
    return MQC_CoreOpen(MQCHandler, Timeout);
}

/** 
 * @brief               Close a MQTT connection with DISCONNECT Message
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Close(S_MQC_SESSION_HANDLE* MQCHandler)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Close */
    return MQC_CoreClose(MQCHandler);
}

/** 
 * @brief               Reset a MQTT connection with CONNECT Message (CleanSession Flag On)
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           Timeout                 Timeout with millisecond for cannot receive server response
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Reset(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Reset */
    return MQC_CoreReset(MQCHandler, Timeout);
}
 
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
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/19
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Subscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_QOS_LEVEL* QoSList, uint32_t ListNum, F_SUBSCRIBE_RES_CBFUNC ResultFuncCB)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if( !TopicFilterList || !QoSList || !ListNum )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Subscribe */
    return MQC_CoreSubscribe( MQCHandler, TopicFilterList, QoSList, ListNum, ResultFuncCB);
}

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
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Unsubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, F_UNSUBSCRIBE_RES_CBFUNC ResultFuncCB)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if( !TopicFilterList || !ListNum )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core UnSubscribe */
    return MQC_CoreUnsubscribe( MQCHandler, TopicFilterList, ListNum, ResultFuncCB );
}

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
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/06/19
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Publish(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, E_MQC_QOS_LEVEL QoS, bool Retain, F_PUBLISH_RES_CBFUNC ResultFuncCB)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if( !Message )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if( (!Message->Topic.Data) || (!Message->Topic.Length) )
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Publish */
    return MQC_CorePublish( MQCHandler, Message, QoS, Retain, ResultFuncCB );
}

/** 
 * @brief               Send a PINFREQ Message via MQTT Session.
 * @param[in,out]       MQCHandler              MQTT client handler
 * @retval              D_MQC_RET_OK
 * @retval              D_MQC_RET_BAD_SEQUEUE
 * @retval              D_MQC_RET_UNEXPECTED_ERROR
 * @retval              D_MQC_RET_NO_MEMORY
 * @retval              D_MQC_RET_CALLBACK_ERROR
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @note                Ping Response Message will notify user by ReadFuncCB in handler
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Ping(S_MQC_SESSION_HANDLE* MQCHandler)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Ping */
    return MQC_CorePing(MQCHandler);
}

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
 * @retval              D_MQC_RET_BAD_INPUT_DATA
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN int32_t MQC_Read(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t* Data, size_t Size)
{
    /* Check the input parameter */
    if(!MQCHandler)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    if(!Data || !Size)
    {
        return D_MQC_RET_BAD_INPUT_DATA;
    }
    /* Core Read */
    return MQC_CoreRead(MQCHandler, Data, Size);
}

/** 
 * @brief               This function will be called periodically to keep the MQTT session alive
 * @param[in,out]       MQCHandler              MQTT client handler
 * @param[in]           SystimeCount            System timer count with millisecond
 * @return              None
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/04
 * @callgraph
 * @hidecallergraph
 */
MQC_EXTERN void MQC_Continue(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount)
{
    /* Check the input parameter */
    if(MQCHandler)
    {
        MQC_CoreContinue(MQCHandler, SystimeCount);
    }
    return;
}
