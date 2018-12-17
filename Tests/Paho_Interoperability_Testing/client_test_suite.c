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
 * @example     client_test_suite.c
 * @brief       Client test suite for Paho MQTT Conformance/Interoperability Testing.
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
**  Structure
**************************************************************/

/**
 * @brief      Test Case Data
 * @author     zhaozhenge@outlook.com
 * @date       2018/12/14
 */
typedef struct _S_TEST_CASE
{
    int32_t (*Function)(S_USER_DATA* Ctx);
    int32_t (*OpenResetFuncCB)(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete);
    int32_t (*ReadFuncCB)(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
    int32_t (*SubscribeFuncCB)(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum, bool* Complete);
    int32_t (*UnSubscribeFuncCB)(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, bool* Complete);
    int32_t (*PublishFuncCB)(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message, bool* Complete);
}S_TEST_CASE;

/**************************************************************
**  Global Param
**************************************************************/

static S_MQC_SESSION_HANDLE     MQCHandler;
static S_USER_DATA              UsrData;

static const S_TEST_CASE        TestCase[]      = 
{
    {   TestCase_001,   TestCase_001_OpenResetFuncCB    ,   NULL                                ,   NULL                                ,   NULL                            ,   NULL                                },
    {   TestCase_002,   NULL                            ,   NULL                                ,   TestCase_002_SubscribeResponseFunc  ,   NULL                            ,   NULL                                },
    {   TestCase_003,   NULL                            ,   TestCase_003_ReadNotify_callback    ,   NULL                                ,   NULL                            ,   NULL                                },
    {   TestCase_004,   NULL                            ,   TestCase_004_ReadNotify_callback    ,   NULL                                ,   NULL                            ,   TestCase_004_PublishResponseFunc    },
    {   TestCase_005,   NULL                            ,   TestCase_005_ReadNotify_callback    ,   NULL                                ,   NULL                            ,   TestCase_005_PublishResponseFunc    },
    {   TestCase_006,   NULL                            ,   NULL                                ,   NULL                                ,   TestCase_006_UnSubscribeFunc    ,   NULL                                },
    {   TestCase_007,   NULL                            ,   TestCase_007_ReadNotify_callback    ,   NULL                                ,   NULL                            ,   NULL                                },
    {   TestCase_008,   NULL                            ,   NULL                                ,   NULL                                ,   TestCase_008_UnSubscribeFunc    ,   NULL                                },
    {   TestCase_009,   TestCase_009_OpenResetFuncCB    ,   NULL                                ,   NULL                                ,   NULL                            ,   NULL                                },
    {   TestCase_010,   TestCase_010_OpenResetFuncCB    ,   NULL                                ,   NULL                                ,   NULL                            ,   TestCase_010_PublishResponseFunc    },
    {   TestCase_011,   TestCase_011_OpenResetFuncCB    ,   TestCase_011_ReadNotify_callback    ,   TestCase_011_SubscribeResponseFunc  ,   NULL                            ,   NULL                                },
    {   TestCase_012,   NULL                            ,   NULL                                ,   NULL                                ,   NULL                            ,   NULL                                }
};

/**************************************************************
**  Function
**************************************************************/

/** 
 * @brief               Topic subscribe callback function
 * @param[in]           Result              Topic subscribe result
 * @param[in]           TopicFilterList     List of the topic filter which want to be subscribed
 * @param[in]           SrvRetCodeList      List of the topic filter subscribed result from server
 * @param[in]           ListNum             Count of the TopicFilterList and SrvRetCodeList
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/10
 */
extern int32_t SubscribeResponseFunc( E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum )
{
    int32_t Ret         =   0;
    bool    Complete    =   false;
    
    do
    {
        if( !TestCase[UsrData.CaseNo].SubscribeFuncCB )
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(TestCase[UsrData.CaseNo].SubscribeFuncCB(&UsrData, Result, TopicFilterList, SrvRetCodeList, ListNum, &Complete))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(!Complete)
        {
            break;
        }
        
        UsrData.CaseNo++;
        
        if(TestCase[UsrData.CaseNo].Function(&UsrData))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }

    }while(0);
    
    return Ret;
}

