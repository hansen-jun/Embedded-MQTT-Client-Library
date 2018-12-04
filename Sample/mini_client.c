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
 * @example     mini_client.c
 * @brief       A Simple Minimal MQTT Client.
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/12/03 : zhaozhenge@outlook.com 
 *                  -# New
 */
 
#if !defined(PLATFORM_LINUX) && !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_OTHER)  
#error Should indicate a platform for your target (PLATFORM_LINUX or PLATFORM_WINDOWS or PLATFORM_OTHER)
#endif 

/**************************************************************
**  Include
**************************************************************/

#if defined(PLATFORM_LINUX)
#include "../Platform/Linux/wrapper.h"
#elif defined(PLATFORM_WINDOWS)
#include "../Platform/Windows/wrapper.h"
#else
#include "../Platform/Lwip/wrapper.h"
#endif

#include "MQC_api.h"

/**************************************************************
**  Symbol
**************************************************************/

#define D_MQC_MQTT_HOST      "test.mosquitto.org"   /*!< MQTT test Server hostname */
#define D_MQC_MQTT_PORT      (1883)                 /*!< MQTT test Server port */

/**************************************************************
**  Structure
**************************************************************/

/**
 * @brief      Custom User Data
 * @author     zhaozhenge@outlook.com
 * @date       2018/12/03
 */
typedef struct _S_USER_DATA
{
    S_MQC_SESSION_HANDLE*   Handler;
    S_PLATFORM_DATA         Platform;
    bool                    Running;
}S_USER_DATA;

/**************************************************************
**  Global Param
**************************************************************/

static S_MQC_SESSION_HANDLE     MQCHandler;
static S_USER_DATA              UsrData;
static uint8_t const*           PublishTopic    =   (uint8_t*)"temp/random";
static uint8_t const*           ClientId        =   (uint8_t*)"b1bfee9163f44141b56eca25af6bc27c";

/**************************************************************
**  Function
**************************************************************/

int32_t SubscribeResponseFunc( E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum )
{
    int i   =   0;
    do
    {
        if(E_MQC_BEHAVIOR_COMPLETE == Result)
        {
            D_MQC_PRINT( " Topic Subscribed\n ");
            for(i = 0; i < ListNum; i++)
            {
                D_MQC_PRINT( " Topic    : %.*s\n ", TopicFilterList[i].Length, TopicFilterList[i].Data);
                D_MQC_PRINT( " QoS      : %d\n ", SrvRetCodeList[i]);
            }
            D_MQC_PRINT("\n\n");
        }
        else if(E_MQC_BEHAVIOR_TIMEOUT == Result)
        {
            D_MQC_PRINT( " Topic Subscribe timeout\n\n ");
        }
        else if(E_MQC_BEHAVIOR_CANCEL == Result)
        {
            D_MQC_PRINT( " Topic Subscribe Cancel\n\n ");
        }
        else
        {
            D_MQC_PRINT( " failed\n  ! Unknown Result %d\n\n", Result );
        }
    }while(0);
    
    return 0;
}

/** 
 * @brief               Message publish callback function
 * @param[in]           Result              Publish Message result
 * @param[in]           Message             Information of the Message
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
int32_t PublishResponseFunc(E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message)
{
    do
    {
        if(E_MQC_BEHAVIOR_COMPLETE == Result)
        {
            D_MQC_PRINT( " Message Published\n ");
            D_MQC_PRINT( " Topic    : %.*s\n ", Message->Topic.Length, Message->Topic.Data);
            if(Message->Content)
            {
                D_MQC_PRINT( " Message  : %.*s\n ", Message->Length, Message->Content);
            }
            else
            {
                D_MQC_PRINT(" Message  : \n ");
            }
            D_MQC_PRINT("\n\n");
        }
        else if(E_MQC_BEHAVIOR_TIMEOUT == Result)
        {
            D_MQC_PRINT( " Message Published timeout\n\n ");
        }
        else if(E_MQC_BEHAVIOR_CANCEL == Result)
        {
            D_MQC_PRINT( " Message Published Cancel\n\n ");
        }
        else
        {
            D_MQC_PRINT( " failed\n  ! Unknown Result %d\n\n", Result );
        }
    }while(0);
    
    return 0;
}

/** 
 * @brief               TCP/IP Data Send callback function
 * @param[in,out]       Ctx                 User Context
 * @param[in]           Data                Data want to write via network
 * @param[in]           Size                Size of the Data
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/11/08
 */
