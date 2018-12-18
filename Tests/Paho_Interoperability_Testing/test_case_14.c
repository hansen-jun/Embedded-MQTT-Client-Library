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
 * @example     test_case_14.c
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

static uint8_t const*   PublishTopic    =   (uint8_t*)"test/qos1";

/**************************************************************
**  Interface
**************************************************************/

/* 
    Test Case 14 :  
    1.Publish a 0 byte QoS1 message to MQTT broker with retain flag on.
    2.Confirm the publish sequence is OK.
*/

extern int32_t TestCase_014(S_USER_DATA* Ctx)
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
        Message.Length          =   0;
        Message.Content         =   NULL;
        Err = MQC_Publish(Ctx->Handler, &Message, E_MQC_QOS_1, true, PublishResponseFunc);
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

extern int32_t TestCase_014_PublishResponseFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message, bool* Complete)
{
    int32_t Ret =   0;
    
    do
    {
        if(E_MQC_BEHAVIOR_COMPLETE != Result)
        {
            D_MQC_PRINT( " failed\n  ! Message Published %d\n\n ", Result);
            Ret = -1;
            break;
        }
        
        if( Message->Topic.Length != strlen((char*)PublishTopic) )
        {
            D_MQC_PRINT( " failed\n  ! Topic Length is %d\n\n" , Message->Topic.Length);
            Ret = -1;
            break;
        }
        
        if( memcmp(Message->Topic.Data, PublishTopic, Message->Topic.Length) )
        {
            D_MQC_PRINT( " failed\n  ! Topic is %.*s\n\n" , Message->Topic.Length, Message->Topic.Data);
            Ret = -1;
            break;
        }
        
        if( 0 != Message->Length )
        {
            D_MQC_PRINT( " failed\n  ! Message Length is %d\n\n" , Message->Length);
            Ret = -1;
            break;
        }
        
        if( NULL != Message->Content )
        {
            D_MQC_PRINT( " failed\n  ! Message is %.*s\n\n" , Message->Length, Message->Content);
            Ret = -1;
            break;
        }
        
        *Complete = true;
        D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);
        
    }while(0);
    
    return Ret;
}