extern int32_t UnSubscribeResponseFunc(E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum)
{
    int32_t Ret         =   0;
    bool    Complete    =   false;
    
    do
    {
        if( !TestCase[UsrData.CaseNo].UnSubscribeFuncCB )
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(TestCase[UsrData.CaseNo].UnSubscribeFuncCB(&UsrData, Result, TopicFilterList, ListNum, &Complete))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(!Complete)
        {
            break;
        }
        
        UsrData.CaseNo++;
        
        if(TestCase[UsrData.CaseNo].Function(&UsrData))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }

    }while(0);
    
    return Ret;
}

/** 
 * @brief               Message publish callback function
 * @param[in]           Result              Message Publish result
 * @param[in]           Message             Information of the Message
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
extern int32_t PublishResponseFunc(E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message)
{
    int32_t Ret         =   0;
    bool    Complete    =   false;
    
    do
    {
        if( !TestCase[UsrData.CaseNo].PublishFuncCB )
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(TestCase[UsrData.CaseNo].PublishFuncCB(&UsrData, Result, Message, &Complete))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(!Complete)
        {
            break;
        }
        
        UsrData.CaseNo++;
        
        if(TestCase[UsrData.CaseNo].Function(&UsrData))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }

    }while(0);
    
    return Ret;
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
extern int32_t WriteTcp_callback(void* Ctx, const uint8_t* Data, size_t Size)
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

extern void lock_callback(void* Ctx)
{
    S_USER_DATA*    CustomData  =   (S_USER_DATA*)Ctx;
    lock_wrapper( &(CustomData->Platform) );
}

extern void unlock_callback(void* Ctx)
{
    S_USER_DATA*    CustomData  =   (S_USER_DATA*)Ctx;
    unlock_wrapper( &(CustomData->Platform) );
}

/** 
 * @brief               Open/Reset callback function
 * @param[in,out]       Ctx                 User Context for callback
 * @param[in]           Result              Result of the Connect Behavior
 * @param[in]           SrvResCode          Result of the server response
 * @param[in]           SessionPresent      If server has already hold the MQTT Session
 * @retval              0                   success
 * @retval              -1                  fail
 * @author              zhaozhenge@outlook.com
 * @date                2018/12/03
 */
extern int32_t OpenResetNotify_callback(void* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent)
{
    int32_t Ret         =   0;
    bool    Complete    =   false;
    
    do
    {
        if( !TestCase[UsrData.CaseNo].OpenResetFuncCB )
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(TestCase[UsrData.CaseNo].OpenResetFuncCB(&UsrData, Result, SrvResCode, SessionPresent, &Complete))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(!Complete)
        {
            break;
        }
        
        UsrData.CaseNo++;
        
        if(TestCase[UsrData.CaseNo].Function(&UsrData))
        {
            UsrData.Running = false;
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
extern int32_t ReadNotify_callback(void* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info)
{
    int32_t Ret         =   0;
    bool    Complete    =   false;
    
    do
    {
        if( !TestCase[UsrData.CaseNo].ReadFuncCB )
        {
            break;
        }
        
        if(TestCase[UsrData.CaseNo].ReadFuncCB(&UsrData, Type, Info, &Complete))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }
        
        if(!Complete)
        {
            break;
        }
        
        UsrData.CaseNo++;
        
        if(TestCase[UsrData.CaseNo].Function(&UsrData))
        {
            UsrData.Running = false;
            Ret = -1;
            break;
        }

    }while(0);
    
    return Ret; 
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
    UsrData.CaseNo  =   0;
    
    Err = TestCase[UsrData.CaseNo].Function(&UsrData);
    if(Err)
    {
        wrapper_deinit(&(UsrData.Platform));
        return (-1);
    }
    
    UsrData.Running = true;
    
    while(UsrData.Running)
    {
        Err = tcpcheck_wrapper(&(UsrData.Platform), 100);
        if(0 > Err)
        {
            D_MQC_PRINT( " failed\n  ! tcpcheck_wrapper() returned %d\n\n", Err );
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
    
    (void)MQC_Stop(&MQCHandler);

    wrapper_deinit(&(UsrData.Platform));
    
    return Err;
}
