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
 * @example     test_case_7.c
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

/* Connect with server */
extern int32_t TestCase_007(S_USER_DATA* Ctx)
{
    int32_t             Ret =   0;
    
    D_MQC_PRINT( " [test_case_%03d]\n", Ctx->CaseNo + 1);
    
    do
    {
        Ctx->ConfirmFlag =  0x00;         
    }while(0);
    
    return Ret;
}

extern int32_t TestCase_007_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete)
{
    int32_t Ret =   0;
    
    do
    {
        if(E_MQC_MSG_PINGRESP != Type)
        {
            D_MQC_PRINT( " failed\n  ! Unknown Message Type %d\n\n", Type );
            Ret = -1;
            break;
        }
        
        Ctx->ConfirmFlag++;
        
        if(2 == Ctx->ConfirmFlag)
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

