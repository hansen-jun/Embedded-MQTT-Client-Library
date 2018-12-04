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
 * @file        MQC_api.h
 * @brief       MQTT Client Library API Header
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/06/13 : zhaozhenge@outlook.com 
 *                  -# New
 */

#ifndef _MQC_API_H_
#define _MQC_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************
**  Include
**************************************************************/

#include "../src/inc/MQC_def.h"

/**************************************************************
**  Symbol
**************************************************************/

/**
 * @addtogroup  mbed_mqtt_library
 * @brief       MQTT Client Library API
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01
 *              - 2018/11/22 : zhaozhenge@outlook.com
 *                  -# New
 * @{ 
 */

/** @def MQC Error Code */
#define D_MQC_RET_NO_NOTIFY             (1)         /*!< The request/command has already been done */
#define D_MQC_RET_OK                    (0)         /*!< Success */
#define D_MQC_RET_UNEXPECTED_ERROR      (-1)        /*!< Unexpected error */
#define D_MQC_RET_BAD_INPUT_DATA        (-2)        /*!< Bad input parameters for funcion */
#define D_MQC_RET_NO_MEMORY             (-3)        /*!< No enough memory to use */
#define D_MQC_RET_BAD_SEQUEUE           (-4)        /*!< Bad sequeue for MQTT protocol */
#define D_MQC_RET_BAD_FORMAT            (-5)        /*!< Bad Message format for MQTT protocol */
#define D_MQC_RET_CALLBACK_ERROR        (-6)        /*!< Callback function error */

#define D_MQC_OPEN_SUCCESS_CODE         (0)         /*!< Connect Return Code for open success */

/** 
 * @brief       Message Type define of MQTT
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/13
 */
typedef enum _E_MQC_MSG_TYPE
{
    E_MQC_MSG_RESERVED_MIN  =   0x00,               /*!< Reserved, Do not used by User */
    E_MQC_MSG_CONNECT,                              /*!< Connect Message */
    E_MQC_MSG_CONNACK,                              /*!< Response for the Connect Message */
    E_MQC_MSG_PUBLISH,                              /*!< Publish Message */
    E_MQC_MSG_PUBACK,                               /*!< Response for the Publish Message in QoS1 */
    E_MQC_MSG_PUBREC,                               /*!< Response for the Publish Message in QoS2 */
    E_MQC_MSG_PUBREL,                               /*!< Response for the PUBREC Message */
    E_MQC_MSG_PUBCOMP,                              /*!< Response for the PUBREL Message */
    E_MQC_MSG_SUBSCRIBE,                            /*!< Subscribe Message */
    E_MQC_MSG_SUBACK,                               /*!< Response for the Subscribe Message */
    E_MQC_MSG_UNSUBSCRIBE,                          /*!< UnSubscribe Message */
    E_MQC_MSG_UNSUBACK,                             /*!< Response for the UnSubscribe Message */
    E_MQC_MSG_PINGREQ,                              /*!< Ping Message */
    E_MQC_MSG_PINGRESP,                             /*!< Response for the Ping Message */
    E_MQC_MSG_DISCONNECT,                           /*!< Disconnect Message */
    E_MQC_MSG_RESERVED_MAX                          /*!< Reserved, Do not used by User */
}E_MQC_MSG_TYPE;

/** 
 * @brief       QoS Level define of MQTT
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/16
 */
typedef enum _E_MQC_QOS_LEVEL
{
    E_MQC_QOS_0 = 0x00,                             /*!< QoS0 : at most once */
    E_MQC_QOS_1,                                    /*!< QoS1 : at least once */
    E_MQC_QOS_2                                     /*!< QoS2 : exactly once */
}E_MQC_QOS_LEVEL;

/** 
 * @brief       Return Code define of MQTT (for SUBACK message)
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/19
 */
