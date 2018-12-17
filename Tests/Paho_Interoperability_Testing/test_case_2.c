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
 * @example     test_case_2.c
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

static uint8_t const*   PublishTopic[]  =   
{
    (uint8_t*)"test/qos_0", 
    (uint8_t*)"test/qos_1", 
    (uint8_t*)"test/qos_2"
};

/**************************************************************
**  Interface
**************************************************************/

/* Connect with server */
extern int32_t TestCase_002(S_USER_DATA* Ctx)
{
    int32_t             Err =   D_MQC_RET_OK;
    int32_t             Ret =   0;
    S_MQC_UTF8_DATA     TopicFilter[3];
    E_MQC_QOS_LEVEL     QoS[3];
    
    D_MQC_PRINT( " [test_case_%03d]\n", Ctx->CaseNo + 1);
    
    do
    {
        /* Subscribe Topic */
        TopicFilter[0].Length  =   strlen((char*)PublishTopic[0]);
        TopicFilter[0].Data    =   (uint8_t*)PublishTopic[0];
        QoS[0]                 =   E_MQC_QOS_0;
        TopicFilter[1].Length  =   strlen((char*)PublishTopic[1]);
        TopicFilter[1].Data    =   (uint8_t*)PublishTopic[1];
        QoS[1]                 =   E_MQC_QOS_1;
        TopicFilter[2].Length  =   strlen((char*)PublishTopic[2]);
        TopicFilter[2].Data    =   (uint8_t*)PublishTopic[2];
        QoS[2]                 =   E_MQC_QOS_2;
        
        Err = MQC_Subscribe( Ctx->Handler, TopicFilter, QoS, 3, SubscribeResponseFunc);
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

extern int32_t TestCase_002_SubscribeResponseFunc( S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum, bool* Complete)
{
    int32_t Ret =   0;
    int     i   =   0;
    
    do
    {
        if(E_MQC_BEHAVIOR_COMPLETE != Result)
        {
            D_MQC_PRINT( " failed\n  ! Connect Result is %d\n\n" , Result);
            Ret = -1;
            break;
        }
        
        if(3 != ListNum)
        {
            D_MQC_PRINT( " failed\n  ! ListNum is %d\n\n" , ListNum);
            Ret = -1;
            break;
        }
        
        for(i = 0; i < 3; i++)
        {
            if(E_MQC_CODE_FAIL == SrvRetCodeList[i])
            {
                D_MQC_PRINT( " failed\n  ! SrvRetCodeList[%d] is E_MQC_CODE_FAIL\n\n" , i);
                Ret = -1;
                break;
            }
            
            if( TopicFilterList[i].Length != strlen((char*)PublishTopic[i]) )
            {
                D_MQC_PRINT( " failed\n  ! TopicFilter[%d] Length is %d\n\n" , i, TopicFilterList[i].Length);
                Ret = -1;
                break;
            }
            
            if( memcmp(TopicFilterList[i].Data, PublishTopic[i], TopicFilterList[i].Length) )
            {
                D_MQC_PRINT( " failed\n  ! TopicFilterList[%d] is %.*s\n\n" , i, TopicFilterList[i].Length, TopicFilterList[i].Data);
                Ret = -1;
                break;
            }
        }
        
        *Complete = true;
        
        D_MQC_PRINT( " [test_case_%03d][retCode=%d]\n", Ctx->CaseNo+1, Ret);

    }while(0);
    
    return Ret;
}