int32_t WriteTcp_callback(void* Ctx, const uint8_t* Data, size_t Size)
{
    S_USER_DATA*    CustomData  =   (S_USER_DATA*)Ctx;
    uint8_t*        DataStr     =   (uint8_t*)Data;
    int32_t         Err         =   0;
    
    while(Size)
    {
        Err = tcpwrite_wrapper( &(CustomData->Platform), DataStr, Size);
        if(0 > Err)
        {
            D_MQC_PRINT( " failed\n  ! send() returned %d\n\n", Err );
            return (-1);
        }
        Size    =   Size - Err;
        DataStr =   DataStr + Err;
    }
    
    return 0;
}

/** 
 * @brief               Open/Reset callback function
 * @param[in,out]       Ctx                 User Context for callback
 * @param[in]           Result              Result of the Connect Behavior
 * @param[in]           SrvResCode          Result of the server response
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
int32_t OpenResetNotify_callback(void* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode)
{
    S_USER_DATA*        CustomData  =   (S_USER_DATA*)Ctx;
    int32_t             Ret         =   0;
    int32_t             Err         =   D_MQC_RET_OK;
    S_MQC_MESSAGE_INFO  Message;
    S_MQC_UTF8_DATA     TopicFilter;
    E_MQC_QOS_LEVEL     QoS;
    
    do
    {
        if( (E_MQC_BEHAVIOR_TIMEOUT == Result) || (E_MQC_BEHAVIOR_CANCEL == Result) )
        {
            D_MQC_PRINT( " failed\n  ! Connect timeout\n\n" );
            CustomData->Running = false;
            break;
        }
        
        if(E_MQC_BEHAVIOR_NEEDRESET == Result)
        {
            D_MQC_PRINT( " failed\n  ! Session need reset\n\n");
            /* Session close */
            Err = MQC_Close(CustomData->Handler);
            if(D_MQC_RET_OK != Err)
            {
                D_MQC_PRINT( " failed\n  ! MQC_Reset() returned %d\n\n", Err );
                CustomData->Running = false;
                Ret = -1;
                break;
            }
            /* Network close */
            network_close_wrapper(&(UsrData.Platform));
            if( network_open_wrapper(&(UsrData.Platform)) )
            {
                D_MQC_PRINT( " failed\n  ! network_open_wrapper() \n\n");
                UsrData.Running = false;
                Ret = -1;
                break;
            }
            Err = MQC_Reset(CustomData->Handler, 5000);
            if(D_MQC_RET_OK != Err)
            {
                D_MQC_PRINT( " failed\n  ! MQC_Reset() returned %d\n\n", Err );
                CustomData->Running = false;
                Ret = -1;
            }
            break;
        }
        
        if(D_MQC_OPEN_SUCCESS_CODE != SrvResCode)
        {
            D_MQC_PRINT( " Session Connect Error %d\n\n", SrvResCode );
            CustomData->Running = false;
            break;
        }
        
        D_MQC_PRINT( " Session Connected \n");
        
        /* Subscribe Topic */
        TopicFilter.Length  =   strlen((char*)PublishTopic);
        TopicFilter.Data    =   (uint8_t*)PublishTopic;
        QoS                 =   E_MQC_QOS_2;
        Err = MQC_Subscribe(CustomData->Handler, &TopicFilter, &QoS, 1, SubscribeResponseFunc);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Publish() returned %d\n\n", Err );
            CustomData->Running = false;
            Ret = -1;
            break;
        }

        /* Publish Message */
        Message.Topic.Length    =   strlen((char*)PublishTopic);
        Message.Topic.Data      =   (uint8_t*)PublishTopic;
        Message.Length          =   strlen("50");
        Message.Content         =   (uint8_t*)"50";
        Err = MQC_Publish(CustomData->Handler, &Message, E_MQC_QOS_2, false, PublishResponseFunc);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Publish() returned %d\n\n", Err );
            CustomData->Running = false;
            Ret = -1;
            break;
        }

    }while(0);
    
    return Ret;
}