typedef enum _E_MQC_RETURN_CODE
{
    E_MQC_CODE_QOS0 = E_MQC_QOS_0,                  /*!< QoS0 */
    E_MQC_CODE_QOS1 = E_MQC_QOS_1,                  /*!< QoS1 */
    E_MQC_CODE_QOS2 = E_MQC_QOS_2,                  /*!< QoS2 */
    E_MQC_CODE_FAIL = 0x80                          /*!< Failure */
}E_MQC_RETURN_CODE;

/** 
 * @brief       Operational behavior result of MQTT
 * @author      zhaozhenge@outlook.com
 * @date        2018/11/22
 */
typedef enum _E_MQC_BEHAVIOR_RESULT
{
    E_MQC_BEHAVIOR_COMPLETE = 0x00,                 /*!< Operational behavior complete */
    E_MQC_BEHAVIOR_TIMEOUT,                         /*!< Operational behavior timeout */
    E_MQC_BEHAVIOR_CANCEL,                          /*!< Operational behavior cancel */
    E_MQC_BEHAVIOR_NEEDRESET                        /*!< Need reset the MQTT session */
}E_MQC_BEHAVIOR_RESULT;

/**
 * @} 
 */

/**************************************************************
**  Structure
**************************************************************/

/**
 * @addtogroup  mbed_mqtt_library
 * @{ 
 */

/**
 * @brief       Standard UTF-8 Encode Data for MQTT format
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/16
 */
typedef struct _S_MQC_UTF8_DATA
{
    uint8_t*                Data;                   /*!< UTF-8 Data */
    uint16_t                Length;                 /*!< Data Length */
}S_MQC_UTF8_DATA;

/**
 * @brief       MQTT Common Message Define
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/16
 */
typedef struct _S_MQC_MESSAGE_INFO
{
    S_MQC_UTF8_DATA         Topic;                  /*!< Message Topic */
    uint8_t*                Content;                /*!< Message Content */
    uint32_t                Length;                 /*!< Message Length */
}S_MQC_MESSAGE_INFO;

/**
 * @brief       Will Message Setting for MQTT Session
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/16
 */
typedef struct _S_MQC_WILL_INFO
{
    bool                    Enable;                 /*!< If Enable the Will Message */
    E_MQC_QOS_LEVEL         QoS;                    /*!< Message QoS Level */
    bool                    Retain;                 /*!< If Message need Retain */
    S_MQC_MESSAGE_INFO      Message;                /*!< Will Message */
}S_MQC_WILL_INFO;

/**
 * @brief       Authorition Setting for MQTT Session
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/16
 */
typedef struct _S_MQC_AUTH_INFO
{
    bool                    UsernameEnable;         /*!< If Set the UserName */
    S_MQC_UTF8_DATA         Username;               /*!< UserName Content */
    bool                    PasswordEnable;         /*!< If Set the Password */
    S_MQC_UTF8_DATA         Password;               /*!< Password Content */
}S_MQC_AUTH_INFO;

/**
 * @brief       MQTT session handler
 * @author      zhaozhenge@outlook.com
 * @date        2018/06/16
 */
