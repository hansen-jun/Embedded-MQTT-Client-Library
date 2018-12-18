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
 * @example     test_case_3.c
 * @brief       Client test case for Paho MQTT Conformance/Interoperability Testing.
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/12/14 : zhaozhenge@outlook.com 
 *                  -# New
 */
 
/**************************************************************
**  Include
**************************************************************/

#include "client_test_suite.h"

/**************************************************************
**  Global Param
**************************************************************/

static uint8_t const*   PublishTopic    =   (uint8_t*)"test/qos_0";
static uint8_t const*   PublishMessage  =   (uint8_t*)"This is a test message for qos0.";

/**************************************************************
**  Interface
**************************************************************/

/* 
    Test Case 3 :  
    1.Publish a QoS0 message to MQTT broker with retain flag off.
    2.Receive a QoS0 message from broker and correctly get the message content.   
*/

extern int32_t TestCase_003(S_USER_DATA* Ctx)
{
    int32_t             Err =   D_MQC_RET_OK;
    int32_t             Ret =   0;
    S_MQC_MESSAGE_INFO  Message;
    
    D_MQC_PRINT( " [test_case_%03d]\n", Ctx->CaseNo + 1);
    
    do
    {
        /* Publish Message */
        Message.Topic.Length    =   strlen((char*)PublishTopic);
        Message.Topic.Data      =   (uint8_t*)PublishTopic;
        Message.Length          =   strlen((char*)PublishMessage);
        Message.Content         =   (uint8_t*)PublishMessage;
        Err = MQC_Publish(Ctx->Handler, &Message, E_MQC_QOS_0, false, NULL);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Publish() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag =  0x00;
                
    }while(0);
    
    return Ret;
}

extern int32_t TestCase_003_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete)
{
    int32_t Ret =   0;
    
    do
    {
        if(E_MQC_MSG_PINGRESP == Type)
        {
            *Complete = false;
            break;
        }
        
        if(E_MQC_MSG_PUBLISH != Type)
        {
            D_MQC_PRINT( " failed\n  ! Unknown Message Type %d\n\n", Type );
            Ret = -1;
            break;
        }
        
        if( Info->Topic.Length != strlen((char*)PublishTopic) )
        {
            D_MQC_PRINT( " failed\n  ! Topic Length is %d\n\n" , Info->Topic.Length);
            Ret = -1;
            break;
        }
        
        if( memcmp(Info->Topic.Data, PublishTopic, Info->Topic.Length) )
        {
            D_MQC_PRINT( " failed\n  ! Topic is %.*s\n\n" , Info->Topic.Length, Info->Topic.Data);
            Ret = -1;
            break;
        }
        
        if( Info->Length != strlen((char*)PublishMessage) )
        {
            D_MQC_PRINT( " failed\n  ! Message Length is %d\n\n" , Info->Length);
            Ret = -1;
            break;
        }
        
        if( memcmp(Info->Content, PublishMessage, Info->Length) )
        {
            D_MQC_PRINT( " failed\n  ! Message is %.*s\n\n" , Info->Length, Info->Content);
            Ret = -1;
            break;
        }
        
        *Complete = true;
        D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);
        
    }while(0);
    
    return Ret;
}