/** 
 * @brief               Message data read callback function
 * @param[in,out]       Ctx                 User Context for callback
 * @param[in]           Type                Message Type
 * @param[in]           Info                Information of the Message
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
int32_t ReadNotify_callback(void* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info)
{
    S_USER_DATA*    CustomData  =   (S_USER_DATA*)Ctx;
    
    do
    {
        if(E_MQC_MSG_PUBLISH == Type)
        {
            D_MQC_PRINT( " Receive Message by Server\n ");
            D_MQC_PRINT( " Topic    : %.*s\n ", Info->Topic.Length, Info->Topic.Data);
            if(Info->Content)
            {
                D_MQC_PRINT( " Message  : %.*s\n ", Info->Length, Info->Content);
            }
            else
            {
                D_MQC_PRINT(" Message  : \n ");
            }
            D_MQC_PRINT("\n\n");
        }
        else if(E_MQC_MSG_PINGRESP == Type)
        {
            D_MQC_PRINT( " Ping Response Received\n\n " );
        }
        else
        {
            D_MQC_PRINT( " failed\n  ! Unknown Message Type %d\n\n", Type );
            CustomData->Running = false;
        }
    }while(0);
    
    return 0;
}

/** 
 * @brief               main function
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
#if defined(PLATFORM_LINUX)
int main( int argc, char *argv[] )
#elif defined(PLATFORM_WINDOWS)
int main( int argc, char *argv[] )
#else
int MqttTask( void* Input )
#endif
{
    int32_t     Err         =   D_MQC_RET_OK;
    size_t      Size        =   0;
    uint8_t     Data[64];
    
    UsrData.Platform.DstAddress  =   (char*)D_MQC_MQTT_HOST;
    UsrData.Platform.DstPort     =   D_MQC_MQTT_PORT;
    
    if(wrapper_init(&(UsrData.Platform)))
    {
        wrapper_deinit(&(UsrData.Platform));
        return (-1);
    }
    
    if(network_open_wrapper(&(UsrData.Platform)))
    {
        wrapper_deinit(&(UsrData.Platform));
        return (-1);
    }
    
    UsrData.Handler =   &MQCHandler;
    UsrData.Running =   false;
    
    MQCHandler.UsrCtx                       =   &UsrData;
    MQCHandler.CleanSession                 =   true;
    MQCHandler.WillMessage.Enable           =   false;
    MQCHandler.Authorition.UsernameEnable   =   false;
    MQCHandler.Authorition.PasswordEnable   =   false;
    MQCHandler.ClientId.Data                =   (uint8_t*)ClientId;
    MQCHandler.ClientId.Length              =   strlen((char*)ClientId);
    MQCHandler.KeepAliveInterval            =   10;
    MQCHandler.MessageRetryInterval         =   5;
    MQCHandler.MessageRetryCount            =   3;
    MQCHandler.MallocFunc                   =   malloc_wrapper;
    MQCHandler.FreeFunc                     =   free_wrapper;
    MQCHandler.LockFunc                     =   NULL;
    MQCHandler.UnlockFunc                   =   NULL;
    MQCHandler.WriteFuncCB                  =   WriteTcp_callback;
    MQCHandler.ReadFuncCB                   =   ReadNotify_callback;
    MQCHandler.OpenResetFuncCB              =   OpenResetNotify_callback;

    Err = MQC_Start(&MQCHandler, systick_wrapper());
    if( D_MQC_RET_OK != Err)
    {
        D_MQC_PRINT( " failed\n  ! MQC_Start() returned %d\n\n", Err );
        wrapper_deinit(&(UsrData.Platform));
        return Err;
    }
    
    Err = MQC_Open(&MQCHandler, 5000);
    if( D_MQC_RET_OK != Err)
    {
        D_MQC_PRINT( " failed\n  ! MQC_Open() returned %d\n\n", Err );
        wrapper_deinit(&(UsrData.Platform));
        return Err;
    }
    
    UsrData.Running = true;
    
    while(UsrData.Running)
    {
        Err = tcpcheck_wrapper(&(UsrData.Platform), 100);
        if(0 > Err)
        {
            UsrData.Running = false;
            break;
        }
        else if(0 == Err)
        {
            MQC_Continue(&MQCHandler, systick_wrapper());
        }
        else
        {
            do
            {
                Err = tcpread_wrapper(&(UsrData.Platform), Data, sizeof(Data));
                if(0 >= Err)
                {
                    D_MQC_PRINT( " failed\n  ! tcpread_wrapper() returned %d\n\n", Err );
                    UsrData.Running = false;
                    break;
                }
                Size = Err;
                Err = MQC_Read(&MQCHandler, Data, Err);
                if(D_MQC_RET_OK != Err)
                {
                    D_MQC_PRINT( " failed\n  ! MQC_Read() returned %d\n\n", Err );
                    UsrData.Running = false;
                    break;
                }
            }while( sizeof(Data) == Size );
        }
    }
    
    D_MQC_PRINT( " Program End with %d returned\n\n", Err );
    
    Err = MQC_Stop(&MQCHandler);
    if( D_MQC_RET_OK != Err)
    {
        D_MQC_PRINT( " failed\n  ! MQC_Stop() returned %d\n\n", Err );
    }

    wrapper_deinit(&(UsrData.Platform));
    
    return Err;
}