typedef struct _S_MQC_SESSION_HANDLE
{
    S_MQC_SESSION_CTX       SessionCtx;
    /*!< MQTT session data (User not use) */
    
    void*                   UsrCtx;
    /*!< Callback context for user */
    
    bool                    CleanSession;
    /*!< If Enable Clean Session */
    
    S_MQC_WILL_INFO         WillMessage;
    /*!< Will Information of MQTT Session */
    
    S_MQC_AUTH_INFO         Authorition;
    /*!< Authorition Information of MQTT Session */
    
    S_MQC_UTF8_DATA         ClientId;
    /*!< Client Id */
    
    uint16_t                KeepAliveInterval;
    /*!< Keep Alive Message Send Interval (unit:second, 0 means do not keep alive) */
    
    uint16_t                MessageRetryInterval;
    /*!< Message send retry Interval (unit:second, 0 means keep the message until send succuessfully or session deleted ) */
    
    uint32_t                MessageRetryCount;
    /*!< Keep Alive Message Send Interval ( 0 means do not retry ) */
    
    void*                   (*MallocFunc)(size_t);
    /*!< malloc callback function */
    
    void                    (*FreeFunc)(void* );
    /*!< free callback function */
    
    void                    (*LockFunc)(void* Ctx);
    /*!< Lock callback function */
    
    void                    (*UnlockFunc)(void* Ctx);
    /*!< Unlock callback function */
    
    int32_t                 (*WriteFuncCB)(void* Ctx, const uint8_t* Data, size_t Size);
    /*!< Message data write callback function */
    
    int32_t                 (*ReadFuncCB)(void* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info);
    /*!< Message data read callback function */
    
    int32_t                 (*OpenResetFuncCB)(void* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode);
    /*!< CONNECT result callback function */
    
}S_MQC_SESSION_HANDLE;

/**
 * @} 
 */

/**************************************************************
**  Callback function
**************************************************************/

/**
 * @brief               Subscribe behavior finish callback function
 * @param[in]           Result              The execution of the behavior
 * @param[in]           TopicFilterList     The list of the topic filter that want to be subscribed 
 * @param[in]           SrvRetCodeList      The list of the subscribe return code responsed by server(if no response it will be NULL)
 * @param[in]           ListNum             The count of the element in list
 */
typedef int32_t (*F_SUBSCRIBE_RES_CBFUNC)( E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum );

/**
 * @brief               UnSubscribe behavior finish callback function
 * @param[in]           Result              The execution of the behavior
 * @param[in]           TopicFilterList     The list of the topic filter that want to be unsubscribed 
 * @param[in]           ListNum             The count of the element in list
 */
typedef int32_t (*F_UNSUBSCRIBE_RES_CBFUNC)(E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum);

/**
 * @brief               Publish behavior finish callback function
 * @param[in]           Result              The execution of the behavior
 * @param[in]           Message             The message that want to be published 
 */
typedef int32_t (*F_PUBLISH_RES_CBFUNC)(E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message);

/**************************************************************
**  Interface
**************************************************************/

/**
 * @addtogroup  mbed_mqtt_library
 * @{ 
 */

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
MQC_EXTERN int32_t MQC_Start(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount);

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
MQC_EXTERN int32_t MQC_Stop(S_MQC_SESSION_HANDLE* MQCHandler);

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
MQC_EXTERN int32_t MQC_Open(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout);

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
MQC_EXTERN int32_t MQC_Close(S_MQC_SESSION_HANDLE* MQCHandler);

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
MQC_EXTERN int32_t MQC_Reset(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t Timeout);

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
MQC_EXTERN int32_t MQC_Subscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_QOS_LEVEL* QoSList, uint32_t ListNum, F_SUBSCRIBE_RES_CBFUNC ResultFuncCB);

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
MQC_EXTERN int32_t MQC_Unsubscribe(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, F_UNSUBSCRIBE_RES_CBFUNC ResultFuncCB);

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
MQC_EXTERN int32_t MQC_Publish(S_MQC_SESSION_HANDLE* MQCHandler, S_MQC_MESSAGE_INFO* Message, E_MQC_QOS_LEVEL QoS, bool Retain, F_PUBLISH_RES_CBFUNC ResultFuncCB);

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
MQC_EXTERN int32_t MQC_Ping(S_MQC_SESSION_HANDLE* MQCHandler);

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
MQC_EXTERN int32_t MQC_Read(S_MQC_SESSION_HANDLE* MQCHandler, uint8_t* Data, size_t Size);

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
MQC_EXTERN void MQC_Continue(S_MQC_SESSION_HANDLE* MQCHandler, uint32_t SystimeCount);

/**
 * @} 
 */

#ifdef __cplusplus
}
#endif

#endif /* _MQC_API_H_ */
