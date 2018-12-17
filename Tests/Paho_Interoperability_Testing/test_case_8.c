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
 * @example     test_case_8.c
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

static uint8_t const*   PublishTopic    =  (uint8_t*)"test/not_exist"; 

/**************************************************************
**  Interface
**************************************************************/

/* Connect with server */
extern int32_t TestCase_008(S_USER_DATA* Ctx)
{
    int32_t             Err =   D_MQC_RET_OK;
    int32_t             Ret =   0;
    S_MQC_UTF8_DATA     TopicFilter;
    
    D_MQC_PRINT( " [test_case_%03d]\n", Ctx->CaseNo + 1);
    
    do
    {
        /* Subscribe Topic */
        TopicFilter.Length  =   strlen((char*)PublishTopic);
        TopicFilter.Data    =   (uint8_t*)PublishTopic;

        Err = MQC_Unsubscribe( Ctx->Handler, &TopicFilter, 1, UnSubscribeResponseFunc);
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

extern int32_t TestCase_008_UnSubscribeFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, bool* Complete)
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
        
        if( TopicFilterList->Length != strlen((char*)PublishTopic) )
        {
            D_MQC_PRINT( " failed\n  ! TopicFilter Length is %d\n\n" , TopicFilterList->Length);
            Ret = -1;
            break;
        }
        
        if( memcmp(TopicFilterList->Data, PublishTopic, TopicFilterList->Length) )
        {
            D_MQC_PRINT( " failed\n  ! TopicFilterList is %.*s\n\n" , TopicFilterList->Length, TopicFilterList->Data);
            Ret = -1;
            break;
        }
        
        *Complete = true;
        
        D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);

    }while(0);
    
    return Ret;
}
