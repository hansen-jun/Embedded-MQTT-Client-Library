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
 * @example     test_case_9.c
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

static uint8_t const*           ClientId        =   (uint8_t*)"6823e68d-849e-4742-98c2-84093f7c5bf4";
static uint8_t const*           UserName        =   (uint8_t*)"test_client";
static uint8_t const*           Password        =   (uint8_t*)"paho.test";
static uint8_t const*           WillTopic       =   (uint8_t*)"test/will";
static uint8_t const*           WillMessage     =   (uint8_t*)"Client 6823e68d-849e-4742-98c2-84093f7c5bf4 Down";

/**************************************************************
**  Interface
**************************************************************/

/* Connect with server */
extern int32_t TestCase_009(S_USER_DATA* Ctx)
{
    int32_t Err =   D_MQC_RET_OK;
    int32_t Ret =   0;
    
    D_MQC_PRINT( " [test_case_%03d]\n", Ctx->CaseNo + 1);
    
    do
    {
        /* Session Top */
        Err = MQC_Stop( Ctx->Handler );
        if( D_MQC_RET_OK != Err)
        {
            D_MQC_PRINT( " failed\n  ! MQC_Stop() returned %d\n\n", Err );
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
        
        /* Update Configuration */
        Ctx->Handler->CleanSession                      =   false;
        Ctx->Handler->WillMessage.Enable                =   true;
        Ctx->Handler->WillMessage.QoS                   =   E_MQC_QOS_1;
        Ctx->Handler->WillMessage.Retain                =   true;
        Ctx->Handler->WillMessage.Message.Topic.Data    =   (uint8_t*)WillTopic;
        Ctx->Handler->WillMessage.Message.Topic.Length  =   strlen((char*)WillTopic);
        Ctx->Handler->WillMessage.Message.Content       =   (uint8_t*)WillMessage;
        Ctx->Handler->WillMessage.Message.Length        =   strlen((char*)WillMessage);
        Ctx->Handler->Authorition.UsernameEnable        =   true;
        Ctx->Handler->Authorition.Username.Data         =   (uint8_t*)UserName;
        Ctx->Handler->Authorition.Username.Length       =   strlen((char*)UserName);
        Ctx->Handler->Authorition.PasswordEnable        =   true;
        Ctx->Handler->Authorition.Password.Data         =   (uint8_t*)Password;
        Ctx->Handler->Authorition.Password.Length       =   strlen((char*)Password);
        Ctx->Handler->ClientId.Data                     =   (uint8_t*)ClientId;
        Ctx->Handler->ClientId.Length                   =   strlen((char*)ClientId);
        
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
        
        Ctx->ConfirmFlag =  0x00;
        
    }while(0);
    
    return Ret;
}

extern int32_t TestCase_009_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete)
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
        
        if(SessionPresent)
        {
            D_MQC_PRINT( " failed\n  ! SessionPresent is %d\n\n" , SessionPresent);
            Ret = -1;
            break;
        }
        
        *Complete = true;
        
        D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);

    }while(0);
    
    return Ret;
}
