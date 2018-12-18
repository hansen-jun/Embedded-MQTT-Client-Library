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
 * @example     test_case_11.c
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

static uint8_t const*           PublishTopic    =   (uint8_t*)"test/will";
static uint8_t const*           PublishMessage  =   (uint8_t*)"Client 6823e68d-849e-4742-98c2-84093f7c5bf4 Down";

/**************************************************************
**  Interface
**************************************************************/

/* 
    Test Case 11 :  
    1.Close the TCP/IP network (Disconnect with MQTT broker abnormally)
    2.Stop the current MQTT Session.
    3.Restart the TCP/IP network.
    4.Start a new MQTT Session.
    5.Subscribe the will message topic of the stopped session before.
    6.Receive response(CONNACK message) from broker and correctly judge the connect result is OK. 
    7.Receive the will message of the stopped session before and correctly get the message conetent.
*/

extern int32_t TestCase_011(S_USER_DATA* Ctx)
{
    int32_t             Err =   D_MQC_RET_OK;
    int32_t             Ret =   0;
    S_MQC_UTF8_DATA     TopicFilter;
    E_MQC_QOS_LEVEL     QoS;
    
    D_MQC_PRINT( " [test_case_%03d]\n", Ctx->CaseNo + 1);
    
    do
    {
        /* Close Network */
        network_close_wrapper(&(Ctx->Platform));
        
        /* Session Stop */
        (void)MQC_Stop(Ctx->Handler);
        
        /* Open network again */
        if( network_open_wrapper(&(Ctx->Platform)) )
        {
            D_MQC_PRINT( " failed\n  ! network_open_wrapper() Error\n\n" );
            Ret = -1;
            break;
        }
        
        /* Restart Session */
        Err = MQC_Start(Ctx->Handler, systick_wrapper());
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Start() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        Err = MQC_Open(Ctx->Handler, 5000);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Open() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        /* Subscribe Topic */
        TopicFilter.Length  =   strlen((char*)PublishTopic);
        TopicFilter.Data    =   (uint8_t*)PublishTopic;
        QoS                 =   E_MQC_QOS_1;
        
        Err = MQC_Subscribe( Ctx->Handler, &TopicFilter, &QoS, 1, SubscribeResponseFunc);
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Subscribe() returned %d\n\n", Err );
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag =  0x00;
        
    }while(0);
    
    return Ret;
}

extern int32_t TestCase_011_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete)
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
        
        Ctx->ConfirmFlag =  Ctx->ConfirmFlag | 0x02;
        
        if(0x07 == Ctx->ConfirmFlag)
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

extern int32_t TestCase_011_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete)
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
        
        Ctx->ConfirmFlag =  Ctx->ConfirmFlag | 0x01;
        
        if(0x07 == Ctx->ConfirmFlag)
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

extern int32_t TestCase_011_SubscribeResponseFunc( S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum, bool* Complete)
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
        
        if(1 != ListNum)
        {
            D_MQC_PRINT( " failed\n  ! ListNum is %d\n\n" , ListNum);
            Ret = -1;
            break;
        }
        
        if(E_MQC_CODE_FAIL == SrvRetCodeList[0])
        {
            D_MQC_PRINT( " failed\n  ! SrvRetCode is E_MQC_CODE_FAIL\n\n" );
            Ret = -1;
            break;
        }
        
        if( TopicFilterList[0].Length != strlen((char*)PublishTopic) )
        {
            D_MQC_PRINT( " failed\n  ! TopicFilter Length is %d\n\n" , TopicFilterList[0].Length);
            Ret = -1;
            break;
        }
        
        if( memcmp(TopicFilterList[0].Data, PublishTopic, TopicFilterList[0].Length) )
        {
            D_MQC_PRINT( " failed\n  ! TopicFilter is %.*s\n\n" , TopicFilterList[0].Length, TopicFilterList[0].Data);
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag =  Ctx->ConfirmFlag | 0x04;
        
        if(0x07 == Ctx->ConfirmFlag)
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

