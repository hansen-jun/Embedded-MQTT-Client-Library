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
 * @example     test_case_12.c
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

/**************************************************************
**  Interface
**************************************************************/

/* 
    Test Case 12 :  
    1.Stop the current MQTT Session.
    2.Start a new MQTT Session.
    3.Close the TCP/IP network
    4.Restart the TCP/IP network
    5.Connect to MQTT broker (CONNECT message) with 0 byte client id.
    6.Receive response(CONNACK message) from broker and correctly judge the connect result is OK.   
*/

extern int32_t TestCase_012(S_USER_DATA* Ctx)
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
        Ctx->Handler->CleanSession                      =   true;
        Ctx->Handler->WillMessage.Enable                =   false;
        Ctx->Handler->Authorition.UsernameEnable        =   false;
        Ctx->Handler->Authorition.PasswordEnable        =   false;
        Ctx->Handler->ClientId.Data                     =   NULL;
        Ctx->Handler->ClientId.Length                   =   0;
        
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

extern int32_t TestCase_012_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete)
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
