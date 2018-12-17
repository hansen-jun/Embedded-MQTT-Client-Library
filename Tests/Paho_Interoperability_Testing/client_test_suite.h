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
 * @example     client_test_suite.h
 * @brief       Client test suite for Paho MQTT Conformance/Interoperability Testing.
 * @author      zhaozhenge@outlook.com
 *
 * @version     00.00.01 
 *              - 2018/12/14 : zhaozhenge@outlook.com 
 *                  -# New
 */
 
#if !defined(PLATFORM_LINUX) && !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_OTHER)  
#error Should indicate a platform for your target (PLATFORM_LINUX or PLATFORM_WINDOWS or PLATFORM_OTHER)
#endif 

/**************************************************************
**  Include
**************************************************************/

#if defined(PLATFORM_LINUX)
#include "../../Platform/Linux/wrapper.h"
#elif defined(PLATFORM_WINDOWS)
#include "../../Platform/Windows/wrapper.h"
#else
#include "../../Platform/Lwip/wrapper.h"
#endif

#include "MQC_api.h"

/**************************************************************
**  Symbol
**************************************************************/

#define D_MQC_MQTT_HOST      "10.3.1.29"            /*!< MQTT test Broker hostname */
#define D_MQC_MQTT_PORT      (1883)                 /*!< MQTT test Broker port */

/**************************************************************
**  Structure
**************************************************************/

/**
 * @brief      Custom User Data
 * @author     zhaozhenge@outlook.com
 * @date       2018/12/14
 */
typedef struct _S_USER_DATA
{
    S_MQC_SESSION_HANDLE*   Handler;
    S_PLATFORM_DATA         Platform;
    bool                    Running;
    uint32_t                CaseNo;
    int                     ConfirmFlag;
}S_USER_DATA;

/**************************************************************
**  Interface
**************************************************************/

extern void lock_callback(void* Ctx);
extern void unlock_callback(void* Ctx);
extern int32_t OpenResetNotify_callback(void* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent);
extern int32_t WriteTcp_callback(void* Ctx, const uint8_t* Data, size_t Size);
extern int32_t ReadNotify_callback(void* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info);
extern int32_t SubscribeResponseFunc(E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum );
extern int32_t UnSubscribeResponseFunc(E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum);
extern int32_t PublishResponseFunc(E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message);

extern int32_t TestCase_001(S_USER_DATA* Ctx);
extern int32_t TestCase_001_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete);
extern int32_t TestCase_002(S_USER_DATA* Ctx);
extern int32_t TestCase_002_SubscribeResponseFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum, bool* Complete);
extern int32_t TestCase_003(S_USER_DATA* Ctx);
extern int32_t TestCase_003_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
extern int32_t TestCase_004(S_USER_DATA* Ctx);
extern int32_t TestCase_004_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
extern int32_t TestCase_004_PublishResponseFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message, bool* Complete);
extern int32_t TestCase_005(S_USER_DATA* Ctx);
extern int32_t TestCase_005_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
extern int32_t TestCase_005_PublishResponseFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message, bool* Complete);
extern int32_t TestCase_006(S_USER_DATA* Ctx);
extern int32_t TestCase_006_UnSubscribeFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, bool* Complete);
extern int32_t TestCase_007(S_USER_DATA* Ctx);
extern int32_t TestCase_007_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
extern int32_t TestCase_008(S_USER_DATA* Ctx);
extern int32_t TestCase_008_UnSubscribeFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, uint32_t ListNum, bool* Complete);
extern int32_t TestCase_009(S_USER_DATA* Ctx);
extern int32_t TestCase_009_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete);
extern int32_t TestCase_010(S_USER_DATA* Ctx);
extern int32_t TestCase_010_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete);
extern int32_t TestCase_010_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
extern int32_t TestCase_010_PublishResponseFunc(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_MESSAGE_INFO* Message, bool* Complete);
extern int32_t TestCase_011(S_USER_DATA* Ctx);
extern int32_t TestCase_011_OpenResetFuncCB(S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, uint8_t SrvResCode, bool SessionPresent, bool* Complete);
extern int32_t TestCase_011_ReadNotify_callback(S_USER_DATA* Ctx, E_MQC_MSG_TYPE Type, S_MQC_MESSAGE_INFO* Info, bool* Complete);
extern int32_t TestCase_011_SubscribeResponseFunc( S_USER_DATA* Ctx, E_MQC_BEHAVIOR_RESULT Result, S_MQC_UTF8_DATA* TopicFilterList, E_MQC_RETURN_CODE* SrvRetCodeList, uint32_t ListNum, bool* Complete);
extern int32_t TestCase_012(S_USER_DATA* Ctx);
