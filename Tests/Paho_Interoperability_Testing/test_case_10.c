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
 * @example     test_case_10.c
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

static uint8_t const*           PublishTopic    =   (uint8_t*)"test/qos_2";
static uint8_t const*           PublishMessage  =   (uint8_t*)"This is a test message for qos2.";

/**************************************************************
**  Interface
**************************************************************/

/* 
    Test Case 10 :  
    1.Publish a QoS2 message to MQTT broker with retain flag On.
    2.Disconnect with MQTT broker (DISCONNECT message).
    3.Close the TCP/IP network.
    4.Restart the TCP/IP network.
    5.Connect with MQTT broker again with CleanSession Off.
    6.Confirm the publish sequence is OK.
    7.Receive response(CONNACK message) from broker and correctly judge the connect result is OK.
    8.Confirm the library will continue the message publish sequence before disconnect with MQTT broker.
*/

extern int32_t TestCase_010(S_USER_DATA* Ctx)
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
        Err = MQC_Publish(Ctx->Handler, &Message, E_MQC_QOS_2, true, PublishResponseFunc);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Publish() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        /* Session Close */
        Err = MQC_Close(Ctx->Handler);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Close() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        /* Close Network */
        network_close_wrapper(&(Ctx->Platform));
        
        /* Open network again */
        if( network_open_wrapper(&(Ctx->Platform)) )
        {
            D_MQC_PRINT( " failed\n  ! network_open_wrapper() Error\n\n" );
            Ret = -1;
            break;
        }
        
        /* Reopen Session */
        Err = MQC_Open(Ctx->Handler, 5000);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Open() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag =  0x00;
        
    }while(0);
    
    return Ret;
}

extern int32_t TestCase_010_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete)
{
    int32_t Ret =   0;
    
    do
    {
        if(E_MQC_BEHAVIOR_COMPLETE != Result)
        {
            D_MQC_PRINT( " failed\n  ! Connect Result is %d\n\n" , Result);
            Ret = -1;
            break;
        }
        
        if(D_MQC_OPEN_SUCCESS_CODE != SrvResCode)
        {
            D_MQC_PRINT( " failed\n  ! SrvResCode is %d\n\n" , SrvResCode);
            Ret = -1;
            break;
        }
        
        if(!SessionPresent)
        {
            D_MQC_PRINT( " failed\n  ! SessionPresent is %d\n\n" , SessionPresent);
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag =  Ctx->ConfirmFlag | 0x01;
        
        if(0x03 == Ctx->ConfirmFlag)
        {
            *Complete = true;
            D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);
        }
        else
        { 
            *Complete = false;
        }

    }while(0);
    
    return Ret;
}

extern int32_t TestCase_010_PublishResponseFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message, bool* Complete)
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
        
        if( Message->Length != strlen((char*)PublishMessage) )
        {
            D_MQC_PRINT( " failed\n  ! Message Length is %d\n\n" , Message->Length);
            Ret = -1;
            break;
        }
        
        if( memcmp(Message->Content, PublishMessage, Message->Length) )
        {
            D_MQC_PRINT( " failed\n  ! Message is %.*s\n\n" , Message->Length, Message->Content);
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag =  Ctx->ConfirmFlag | 0x02;
        
        if(0x03 == Ctx->ConfirmFlag)
        {
            *Complete = true;
            D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);
        }
        else
        { 
            *Complete = false;
        }
        
    }while(0);
    
    return Ret;
}
